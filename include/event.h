#pragma once

#include "device.h"
#include <chrono>
#include <string>

class Event {
  public:
    static int nextId;
    int id;
    std::string name;
    std::chrono::system_clock::time_point timestamp;
    int deviceId;
    DeviceType deviceType;
    bool state;
};

class SensorEvent : public Event {
  public:
    double temperature;
    double humidity;
    double CO2_Concentration;
};

class LightEvent : public Event {
  public:
    double lightness;
};

class AirConditionerEvent : public Event {
  public:
    double temperature;
    double targetTemperature;
    double speed;
};