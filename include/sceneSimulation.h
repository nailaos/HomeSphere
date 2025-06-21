#pragma once

#include "json.hpp"
#include "room.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using json = nlohmann::ordered_json;

class Room;

class SceneSimulation {
  public:
    SceneSimulation(Room *room);
    ~SceneSimulation();

    // 加载环境与事件配置
    void loadEnvironmentConfig(const std::string &filename);
    // 启动/停止模拟
    void start();
    void stop();

  private:
    Room *room;
    std::atomic<bool> running;

    // 环境参数
    double temperature;
    double humidity;
    double co2;
    double targetTemperature;
    double targetHumidity;

    // 紧急状态管理
    std::atomic<bool> emergencyMode;
    std::atomic<int> emergencyStartTime;             // 紧急模式开始时间（分钟）
    static const int CO2_EMERGENCY_THRESHOLD = 1000; // CO2紧急阈值
    static const int EMERGENCY_DURATION = 10;        // 紧急模式持续时间（分钟）

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
    std::thread emergencyThread; // 新增紧急处理线程
    std::thread sensorThread;    // 新增传感器线程

    // 互斥锁保护共享环境参数
    mutable std::mutex envMutex;

    // 线程函数
    void environmentThreadFunc();
    void eventThreadFunc();
    void airConditionerThreadFunc();
    void lightThreadFunc();
    void loggingThreadFunc();
    void emergencyThreadFunc(); // 新增紧急处理线程函数
    void sensorThreadFunc();    // 新增传感器线程函数

    // 时间推进
    std::atomic<int> minuteOfDay;
};