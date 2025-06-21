#include "sceneSimulation.h"
#include "SmartLogger.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

SceneSimulation::SceneSimulation(Room *room)
    : room(room), running(false), co2(400.0), minuteOfDay(0), 
      emergencyMode(false), emergencyStartTime(0) {}

SceneSimulation::~SceneSimulation() { stop(); }

void SceneSimulation::loadEnvironmentConfig(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        LOG_ALERT_SYS("无法打开环境配置文件: " + filename);
        return;
    }
    ifs >> envConfig;
    ifs.close();
    // 读取目标温湿度
    targetTemperature = envConfig["target_temperature"];
    targetHumidity = envConfig["target_humidity"];
    // 初始值等于目标值
    {
        std::lock_guard<std::mutex> lock(envMutex);
        temperature = targetTemperature;
        humidity = targetHumidity;
        for (auto &ac : room->getAirConditioners()->getDevices()) {
            ac->setTargetTemperature(targetTemperature);
        }
    }
    // 事件
    if (envConfig.contains("events")) {
        events = envConfig["events"].get<std::vector<json>>();
        eventTriggered.assign(events.size(), false);
    }
    LOG_INFO_SYS("环境配置加载完成 - 目标温度: " + std::to_string(targetTemperature) + 
                 "°C, 目标湿度: " + std::to_string(targetHumidity) + "%");
}

void SceneSimulation::start() {
    LOG_INFO_SYS("是否自定义目标温度和湿度？(y/n): ");
    std::string yn;
    std::cin >> yn;
    if (yn == "y" || yn == "Y") {
        LOG_INFO_SYS("请输入目标温度(℃): ");
        std::string tempStr;
        std::cin >> tempStr;
        try {
            targetTemperature = std::stod(tempStr);
        } catch (...) {
        }
        LOG_INFO_SYS("请输入目标湿度(%): ");
        std::string humStr;
        std::cin >> humStr;
        try {
            targetHumidity = std::stod(humStr);
        } catch (...) {
        }
        {
            std::lock_guard<std::mutex> lock(envMutex);
            temperature = targetTemperature;
            humidity = targetHumidity;
            for (auto &ac : room->getAirConditioners()->getDevices()) {
                ac->setTargetTemperature(targetTemperature);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(envMutex);
        temperature = targetTemperature;
        humidity = targetHumidity;
        for (auto &ac : room->getAirConditioners()->getDevices()) {
            ac->setTargetTemperature(targetTemperature);
        }
    }
    running = true;
    minuteOfDay = 0;
    emergencyMode = false;
    emergencyStartTime = 0;
    
    LOG_INFO_SYS("启动场景模拟...");
    envThread = std::thread(&SceneSimulation::environmentThreadFunc, this);
    eventThread = std::thread(&SceneSimulation::eventThreadFunc, this);
    acThread = std::thread(&SceneSimulation::airConditionerThreadFunc, this);
    lightThread = std::thread(&SceneSimulation::lightThreadFunc, this);
    logThread = std::thread(&SceneSimulation::loggingThreadFunc, this);
    emergencyThread = std::thread(&SceneSimulation::emergencyThreadFunc, this);
    sensorThread = std::thread(&SceneSimulation::sensorThreadFunc, this);
    
    while (running && minuteOfDay < 1440) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)); // 100ms推进1分钟
        ++minuteOfDay;
    }
    running = false;
    stop();
}

void SceneSimulation::stop() {
    running = false;
    if (envThread.joinable())
        envThread.join();
    if (eventThread.joinable())
        eventThread.join();
    if (acThread.joinable())
        acThread.join();
    if (lightThread.joinable())
        lightThread.join();
    if (logThread.joinable())
        logThread.join();
    if (emergencyThread.joinable())
        emergencyThread.join();
    if (sensorThread.joinable())
        sensorThread.join();
    LOG_INFO_SYS("场景模拟已停止");
}

void SceneSimulation::environmentThreadFunc() {
    while (running && minuteOfDay < 1440) {
        // 在紧急模式下停止所有环境变化
        if (emergencyMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        // 检查是否有空调在工作
        bool acWorking = false;
        for (auto &ac : room->getAirConditioners()->getDevices()) {
            if (ac->getState()) {
                acWorking = true;
                break;
            }
        }
        
        // 只有在没有空调工作时才进行自然温度变化
        if (!acWorking) {
            double tempBase, humBase;
            {
                std::lock_guard<std::mutex> lock(envMutex);
                tempBase = temperature;
                humBase = targetHumidity;
            }
            double tempAmp = 1.0; // 减小温度变化幅度
            int tempPeak = 14 * 60;
            double t = tempBase +
                       tempAmp * std::sin(2 * M_PI * (minuteOfDay - tempPeak) /
                                          1440.0);
            double humAmp = 1.0; // 减小湿度变化幅度
            int humTrough = 14 * 60;
            double h =
                humBase - humAmp * std::sin(2 * M_PI *
                                             (minuteOfDay - humTrough) / 1440.0);
            {
                std::lock_guard<std::mutex> lock(envMutex);
                temperature = t;
                humidity = h;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

static std::string timeStr(int minuteOfDay) {
    int hour = minuteOfDay / 60;
    int min = minuteOfDay % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ":" << std::setw(2)
        << std::setfill('0') << min;
    return oss.str();
}

void SceneSimulation::eventThreadFunc() {
    while (running && minuteOfDay < 1440) {
        // 在紧急模式下停止事件处理
        if (emergencyMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }
        
        for (size_t i = 0; i < events.size(); ++i) {
            if (!eventTriggered[i] &&
                minuteOfDay == int(events[i]["trigger_time"])) {
                {
                    std::lock_guard<std::mutex> lock(envMutex);
                    temperature += events[i].value("delta_temperature", 0.0);
                    humidity += events[i].value("delta_humidity", 0.0);
                    co2 += events[i].value("delta_co2", 0.0);
                }
                eventTriggered[i] = true;
                // 事件触发时美观输出
                LOG_INFO_SYS("\n********** 事件触发 [" + timeStr(minuteOfDay) + "] **********");
                LOG_INFO_SYS("事件: " + events[i].value("name", "未知事件") + 
                            " (温度" + (events[i].value("delta_temperature", 0.0) >= 0 ? "+" : "") +
                            std::to_string(events[i].value("delta_temperature", 0.0)) +
                            ", 湿度" + (events[i].value("delta_humidity", 0.0) >= 0 ? "+" : "") +
                            std::to_string(events[i].value("delta_humidity", 0.0)) + ", CO2" +
                            (events[i].value("delta_co2", 0.0) >= 0 ? "+" : "") +
                            std::to_string(events[i].value("delta_co2", 0.0)) + ")");
                LOG_INFO_SYS("设备状态变化如下:");

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                // 空调
                LOG_INFO_SYS("空调状态:");
                for (auto &ac : room->getAirConditioners()->getDevices()) {
                    LOG_INFO(ac->getId(), "名称: " + ac->getName() +
                                ", 状态: " + (ac->getState() ? "开" : "关") +
                                ", 目标温度: " + std::to_string(ac->getTargetTemperature()) +
                                ", 模式: " + ac->getMode() +
                                ", 风速: " + std::to_string(ac->getSpeed()));
                }
                LOG_INFO_SYS("灯光状态:");
                for (auto &light : room->getLights()->getDevices()) {
                    LOG_INFO(light->getId(), "名称: " + light->getName() +
                                ", 状态: " + (light->getState() ? "开" : "关") +
                                ", 亮度: " + std::to_string(light->getLightness()) + "%");
                }

                LOG_INFO_SYS("*******************************************\n");
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void SceneSimulation::airConditionerThreadFunc() {
    while (running && minuteOfDay < 1440) {
        // 在紧急模式下停止空调控制
        if (emergencyMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        for (auto &ac : room->getAirConditioners()->getDevices()) {
            // 1. 从传感器获取当前温度，判断是否需要调节
            double currentTemp = 0.0;
            // 获取第一个传感器的温度数据
            for (auto &sensor : room->getSensors()->getDevices()) {
                currentTemp = sensor->getTemperature();
                break; // 只取第一个传感器
            }
            
            // 使用空调自己的目标温度，而不是全局目标温度
            double acTargetTemp = ac->getTargetTemperature();
            double diff = currentTemp - acTargetTemp;

            // 2. 根据温差决定空调状态和模式
            if (std::abs(diff) < 0.5) { // 增大死区，避免频繁开关
                // 温度在可接受范围内，关闭空调
                ac->setState(false);
                ac->setMode("off");
                ac->setSpeed(0);
            } else {
                // 需要调节温度
                ac->setState(true);
                if (diff > 0) {
                    ac->setMode("cool");
                } else {
                    ac->setMode("heat");
                }
                // 根据温差调整风速，使用更平滑的控制
                double speed =
                    std::min(10.0, std::max(1.0, std::abs(diff) * 2.0));
                ac->setSpeed(speed);

                // 3. 根据空调工作效果调整环境
                std::lock_guard<std::mutex> lock(envMutex);
                if (ac->getMode() == "cool") {
                    temperature -= 0.3 * ac->getSpeed(); // 减小调节幅度
                    humidity -= 0.1 * ac->getSpeed();
                } else if (ac->getMode() == "heat") {
                    temperature += 0.3 * ac->getSpeed(); // 减小调节幅度
                    humidity += 0.05 * ac->getSpeed();
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void SceneSimulation::lightThreadFunc() {
    while (running && minuteOfDay < 1440) {
        // 在紧急模式下关闭所有灯光
        if (emergencyMode) {
            for (auto &light : room->getLights()->getDevices()) {
                light->setState(false);
                light->setLightness(0);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        
        int hour = minuteOfDay / 60;
        for (auto &light : room->getLights()->getDevices()) {
            if (hour >= 18 && hour < 24) {
                light->setState(true);
                light->setLightness(80);
            } else {
                light->setState(false);
                light->setLightness(0);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // 23:59后自动关灯
    for (auto &light : room->getLights()->getDevices()) {
        light->setLightness(0);
    }
}

void SceneSimulation::loggingThreadFunc() {
    int lastminuteOfDay = -1;
    while (running && minuteOfDay < 1440) {
        if (minuteOfDay % 30 == 0 && minuteOfDay != lastminuteOfDay) {
            double currentTemp = 0.0, currentHumidity = 0.0, currentCO2 = 0.0;
            int sensorId = -1;
            
            // 获取环境原始数据
            double envTemp, envHumidity, envCO2;
            {
                std::lock_guard<std::mutex> lock(envMutex);
                envTemp = temperature;
                envHumidity = humidity;
                envCO2 = co2;
            }
            
            // 从传感器获取数据
            for (auto &sensor : room->getSensors()->getDevices()) {
                currentTemp = sensor->getTemperature();
                currentHumidity = sensor->getHumidity();
                currentCO2 = sensor->getCO2_Concentration();
                sensorId = sensor->getId();
                break; // 只取第一个传感器
            }
            
            LOG_INFO_SYS("\n================= [ " + timeStr(minuteOfDay) + " ] =================");
            
            if (emergencyMode) {
                LOG_ALERT_SYS("🚨 紧急模式激活 - CO2浓度超标！所有设备已关闭 🚨");
                LOG_ALERT_SYS("紧急模式开始时间: " + timeStr(emergencyStartTime.load()));
                LOG_ALERT_SYS("预计恢复时间: " + timeStr(emergencyStartTime.load() + EMERGENCY_DURATION));
            }
            
            LOG_INFO_SYS("环境状态 (原始数据):");
            LOG_INFO_SYS("  温度: " + std::to_string(envTemp) + " ℃");
            LOG_INFO_SYS("  湿度: " + std::to_string(envHumidity) + " %");
            LOG_INFO_SYS("  CO2: " + std::to_string(envCO2) + " ppm");
            
            LOG_INFO_SYS("传感器读取数据:");
            LOG_INFO(sensorId, "温度: " + std::to_string(currentTemp) + " ℃");
            LOG_INFO(sensorId, "湿度: " + std::to_string(currentHumidity) + " %");
            LOG_INFO(sensorId, "CO2: " + std::to_string(currentCO2) + " ppm");

            LOG_INFO_SYS("空调状态:");
            for (auto &ac : room->getAirConditioners()->getDevices()) {
                LOG_INFO(ac->getId(), "名称: " + ac->getName() +
                            ", 状态: " + (ac->getState() ? "开" : "关") +
                            ", 目标温度: " + std::to_string(ac->getTargetTemperature()) +
                            ", 模式: " + ac->getMode() +
                            ", 风速: " + std::to_string(ac->getSpeed()));
            }
            LOG_INFO_SYS("灯光状态:");
            for (auto &light : room->getLights()->getDevices()) {
                LOG_INFO(light->getId(), "名称: " + light->getName() +
                            ", 状态: " + (light->getState() ? "开" : "关") +
                            ", 亮度: " + std::to_string(light->getLightness()) + "%");
            }

            LOG_INFO_SYS("=============================================");
            lastminuteOfDay = minuteOfDay;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void SceneSimulation::emergencyThreadFunc() {
    while (running && minuteOfDay < 1440) {
        double currentCO2 = 0.0;
        
        // 从传感器获取CO2数据
        for (auto &sensor : room->getSensors()->getDevices()) {
            currentCO2 = sensor->getCO2_Concentration();
            break; // 只取第一个传感器
        }
        
        // 检测CO2浓度是否超标
        if (!emergencyMode && currentCO2 >= CO2_EMERGENCY_THRESHOLD) {
            // 触发紧急模式
            emergencyMode = true;
            emergencyStartTime.store(minuteOfDay);
            
            LOG_ALERT_SYS("🚨 紧急情况！CO2浓度超标！🚨");
            LOG_ALERT_SYS("当前CO2浓度: " + std::to_string(currentCO2) + " ppm (阈值: " + std::to_string(CO2_EMERGENCY_THRESHOLD) + " ppm)");
            LOG_ALERT_SYS("正在执行紧急处理程序...");
            
            // 关闭所有空调
            for (auto &ac : room->getAirConditioners()->getDevices()) {
                ac->setState(false);
                ac->setMode("off");
                ac->setSpeed(0);
                LOG_INFO(ac->getId(), "已关闭空调: " + ac->getName());
            }
            
            // 关闭所有灯光
            for (auto &light : room->getLights()->getDevices()) {
                light->setState(false);
                light->setLightness(0);
                LOG_INFO(light->getId(), "已关闭灯光: " + light->getName());
            }
            
            // 关闭所有传感器
            for (auto &sensor : room->getSensors()->getDevices()) {
                sensor->setState(false);
                LOG_INFO(sensor->getId(), "已关闭传感器: " + sensor->getName());
            }
            
            LOG_ALERT_SYS("全屋断电完成！所有设备已关闭！");
            LOG_ALERT_SYS("紧急模式将在 " + std::to_string(EMERGENCY_DURATION) + " 分钟后自动恢复");
        }
        
        // 检查是否需要恢复
        if (emergencyMode && (minuteOfDay - emergencyStartTime.load()) >= EMERGENCY_DURATION) {
            // 恢复正常模式
            emergencyMode = false;
            
            // 重置CO2浓度为正常值
            {
                std::lock_guard<std::mutex> lock(envMutex);
                co2 = 400.0; // 恢复正常CO2浓度
            }
            
            // 重新开启传感器
            for (auto &sensor : room->getSensors()->getDevices()) {
                sensor->setState(true);
                LOG_INFO(sensor->getId(), "已重新开启传感器: " + sensor->getName());
            }
            
            LOG_INFO_SYS("✅ 紧急模式结束！系统恢复正常运行");
            LOG_INFO_SYS("CO2浓度已重置为正常值: 400 ppm");
            LOG_INFO_SYS("所有设备将恢复正常控制");
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void SceneSimulation::sensorThreadFunc() {
    while (running && minuteOfDay < 1440) {
        // 在紧急模式下停止传感器更新
        if (emergencyMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        
        double currentTemp, currentHumidity, currentCO2;
        {
            std::lock_guard<std::mutex> lock(envMutex);
            currentTemp = temperature;
            currentHumidity = humidity;
            currentCO2 = co2;
        }
        
        // 更新所有传感器的数据
        for (auto &sensor : room->getSensors()->getDevices()) {
            sensor->setTemperature(currentTemp);
            sensor->setHumidity(currentHumidity);
            sensor->setCO2_Concentration(currentCO2);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}