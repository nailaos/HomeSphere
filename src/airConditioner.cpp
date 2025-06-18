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
    // check common parameters
    check(param);

    // check specific parameters
    // TODO: 查看device.cpp和light.cpp以提供帮助

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double targetTemperature = param["targetTemperature"];
    double speed = param["speed"];

    return new AirConditioner(name, priorityLevel, powerConsumption,
                              targetTemperature, speed);
}

Device *AirConditionerFactory::createDevice(DeviceParam &param) {
    json j = param;
    return createDevice(j);
}