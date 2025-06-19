#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <iostream>
#include <vector>

// 日志级别枚举
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    ALERT = 2
};

std::string LogLevelToString(LogLevel level);

// 抽象日志输出器基类
class LogOutputter {
public:
    virtual ~LogOutputter() = default;
    virtual void write(const std::string& message) = 0;
};

// 控制台输出器
class ConsoleOutputter : public LogOutputter {
public:
    void write(const std::string& message) override;
};

// 文件输出器
class FileOutputter : public LogOutputter {
private:
    std::ofstream fileStream;
    std::mutex fileMutex;
public:
    FileOutputter(const std::string& filename);
    ~FileOutputter();
    void write(const std::string& message) override;
};

// 主日志类
class SmartLogger {
private:
    static SmartLogger* instance;
    static std::mutex instanceMutex;
    std::vector<std::unique_ptr<LogOutputter>> outputters;
    std::mutex loggerMutex;
    LogLevel minLevel;
    SmartLogger();
    std::string getCurrentTime();
    std::string getThreadId();
public:
    static SmartLogger* getInstance();
    void setMinLevel(LogLevel level);
    void addOutputter(std::unique_ptr<LogOutputter> outputter);
    void log(LogLevel level, int deviceId, const std::string& message);
    void log(LogLevel level, const std::string& message);
};

// 宏定义简化调用
#define LOG_DEBUG(deviceId, message) SmartLogger::getInstance()->log(LogLevel::DEBUG, deviceId, message)
#define LOG_INFO(deviceId, message) SmartLogger::getInstance()->log(LogLevel::INFO, deviceId, message)
#define LOG_ALERT(deviceId, message) SmartLogger::getInstance()->log(LogLevel::ALERT, deviceId, message)

#define LOG_DEBUG_SYS(message) SmartLogger::getInstance()->log(LogLevel::DEBUG, message)
#define LOG_INFO_SYS(message) SmartLogger::getInstance()->log(LogLevel::INFO, message)
#define LOG_ALERT_SYS(message) SmartLogger::getInstance()->log(LogLevel::ALERT, message) 