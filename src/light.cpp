#include "light.h"
#include "common.h"
#include "exception.h"
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
            {"updateFrequency", updateFrequency},
            {"lightness", lightness}};
}

Device *LightFactory::createDevice() {
    Light *light = new Light("Light", 0, 20.0, 0.5);
    return light;
}

Device *LightFactory::createDevice(const json &param) {

    // check common parameters
    check(param);

    // check light specific parameters
    if (!param.contains("lightness")) {
        throw InvalidParameterException(param,
                                        "Missing required field: lightness");
    }

    if (!param["lightness"].is_number()) {
        throw InvalidParameterException(param, "'lightness' must be a number");
    }

    if (param["lightness"].get<double>() < 0 ||
        param["lightness"].get<double>() > MAX_LIGHTNESS) {
        throw InvalidParameterException(param,
                                        "'lightness' must be between 0 and " +
                                            std::to_string(MAX_LIGHTNESS));
    }

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double lightness = param["lightness"];
    
    // 获取updateFrequency，如果不存在则使用默认值
    int updateFrequency = param.value("updateFrequency", 1000);

    return new Light(name, priorityLevel, powerConsumption, lightness, updateFrequency);
}