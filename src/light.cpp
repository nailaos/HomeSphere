#include "light.h"
#include <iostream>

double Light::getLightness() const { return lightness; }

void Light::setLightness(double lightness) { this->lightness = lightness; }

DeviceType Light::getDeviceType() const { return DeviceType::Light; }

void Light::update() { return; }

DeviceParam Light::getDeviceParam() const {
    DeviceParam param;
    param.type = DeviceType::Light;
    param.name = name;
    param.priorityLevel = priorityLevel;
    param.powerConsumption = powerConsumption;
    param.lightness = lightness;
    return param;
}

Device *LightFactory::createDevice() {
    Light *light = new Light("Light", 0, 20.0, 0.5);
    return light;
}

Device *LightFactory::createDevice(DeviceParam &param) {
    Light *light = new Light(param.name, param.priorityLevel,
                             param.powerConsumption, param.lightness);
    return light;
}

void LightContainer::displayInfo() {
    std::cout << "Light Container Info" << std::endl;
    std::cout << "=====================\n";
    std::cout << "Number of lights: " << size << std::endl;
    for (int i = 0; i < size; i++) {
        std::cout << "Light " << i << ":\n";
        std::cout << "ID: " << devices[i]->getId() << std::endl;
        std::cout << "Name: " << devices[i]->getName() << std::endl;
        std::cout << "Priority Level: " << devices[i]->getPriorityLevel()
                  << std::endl;
        std::cout << "Power Consumption: " << devices[i]->getPowerConsumption()
                  << std::endl;
        std::cout << "Lightness: " << devices[i]->getLightness() << std::endl;
    }
    std::cout << "=====================\n";
    return;
}