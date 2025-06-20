#include "sceneSimulation.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

SceneSimulation::SceneSimulation(Room *room)
    : room(room), running(false), co2(400.0), minuteOfDay(0) {}

SceneSimulation::~SceneSimulation() { stop(); }

void SceneSimulation::loadEnvironmentConfig(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "无法打开环境配置文件: " << filename << std::endl;
        return;
    }
    ifs >> envConfig;
    ifs.close();
    // 读取目标温湿度
    targetTemperature = envConfig["target_temperature"];
    targetHumidity = envConfig["target_humidity"];
    // 初始值等于目标值
    temperature = targetTemperature;
    humidity = targetHumidity;
    // 事件
    if (envConfig.contains("events")) {
        events = envConfig["events"].get<std::vector<json>>();
        eventTriggered.assign(events.size(), false);
    }
}

void SceneSimulation::start() {
    std::cout << "是否自定义目标温度和湿度？(y/n): ";
    std::string yn;
    std::cin >> yn;
    if (yn == "y" || yn == "Y") {
        std::cout << "请输入目标温度(℃): ";
        std::string tempStr;
        std::cin >> tempStr;
        try {
            targetTemperature = std::stod(tempStr);
        } catch (...) {
        }
        std::cout << "请输入目标湿度(%): ";
        std::string humStr;
        std::cin >> humStr;
        try {
            targetHumidity = std::stod(humStr);
        } catch (...) {
        }
        temperature = targetTemperature;
        humidity = targetHumidity;
    }
    running = true;
    minuteOfDay = 0;
    envThread = std::thread(&SceneSimulation::environmentThreadFunc, this);
    eventThread = std::thread(&SceneSimulation::eventThreadFunc, this);
    acThread = std::thread(&SceneSimulation::airConditionerThreadFunc, this);
    lightThread = std::thread(&SceneSimulation::lightThreadFunc, this);
    logThread = std::thread(&SceneSimulation::loggingThreadFunc, this);
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
}

void SceneSimulation::environmentThreadFunc() {
    while (running && minuteOfDay < 1440) {
        double tempBase = targetTemperature;
        double tempAmp = 2.5;
        int tempPeak = 14 * 60;
        double t =
            tempBase +
            tempAmp * std::sin(2 * M_PI * (minuteOfDay - tempPeak) / 1440.0);
        double humBase = targetHumidity;
        double humAmp = 15.0;
        int humTrough = 14 * 60;
        double h =
            humBase -
            humAmp * std::sin(2 * M_PI * (minuteOfDay - humTrough) / 1440.0);
        {
            temperature = t;
            humidity = h;
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
        for (size_t i = 0; i < events.size(); ++i) {
            if (!eventTriggered[i] &&
                minuteOfDay == int(events[i]["trigger_time"])) {
                {
                    temperature += events[i].value("delta_temperature", 0.0);
                    humidity += events[i].value("delta_humidity", 0.0);
                    co2 += events[i].value("delta_co2", 0.0);
                }
                eventTriggered[i] = true;
                // 事件触发时美观输出
                std::cout << "\n********** 事件触发 [" << timeStr(minuteOfDay)
                          << "] **********\n";
                std::cout << "事件: " << events[i].value("name", "未知事件")
                          << " (温度"
                          << (events[i].value("delta_temperature", 0.0) >= 0
                                  ? "+"
                                  : "")
                          << events[i].value("delta_temperature", 0.0)
                          << ", 湿度"
                          << (events[i].value("delta_humidity", 0.0) >= 0 ? "+"
                                                                          : "")
                          << events[i].value("delta_humidity", 0.0) << ", CO2"
                          << (events[i].value("delta_co2", 0.0) >= 0 ? "+" : "")
                          << events[i].value("delta_co2", 0.0) << ")\n";
                std::cout << "设备状态变化如下:\n";

                // 空调
                std::cout << "空调状态:\n";
                for (auto &ac : room->getAirConditioners()->getDevices()) {
                    std::cout << "  - ID: " << ac->getId()
                              << ", 名称: " << ac->getName()
                              << ", 状态: " << (ac->getState() ? "开" : "关")
                              << ", 目标温度: " << ac->getTargetTemperature()
                              << ", 模式: " << ac->getMode()
                              << ", 风速: " << ac->getSpeed() << "\n";
                }
                std::cout << "灯光状态:\n";
                for (auto &light : room->getLights()->getDevices()) {
                    std::cout << "  - ID: " << light->getId()
                              << ", 名称: " << light->getName()
                              << ", 状态: " << (light->getState() ? "开" : "关")
                              << ", 亮度: " << light->getLightness() << "%\n";
                }

                std::cout << "*******************************************\n\n"
                          << std::flush;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void SceneSimulation::airConditionerThreadFunc() {
    while (running && minuteOfDay < 1440) {
        for (auto &ac : room->getAirConditioners()->getDevices()) {
            // 1. 根据上一次空调状态调整环境
            if (ac->getState()) {
                if (ac->getMode() == "cool") {
                    temperature -= 0.5 * ac->getSpeed();
                    humidity -= 0.2 * ac->getSpeed();
                } else if (ac->getMode() == "heat") {
                    temperature += 0.5 * ac->getSpeed();
                    humidity += 0.1 * ac->getSpeed();
                }
            }
            // 2. 判断是否需要开/关空调，设置模式和风速
            double diff = temperature - targetTemperature;
            if (std::abs(diff) < 0.5) {
                ac->setState(false);
                ac->setMode("off");
                ac->setSpeed(0);
            } else {
                ac->setState(true);
                if (diff > 0) {
                    ac->setMode("cool");
                } else {
                    ac->setMode("heat");
                }
                double speed =
                    std::min(10.0, std::max(1.0, std::abs(diff) * 1.5 + 1.0));
                ac->setSpeed(speed);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void SceneSimulation::lightThreadFunc() {
    while (running && minuteOfDay < 1440) {
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
            std::cout << "\n================= [ " << timeStr(minuteOfDay)
                      << " ] =================\n";
            std::cout << "环境状态:\n  温度: " << std::fixed
                      << std::setprecision(1) << temperature << " ℃\n"
                      << "  湿度: " << humidity << " %\n"
                      << "  CO2: " << co2 << " ppm\n";

            std::cout << "空调状态:\n";
            for (auto &ac : room->getAirConditioners()->getDevices()) {
                std::cout << "  - ID: " << ac->getId()
                          << ", 名称: " << ac->getName()
                          << ", 状态: " << (ac->getState() ? "开" : "关")
                          << ", 目标温度: " << ac->getTargetTemperature()
                          << ", 模式: " << ac->getMode()
                          << ", 风速: " << ac->getSpeed() << "\n";
            }
            std::cout << "灯光状态:\n";
            for (auto &light : room->getLights()->getDevices()) {
                std::cout << "  - ID: " << light->getId()
                          << ", 名称: " << light->getName()
                          << ", 状态: " << (light->getState() ? "开" : "关")
                          << ", 亮度: " << light->getLightness() << "%\n";
            }

            std::cout << "=============================================\n"
                      << std::flush;
            lastminuteOfDay = minuteOfDay;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}