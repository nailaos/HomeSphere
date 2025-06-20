#include "sceneSimulation.h"
#include "SmartLogger.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

SceneSimulation::SceneSimulation(Room *room)
    : room(room), running(false), emergencyMode(false), fireDetected(false),
      gasLeakDetected(false), highTempDetected(false),
      fireHandled(false), gasLeakHandled(false), highTempHandled(false),
      gen(std::mt19937(
          std::chrono::system_clock::now().time_since_epoch().count())),
      logIntervalMs(5000), simDurationMin(2), autoStop(true) {

    temperature = 25.0;
    humidity = 60.0;
    co2 = 400.0;
    lightIntensity = 500.0;
    hour = 14;
    minute = 30;
    occupancy = 2;
    scenario = "afternoon";

    std::cout << "SceneSimulation::SceneSimulation()" << std::endl;
    lastLogTime = std::chrono::steady_clock::now();
}

SceneSimulation::~SceneSimulation() { stop(); }

void SceneSimulation::loadEnvironmentConfig(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        LOG_ALERT_SYS("无法打开环境配置文件: " + filename);
        return;
    }
    ifs >> envConfig;
    ifs.close();
    // 环境参数初始化
    auto &env = envConfig["environment"];
    temperature = env["temperature"]["current"];
    humidity = env["humidity"]["current"];
    co2 = env["CO2_Concentration"]["current"];
    lightIntensity = env["light_intensity"]["current"];
    hour = env["time_of_day"]["hour"];
    minute = env["time_of_day"]["minute"];
    occupancy = env["occupancy"]["current"];
    scenario = "afternoon";
    // 模拟配置
    if (envConfig.contains("simulation_config")) {
        auto &simConfig = envConfig["simulation_config"];
        simDurationMin = simConfig.value("duration_minutes", 2);
        autoStop = simConfig.value("auto_stop", true);
        logIntervalMs = simConfig.value("log_interval", 500);
    }
    // 事件
    if (envConfig.contains("emergency_events")) {
        scheduledEvents =
            envConfig["emergency_events"].get<std::vector<json>>();
        eventTriggered.assign(scheduledEvents.size(), false);
    }
    LOG_INFO_SYS("环境配置加载成功");
}

void SceneSimulation::start() {
    if (running.load())
        return;
    running = true;
    emergencyMode = false;
    fireDetected = gasLeakDetected = highTempDetected = false;
    fireHandled = gasLeakHandled = highTempHandled = false;
    lastLogTime = std::chrono::steady_clock::now();
    LOG_INFO_SYS("场景模拟启动");
    envThread = std::thread(&SceneSimulation::environmentThreadFunc, this);
    sensorThread = std::thread(&SceneSimulation::sensorThreadFunc, this);
    lightThread = std::thread(&SceneSimulation::lightThreadFunc, this);
    acThread = std::thread(&SceneSimulation::airConditionerThreadFunc, this);
    automationThread =
        std::thread(&SceneSimulation::automationThreadFunc, this);
    emergencyThread = std::thread(&SceneSimulation::emergencyThreadFunc, this);
    logThread = std::thread(&SceneSimulation::loggingThreadFunc, this);
    // 主线程等待模拟结束
    auto startTime = std::chrono::steady_clock::now();
    while (running.load()) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::minutes>(now - startTime);
        if (autoStop && elapsed.count() >= simDurationMin) {
            LOG_INFO_SYS("模拟时间到，自动停止");
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    stop();
}

void SceneSimulation::stop() {
    if (!running.load())
        return;
    running = false;
    cv.notify_all();
    if (envThread.joinable())
        envThread.join();
    if (sensorThread.joinable())
        sensorThread.join();
    if (lightThread.joinable())
        lightThread.join();
    if (acThread.joinable())
        acThread.join();
    if (automationThread.joinable())
        automationThread.join();
    if (emergencyThread.joinable())
        emergencyThread.join();
    if (logThread.joinable())
        logThread.join();
    LOG_INFO_SYS("场景模拟已停止");
}

// ================= 线程实现 =================
void SceneSimulation::environmentThreadFunc() {
    while (running.load()) {
        {
            std::lock_guard<std::mutex> lock(envMutex);
            updateEnvironment();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

void SceneSimulation::sensorThreadFunc() {
    while (running.load()) {
        auto *sensors = room->getSensors();
        for (auto &sensor : sensors->getDevices()) {
            double t, h, c, l;
            {
                std::lock_guard<std::mutex> lock(envMutex);
                t = temperature;
                h = humidity;
                c = co2;
                l = lightIntensity;
            }
            if (sensor->getName().find("温度") != std::string::npos) {
                sensor->setTemperature(t);
                LOG_DEBUG(sensor->getId(),
                          "温度传感器更新: " + std::to_string(t) + "°C");
            } else if (sensor->getName().find("湿度") != std::string::npos) {
                sensor->setHumidity(h);
                LOG_DEBUG(sensor->getId(),
                          "湿度传感器更新: " + std::to_string(h) + "%");
            } else if (sensor->getName().find("CO2") != std::string::npos) {
                sensor->setCO2_Concentration(c);
                LOG_DEBUG(sensor->getId(),
                          "CO2传感器更新: " + std::to_string(c) + " ppm");
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

void SceneSimulation::lightThreadFunc() {
    while (running.load()) {
        auto *lights = room->getLights();
        for (auto &light : lights->getDevices()) {
            if (light->getLightness() > 0) {
                LOG_DEBUG(light->getId(),
                          "灯光运行中，亮度: " +
                              std::to_string(light->getLightness()) + "%");
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

void SceneSimulation::airConditionerThreadFunc() {
    while (running.load()) {
        auto *acs = room->getAirConditioners();
        for (auto &ac : acs->getDevices()) {
            if (ac->getSpeed() > 0) {
                LOG_DEBUG(ac->getId(),
                          "空调运行中，目标温度: " +
                              std::to_string(ac->getTargetTemperature()) +
                              "°C, 速度: " + std::to_string(ac->getSpeed()));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

void SceneSimulation::automationThreadFunc() {
    while (running.load()) {
        if (!emergencyMode.load()) {
            applyAutomationRules();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }
}

void SceneSimulation::emergencyThreadFunc() {
    while (running.load()) {
        handleEmergencies();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void SceneSimulation::loggingThreadFunc() {
    while (running.load()) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastLogTime);
        if (elapsed.count() >= logIntervalMs) {
            double t, h, c, l;
            int occ;
            std::string sc;
            {
                std::lock_guard<std::mutex> lock(envMutex);
                t = temperature;
                h = humidity;
                c = co2;
                l = lightIntensity;
                occ = occupancy;
                sc = scenario;
            }
            std::ostringstream oss;
            oss << "[" << getRealTimeStr() << "] [模拟时间: " << getSimTimeStr()
                << "] 环境状态 - "
                << "温度: " << std::fixed << std::setprecision(1) << t << "°C, "
                << "湿度: " << h << "%, CO2: " << c << " ppm, 光照: " << l
                << " lux, 人数: " << occ << ", 场景: " << sc;
            if (emergencyMode)
                oss << " [紧急模式]";
            std::cout << oss.str() << std::endl;
            LOG_INFO_SYS(oss.str());
            lastLogTime = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

// ================= 业务实现 =================
void SceneSimulation::updateEnvironment() {
    // 时间推进
    minute++;
    if (minute >= 60) {
        minute = 0;
        hour++;
        if (hour >= 24)
            hour = 0;
    }
    // 场景切换
    if (hour >= 6 && hour < 12)
        scenario = "morning";
    else if (hour >= 12 && hour < 18)
        scenario = "afternoon";
    else if (hour >= 18 && hour < 22)
        scenario = "evening";
    else
        scenario = "night";
    // 环境参数变化
    auto &sc = envConfig["scenarios"][scenario];
    double tempTarget = sc["temperature"]["target"];
    double tempVar = sc["temperature"]["variation"];
    temperature +=
        (tempTarget - temperature) * 0.1 + (gen() % 100 - 50) * tempVar / 100.0;
    double humTarget = sc["humidity"]["target"];
    double humVar = sc["humidity"]["variation"];
    humidity +=
        (humTarget - humidity) * 0.1 + (gen() % 100 - 50) * humVar / 100.0;
    double co2Target = sc["CO2_Concentration"]["target"];
    double co2Var = sc["CO2_Concentration"]["variation"];
    co2 += (co2Target - co2) * 0.05 + (gen() % 100 - 50) * co2Var / 100.0;
    double lightTarget = sc["light_intensity"]["target"];
    double lightVar = sc["light_intensity"]["variation"];
    lightIntensity += (lightTarget - lightIntensity) * 0.1 +
                      (gen() % 100 - 50) * lightVar / 100.0;
    int occTarget = sc["occupancy"]["target"];
    int occVar = sc["occupancy"]["variation"];
    if (gen() % 100 < 10)
        occupancy = std::max(
            0, std::min(10, occupancy + ((int)(gen()) % (occVar * 2 + 1)) -
                                occVar));
    // 触发定时事件
    triggerScheduledEvents();
}

void SceneSimulation::triggerScheduledEvents() {
    int nowMin = hour * 60 + minute;
    for (size_t i = 0; i < scheduledEvents.size(); ++i) {
        if (eventTriggered[i])
            continue;
        int triggerTime = scheduledEvents[i]["trigger_time"];
        if (nowMin >= triggerTime) {
            eventTriggered[i] = true;
            std::string type = scheduledEvents[i]["type"];
            std::string name = scheduledEvents[i]["name"];
            LOG_ALERT_SYS("突发事件触发: " + name + " (" + type + ")");
            if (type == "fire")
                fireDetected = true;
            else if (type == "gas_leak")
                gasLeakDetected = true;
            else if (type == "high_temperature")
                highTempDetected = true;
        }
    }
}

void SceneSimulation::handleEmergencies() {
    if (fireDetected || gasLeakDetected || highTempDetected) {
        emergencyMode = true;
        applyEmergencyActions();
    } else {
        emergencyMode = false;
    }
}

void SceneSimulation::applyAutomationRules() {
    double t, h, c, l;
    int occ;
    {
        std::lock_guard<std::mutex> lock(envMutex);
        t = temperature;
        h = humidity;
        c = co2;
        l = lightIntensity;
        occ = occupancy;
    }
    // 温度
    auto *acs = room->getAirConditioners();
    for (auto &ac : acs->getDevices()) {
        if (t > 28.0) {
            ac->setTargetTemperature(24.0);
            ac->setSpeed(3);
            LOG_INFO(ac->getId(), "温度过高，空调制冷24°C，速度3");
        } else if (t < 18.0) {
            ac->setTargetTemperature(26.0);
            ac->setSpeed(2);
            LOG_INFO(ac->getId(), "温度过低，空调制热26°C，速度2");
        } else {
            ac->setTargetTemperature(22.0);
            ac->setSpeed(1);
            LOG_DEBUG(ac->getId(), "温度正常，空调舒适22°C，速度1");
        }
    }
    // 湿度
    for (auto &ac : acs->getDevices()) {
        if (h > 70.0) {
            ac->setTargetTemperature(ac->getTargetTemperature() - 1.0);
            LOG_INFO(ac->getId(), "湿度高，空调温度-1°C除湿");
        } else if (h < 40.0) {
            ac->setTargetTemperature(ac->getTargetTemperature() + 1.0);
            LOG_INFO(ac->getId(), "湿度低，空调温度+1°C加湿");
        }
    }
    // 光照
    auto *lights = room->getLights();
    for (auto &light : lights->getDevices()) {
        if (l < 300.0) {
            light->setLightness(80);
            LOG_INFO(light->getId(), "光照不足，灯光80%");
        } else if (l > 800.0) {
            light->setLightness(20);
            LOG_INFO(light->getId(), "光照充足，灯光20%");
        } else {
            light->setLightness(50);
            LOG_DEBUG(light->getId(), "光照正常，灯光50%");
        }
    }
    // 占用
    if (occ == 0) {
        for (auto &light : lights->getDevices()) {
            light->setLightness(0);
            LOG_INFO(light->getId(), "无人，灯光关闭");
        }
        for (auto &ac : acs->getDevices()) {
            ac->setSpeed(0);
            LOG_INFO(ac->getId(), "无人，空调关闭");
        }
    } else if (occ > 3) {
        for (auto &light : lights->getDevices()) {
            light->setLightness(80);
            LOG_INFO(light->getId(), "人多，灯光80%");
        }
        for (auto &ac : acs->getDevices()) {
            ac->setSpeed(3);
            LOG_INFO(ac->getId(), "人多，空调速度3");
        }
    }
    // CO2
    if (c > 1000.0) {
        for (auto &ac : acs->getDevices()) {
            ac->setSpeed(3);
            LOG_ALERT(ac->getId(), "CO2超标，空调最大通风");
        }
    }
}

void SceneSimulation::applyEmergencyActions() {
    auto *lights = room->getLights();
    auto *acs = room->getAirConditioners();
    if (fireDetected && !fireHandled) {
        for (auto &light : lights->getDevices()) {
            light->setLightness(0);
            LOG_ALERT(light->getId(), "火灾紧急-灯光断电");
        }
        for (auto &ac : acs->getDevices()) {
            ac->setSpeed(0);
            LOG_ALERT(ac->getId(), "火灾紧急-空调断电");
        }
        logDeviceStates("火灾紧急处理后");
        fireHandled = true;
    } else if (gasLeakDetected && !gasLeakHandled) {
        for (auto &light : lights->getDevices()) {
            light->setLightness(0);
            LOG_ALERT(light->getId(), "燃气泄漏-灯光断电");
        }
        for (auto &ac : acs->getDevices()) {
            ac->setSpeed(0);
            LOG_ALERT(ac->getId(), "燃气泄漏-空调断电");
        }
        logDeviceStates("燃气泄漏紧急处理后");
        gasLeakHandled = true;
    } else if (highTempDetected && !highTempHandled) {
        for (auto &light : lights->getDevices()) {
            light->setLightness(0);
            LOG_ALERT(light->getId(), "高温-灯光关闭降热");
        }
        for (auto &ac : acs->getDevices()) {
            ac->setTargetTemperature(18.0);
            ac->setSpeed(3);
            LOG_ALERT(ac->getId(), "高温-空调全功率18°C");
        }
        logDeviceStates("高温紧急处理后");
        highTempHandled = true;
    }
}

void SceneSimulation::logDeviceStates(const std::string &context) {
    std::ostringstream oss;
    oss << "[" << context << "] 设备状态:";
    auto *lights = room->getLights();
    for (auto &light : lights->getDevices()) {
        oss << "\n  灯光: ID:" << light->getId() << " 名称:" << light->getName()
            << " 亮度:" << light->getLightness() << "%";
    }
    auto *acs = room->getAirConditioners();
    for (auto &ac : acs->getDevices()) {
        oss << "\n  空调: ID:" << ac->getId() << " 名称:" << ac->getName()
            << " 目标温度:" << ac->getTargetTemperature()
            << "°C 速度:" << ac->getSpeed();
    }
    auto *sensors = room->getSensors();
    for (auto &sensor : sensors->getDevices()) {
        oss << "\n  传感器: ID:" << sensor->getId()
            << " 名称:" << sensor->getName();
    }
    LOG_INFO_SYS(oss.str());
    std::cout << oss.str() << std::endl;
}

std::string SceneSimulation::getSimTimeStr() {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hour << ":" << std::setfill('0')
        << std::setw(2) << minute;
    return oss.str();
}
std::string SceneSimulation::getRealTimeStr() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}