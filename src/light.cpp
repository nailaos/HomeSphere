#include "light.h"
#include <iostream>

double Light::getLightness() const { return lightness; }

void Light::setLightness(double lightness) { this->lightness = lightness; }

DeviceType Light::getDeviceType() const { return DeviceType::Light; }

void Light::update() { return; }

json Light::toJson() const {
    return {{"id", id},
            {"name", name},
            {"priorityLevel", priorityLevel},
            {"powerConsumption", powerConsumption},
            {"lightness", lightness}};
}

Device *LightFactory::createDevice() {
    Light *light = new Light("Light", 0, 20.0, 0.5);
    return light;
}

Device *LightFactory::createDevice(const json &param) {
    if (!param.contains("name") || !param.contains("priorityLevel") ||
        !param.contains("powerConsumption") || !param.contains("lightness")) {
        std::cerr << "Invalid Light config: " << param.dump() << std::endl;
        return nullptr;
    }

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double lightness = param["lightness"];

    return new Light(name, priorityLevel, powerConsumption, lightness);
}

Device* LightFactory::createDevice(DeviceParam &param) {
    return new Light(param.name, param.priorityLevel, param.powerConsumption, param.lightness);
}