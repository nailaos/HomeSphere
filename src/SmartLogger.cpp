#include "SmartLogger.h"
#include <iostream>
#include <iomanip>

// 日志级别字符串
std::string LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::ALERT: return "ALERT";
        default: return "UNKNOWN";
    }
}

void ConsoleOutputter::write(const std::string& message) {
    std::cout << message << std::endl;
}

FileOutputter::FileOutputter(const std::string& filename) {
    fileStream.open(filename, std::ios::app);
}
FileOutputter::~FileOutputter() {
    if (fileStream.is_open()) fileStream.close();
}
void FileOutputter::write(const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex);
    if (fileStream.is_open()) {
        fileStream << message << std::endl;
        fileStream.flush();
    }
}

SmartLogger* SmartLogger::instance = nullptr;
std::mutex SmartLogger::instanceMutex;

SmartLogger::SmartLogger() : minLevel(LogLevel::DEBUG) {
    outputters.push_back(std::make_unique<ConsoleOutputter>());
}

SmartLogger* SmartLogger::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    if (instance == nullptr) {
        instance = new SmartLogger();
    }
    return instance;
}

void SmartLogger::setMinLevel(LogLevel level) {
    minLevel = level;
}

void SmartLogger::addOutputter(std::unique_ptr<LogOutputter> outputter) {
    std::lock_guard<std::mutex> lock(loggerMutex);
    outputters.push_back(std::move(outputter));
}

void SmartLogger::log(LogLevel level, int deviceId, const std::string& message) {
    if (level < minLevel) return;
    std::lock_guard<std::mutex> lock(loggerMutex);
    std::ostringstream oss;
    oss << "[" << getCurrentTime() << "] "
        << "[" << LogLevelToString(level) << "] "
        << "[Device:" << deviceId << "] "
        << "[Thread:" << getThreadId() << "] "
        << message;
    std::string logMessage = oss.str();
    for (auto& outputter : outputters) {
        outputter->write(logMessage);
    }
}

void SmartLogger::log(LogLevel level, const std::string& message) {
    log(level, -1, message);
}

std::string SmartLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string SmartLogger::getThreadId() {
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
} 