#pragma once

#include "device.h"

class Sensor : public Device {
  private:
    double temperature;
    double humidity;
    double CO2_Concentration;

  public:
    Sensor(std::string name, int priorityLevel, double powerConsumption, int updateFrequency = 1000)
        : Device(name, priorityLevel, powerConsumption, updateFrequency), temperature(-1.0),
          humidity(-1.0), CO2_Concentration(-1.0) {}

    Sensor(std::string name, int priorityLevel, double powerConsumption,
           double temperature, double humidity, double CO2_Concentration, int updateFrequency = 1000)
        : Device(name, priorityLevel, powerConsumption, updateFrequency),
          temperature(temperature), humidity(humidity),
          CO2_Concentration(CO2_Concentration) {}

    double getTemperature() const;
    double getHumidity() const;
    double getCO2_Concentration() const;

    void setTemperature(double temperature);
    void setHumidity(double humidity);
    void setCO2_Concentration(double CO2_Concentration);

    DeviceType getDeviceType() const override;
    void update() override;

    json toJson() const override;
};

class SensorFactory : public DeviceFactory {
  public:
    Device *createDevice() override;
    Device *createDevice(const json &param) override;
};

class SensorContainer : public DeviceContainer<Sensor> {
  public:
    SensorContainer(DeviceFactory *factory)
        : DeviceContainer<Sensor>(factory) {}

    void changeDevice(int id) override;
};

inline void to_json(json &j, const Sensor &s) { j = s.toJson(); }