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

    return new Light(name, priorityLevel, powerConsumption, lightness,
                     updateFrequency);
}

void LightContainer::changeDevice(int id) {
    Light *light = dynamic_cast<Light *>(getDevice(id));
    if (light) {
        std::cout << "当前设备信息为：\n";
        std::cout << light->toJson().dump(4) << std::endl;
        std::cout << "请输入新的设备优先级：\n";
        int priorityLevel;
        std::cin >> priorityLevel;
        light->setPriorityLevel(priorityLevel);
        std::cout << "请输入新的亮度(0-100)：\n";
        double lightness;
        std::cin >> lightness;
        light->setLightness(lightness);
        std::cout << "请输入新的更新频率(ms)：\n";
        int updateFrequency;
        std::cin >> updateFrequency;
        light->setUpdateFrequency(updateFrequency);
        std::cout << "更改后当前设备信息：\n";
        std::cout << light->toJson().dump(4) << std::endl;
    } else {
        std::cerr << "Device with id " << id << " not found" << std::endl;
    }
}