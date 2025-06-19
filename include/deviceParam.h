#pragma once

#include "json.hpp"
#include <string>

using json = nlohmann::ordered_json;

enum class DeviceType { Sensor, Light, AirConditioner };

struct DeviceParam {
    DeviceType type;
    std::string name;
    int priorityLevel;
    double powerConsumption;
    int updateFrequency = 1000; // 更新频率(毫秒)，默认1秒

    double lightness = -1.0;   // for Light
    double targetTemperature = -1.0; // for AC
    double speed = -1.0;
};

inline std::string DeviceTypeToStr(DeviceType t) {
    switch (t) {
    case DeviceType::Sensor:
        return "Sensor";
    case DeviceType::Light:
        return "Light";
    case DeviceType::AirConditioner:
        return "AirConditioner";
    }
    return "Unknown";
}

inline void to_json(json &j, const DeviceParam &p) {
    j = json{{"type", DeviceTypeToStr(p.type)},
             {"name", p.name},
             {"priorityLevel", p.priorityLevel},
             {"powerConsumption", p.powerConsumption},
             {"updateFrequency", p.updateFrequency}};

    if (p.lightness != -1.0)
        j["lightness"] = p.lightness;
    if (p.targetTemperature != -1.0)
        j["targetTemperature"] = p.targetTemperature;
    if (p.speed != -1.0)
        j["speed"] = p.speed;
}
