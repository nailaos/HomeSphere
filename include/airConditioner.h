#pragma once

#include "device.h"
#include <string>

class AirConditioner : public Device {
  private:
    double targetTemperature;
    double speed;
    std::string mode; // "off", "cool", "heat"

  public:
    AirConditioner(std::string name, int priorityLevel, double powerConsumption,
                   double temperature, double speed, int updateFrequency = 1000)
        : Device(name, priorityLevel, powerConsumption, updateFrequency),
          targetTemperature(temperature), speed(speed), mode("off") {};

    double getTargetTemperature() const;
    double getSpeed() const;
    std::string getMode() const;
    void setMode(const std::string &mode);

    void setTargetTemperature(double temperature);
    void setSpeed(double speed);

    DeviceType getDeviceType() const override;
    void update() override;

    json toJson() const override;
};

class AirConditionerFactory : public DeviceFactory {
  public:
    Device *createDevice() override;
    Device *createDevice(const json &param) override;
};

class AirConditionerContainer : public DeviceContainer<AirConditioner> {
  public:
    AirConditionerContainer(DeviceFactory *factory)
        : DeviceContainer<AirConditioner>(factory) {}

    void changeDevice(int id) override;
};

inline void to_json(json &j, const AirConditioner &ac) { j = ac.toJson(); }