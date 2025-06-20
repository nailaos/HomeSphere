#pragma once

#include "device.h"

class Light : public Device {
  private:
    double lightness;

  public:
    Light(std::string name, int priorityLevel, double powerConsumption,
          double lightness, int updateFrequency = 1000)
        : Device(name, priorityLevel, powerConsumption, updateFrequency), lightness(lightness) {}

    double getLightness() const;
    void setLightness(double lightness);

    DeviceType getDeviceType() const override;
    void update() override;

    json toJson() const override;
};

class LightFactory : public DeviceFactory {
  public:
    Device *createDevice() override;
    Device *createDevice(const json &param) override;
};

class LightContainer : public DeviceContainer<Light> {
  public:
    LightContainer(DeviceFactory *factory) : DeviceContainer<Light>(factory) {}

    void changeDevice(int id) override;
};

inline void to_json(json &j, const Light &l) { j = l.toJson(); }