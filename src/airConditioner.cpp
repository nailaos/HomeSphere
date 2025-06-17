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

DeviceParam AirConditioner::getDeviceParam() const {
    DeviceParam param;
    param.type = DeviceType::AirConditioner;
    param.name = "Air Conditioner";
    param.priorityLevel = priorityLevel;
    param.powerConsumption = powerConsumption;
    param.targetTemperature = targetTemperature;
    param.speed = speed;
    return param;
}

Device *AirConditionerFactory::createDevice() {
    AirConditioner *air_conditioner =
        new AirConditioner("Air Conditioner", 0, 100, 25, 1.0);
    return air_conditioner;
}

Device *AirConditionerFactory::createDevice(DeviceParam &param) {
    AirConditioner *air_conditioner = new AirConditioner(
        param.name, param.priorityLevel, param.powerConsumption,
        param.targetTemperature, param.speed);
    return air_conditioner;
}

void AirConditionerContainer::displayInfo() {
    std::cout << "Air Conditioner Container Info" << std::endl;
    std::cout << "=====================\n";
    std::cout << "Number of air conditioners: " << size << std::endl;
    for (int i = 0; i < size; i++) {
        std::cout << "Light " << i << ":\n";
        std::cout << "ID: " << devices[i]->getId() << std::endl;
        std::cout << "Name: " << devices[i]->getName() << std::endl;
        std::cout << "Priority Level: " << devices[i]->getPriorityLevel()
                  << std::endl;
        std::cout << "Power Consumption: " << devices[i]->getPowerConsumption()
                  << std::endl;
        std::cout << "Target Temperature: "
                  << devices[i]->getTargetTemperature() << std::endl;
        std::cout << "Speed: " << devices[i]->getSpeed() << std::endl;
    }
    std::cout << "=====================\n";
    return;
}