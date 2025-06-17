#pragma once

#include "device.h"

class AirConditioner : public Device {
  private:
    double targetTemperature;
    double speed;

  public:
    AirConditioner(std::string name, int priorityLevel, double powerConsumption,
                   double temperature, double speed)
        : Device(name, priorityLevel, powerConsumption),
          targetTemperature(temperature), speed(speed) {};

    double getTargetTemperature() const;
    double getSpeed() const;

    void setTargetTemperature(double temperature);
    void setSpeed(double speed);

    DeviceType getDeviceType() const override;
    void update() override;
    DeviceParam getDeviceParam() const override;
};

class AirConditionerFactory : public DeviceFactory {
  public:
    Device *createDevice() override;
    Device *createDevice(DeviceParam &param) override;
};

class AirConditionerContainer : public DeviceContainer<AirConditioner> {
  public:
    void displayInfo() override;
};