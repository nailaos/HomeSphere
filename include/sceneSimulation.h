#pragma once

#include "room.h"
#include "json.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <random>
#include <chrono>

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
    // 线程函数
    void environmentThreadFunc();
    void sensorThreadFunc();
    void lightThreadFunc();
    void airConditionerThreadFunc();
    void automationThreadFunc();
    void emergencyThreadFunc();
    void loggingThreadFunc();

    // 环境与事件处理
    void updateEnvironment();
    void triggerScheduledEvents();
    void handleEmergencies();
    void applyAutomationRules();
    void applyEmergencyActions();
    void logDeviceStates(const std::string& context);

    // 工具
    std::string getSimTimeStr();
    std::string getRealTimeStr();

    // 数据成员
    Room* room;
    std::atomic<bool> running;
    std::atomic<bool> emergencyMode;

    // 环境参数
    double temperature;
    double humidity;
    double co2;
    double lightIntensity;
    int hour;
    int minute;
    int occupancy;
    std::string scenario;

    // 配置与事件
    json envConfig;
    std::vector<json> scheduledEvents;
    std::vector<bool> eventTriggered;

    // 紧急状态
    bool fireDetected;
    bool gasLeakDetected;
    bool highTempDetected;

    // 线程
    std::thread envThread;
    std::thread sensorThread;
    std::thread lightThread;
    std::thread acThread;
    std::thread automationThread;
    std::thread emergencyThread;
    std::thread logThread;

    // 同步
    std::mutex envMutex;
    std::condition_variable cv;

    // 随机
    std::mt19937 gen;
    std::random_device rd;

    // 日志
    std::chrono::steady_clock::time_point lastLogTime;
    int logIntervalMs;
    int simDurationMin;
    bool autoStop;

    bool fireHandled;
    bool gasLeakHandled;
    bool highTempHandled;
}; 