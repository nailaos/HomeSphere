#pragma once

#include "device.h"

class Light : public Device {
  private:
    double lightness;

  public:
    Light(std::string name, int priorityLevel, double powerConsumption,
          double lightness)
        : Device(name, priorityLevel, powerConsumption), lightness(lightness) {}

    double getLightness() const;
    void setLightness(double lightness);

    DeviceType getDeviceType() const override;
    void update() override;
    DeviceParam getDeviceParam() const override;
};

class LightFactory : public DeviceFactory {
  public:
    Device *createDevice() override;
    Device *createDevice(DeviceParam &param) override;
};

class LightContainer : public DeviceContainer<Light> {
  public:
    void displayInfo() override;
};