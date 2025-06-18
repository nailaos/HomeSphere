#include "airConditioner.h"
#include <iostream>

double AirConditioner::getTargetTemperature() const {
    return targetTemperature;
}

double AirConditioner::getSpeed() const { return speed; }

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
            {"targetTemperature", targetTemperature},
            {"speed", speed}};
}

Device *AirConditionerFactory::createDevice() {
    AirConditioner *air_conditioner =
        new AirConditioner("Air Conditioner", 0, 100, 25, 1.0);
    return air_conditioner;
}

Device *AirConditionerFactory::createDevice(const json &param) {
    if (!param.contains("name") || !param.contains("priorityLevel") ||
        !param.contains("powerConsumption") ||
        !param.contains("targetTemperature") || !param.contains("speed")) {
        std::cerr << "Invalid AirConditioner config: " << param.dump()
                  << std::endl;
        return nullptr;
    }

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double targetTemperature = param["targetTemperature"];
    double speed = param["speed"];

    return new AirConditioner(name, priorityLevel, powerConsumption,
                              targetTemperature, speed);
}

Device *AirConditionerFactory::createDevice(DeviceParam &param) {
    return new AirConditioner(param.name, param.priorityLevel,
                              param.powerConsumption, param.targetTemperature,
                              param.speed);
}