#include "airConditioner.h"
#include "common.h"
#include <iostream>

double AirConditioner::getTargetTemperature() const {
    return targetTemperature;
}

double AirConditioner::getSpeed() const { return speed; }

std::string AirConditioner::getMode() const { return mode; }
void AirConditioner::setMode(const std::string &m) { mode = m; }

void AirConditioner::setTargetTemperature(double temperature) {
    targetTemperature = temperature;
}

void AirConditioner::setSpeed(double speed) { this->speed = speed; }

DeviceType AirConditioner::getDeviceType() const {
    return DeviceType::AirConditioner;
}

void AirConditioner::update() { return; }

json AirConditioner::toJson() const {
    return {{"id", id},
            {"name", name},
            {"priorityLevel", priorityLevel},
            {"powerConsumption", powerConsumption},
            {"updateFrequency", updateFrequency},
            {"targetTemperature", targetTemperature},
            {"speed", speed},
            {"mode", mode}};
}

Device *AirConditionerFactory::createDevice() {
    AirConditioner *air_conditioner =
        new AirConditioner("Air Conditioner", 0, 100, 25, 1.0);
    return air_conditioner;
}

Device *AirConditionerFactory::createDevice(const json &param) {
    // check common parameters
    check(param);

    // check specific parameters
    // TODO: 查看device.cpp和light.cpp以提供帮助
    if (!param.contains("targetTemperature")) {
        throw InvalidParameterException(
            param, "Missing required field: targetTemperature");
    }
    if (!param.contains("speed")) {
        throw InvalidParameterException(param, "Missing required field: speed");
    }
    if (!param["targetTemperature"].is_number()) {
        throw InvalidParameterException(param,
                                        "targetTemperature must be a number");
    }
    if (!param["speed"].is_number()) {
        throw InvalidParameterException(param, "speed must be a number");
    }
    if (param["targetTemperature"] < MIN_AIR_CONDITIONER_TEMPERATURE ||
        param["targetTemperature"] > MAX_AIR_CONDITIONER_TEMPERATURE) {
        throw InvalidParameterException(
            param, "targetTemperature must be between " +
                       std::to_string(MIN_AIR_CONDITIONER_TEMPERATURE) +
                       " and " +
                       std::to_string(MAX_AIR_CONDITIONER_TEMPERATURE));
    }
    if (param["speed"] < 0 || param["speed"] > MAX_AIR_CONDITIONER_SPEED) {
        throw InvalidParameterException(
            param, "speed must be between " + std::to_string(0) + " and " +
                       std::to_string(MAX_AIR_CONDITIONER_SPEED));
    }

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double targetTemperature = param["targetTemperature"];
    double speed = param["speed"];
    
    // 获取updateFrequency，如果不存在则使用默认值
    int updateFrequency = param.value("updateFrequency", 1000);

    return new AirConditioner(name, priorityLevel, powerConsumption,
                              targetTemperature, speed, updateFrequency);
}

void AirConditionerContainer::changeDevice(int id) {
    AirConditioner *ac = dynamic_cast<AirConditioner *>(getDevice(id));
    if (ac) {
        std::cout << "当前设备信息为：\n";
        std::cout << ac->toJson().dump(4) << std::endl;
        std::cout << "请输入新的设备优先级：\n";
        int priorityLevel;
        std::cin >> priorityLevel;
        ac->setPriorityLevel(priorityLevel);
        std::cout << "请输入新的目标温度：\n";
        double targetTemperature;
        std::cin >> targetTemperature;
        ac->setTargetTemperature(targetTemperature);
        std::cout << "请输入新的速度：\n";
        double speed;
        std::cin >> speed;
        ac->setSpeed(speed);
        std::cout << "请输入新的更新频率(ms)：\n";
        int updateFrequency;
        std::cin >> updateFrequency;
        ac->setUpdateFrequency(updateFrequency);
        std::cout << "更改后当前设备信息：\n";
        std::cout << ac->toJson().dump(4) << std::endl;
    } else {
        std::cerr << "Device with id " << id << " not found" << std::endl;
    }
}