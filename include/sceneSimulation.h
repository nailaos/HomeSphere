#pragma once

#include "room.h"
#include "json.hpp"
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>

using json = nlohmann::ordered_json;

class Room;

class SceneSimulation {
public:
    SceneSimulation(Room* room);
    ~SceneSimulation();

    // 加载环境与事件配置
    void loadEnvironmentConfig(const std::string& filename);
    // 启动/停止模拟
    void start();
    void stop();

private:
    Room* room;
    std::atomic<bool> running;

    // 环境参数
    double temperature;
    double humidity;
    double co2;
    double targetTemperature;
    double targetHumidity;

    // 事件
    std::vector<json> events;
    std::vector<bool> eventTriggered;
    json envConfig;

    // 线程
    std::thread envThread;
    std::thread eventThread;
    std::thread acThread;
    std::thread lightThread;
    std::thread logThread;

    // 线程函数
    void environmentThreadFunc();
    void eventThreadFunc();
    void airConditionerThreadFunc();
    void lightThreadFunc();
    void loggingThreadFunc();

    // 时间推进
    std::atomic<int> minuteOfDay;
}; 