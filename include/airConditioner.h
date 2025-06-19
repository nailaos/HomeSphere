#pragma once

#include "device.h"

class AirConditioner : public Device {
  private:
    double targetTemperature;
    double speed;

  public:
    AirConditioner(std::string name, int priorityLevel, double powerConsumption,
                   double temperature, double speed, int updateFrequency = 1000)
        : Device(name, priorityLevel, powerConsumption, updateFrequency),
          targetTemperature(temperature), speed(speed) {};

    double getTargetTemperature() const;
    double getSpeed() const;

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
};

inline void to_json(json &j, const AirConditioner &ac) { j = ac.toJson(); }