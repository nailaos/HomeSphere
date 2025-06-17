#pragma once

#include "device.h"

class Sensor : public Device {
  private:
    double temperature;
    double humidity;
    double CO2_Concentration;

  public:
    Sensor(std::string name, int priorityLevel, double powerConsumption)
        : Device(name, priorityLevel, powerConsumption), temperature(-1.0),
          humidity(-1.0), CO2_Concentration(-1.0) {}

    double getTemperature() const;
    double getHumidity() const;
    double getCO2_Concentration() const;

    DeviceType getDeviceType() const override;
    void update() override;
    DeviceParam getDeviceParam() const override;
};

class SensorFactory : public DeviceFactory {
  public:
    Device *createDevice() override;
    Device *createDevice(DeviceParam &param) override;
};

class SensorContainer : public DeviceContainer<Sensor> {
  public:
    void displayInfo() override;
};