#include "sensor.h"
#include <iostream>

double Sensor::getTemperature() const { return temperature; }

double Sensor::getHumidity() const { return humidity; }

double Sensor::getCO2_Concentration() const { return CO2_Concentration; }

DeviceType Sensor::getDeviceType() const { return DeviceType::Sensor; }

void Sensor::update() { return; }

json Sensor::toJson() const {
    return {{"id", id},
            {"name", name},
            {"priorityLevel", priorityLevel},
            {"powerConsumption", powerConsumption},
            {"temperature", temperature},
            {"humidity", humidity},
            {"CO2_Concentration", CO2_Concentration}};
}

    Device *SensorFactory::createDevice() {
    Sensor *sensor = new Sensor("Sensor", 0, 2.0);
    return sensor;
}

Device *SensorFactory::createDevice(const json &param) {
    // check common parameters
    check(param);

    // check specific parameters
    // TODO: 查看device.cpp和light.cpp以提供帮助

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double temperature = param["temperature"];
    double humidity = param["humidity"];
    double co2 = param["CO2_Concentration"];

    return new Sensor(name, priorityLevel, powerConsumption, temperature,
                      humidity, co2);
}

Device *SensorFactory::createDevice(DeviceParam &param) {
    json j = param;
    return createDevice(j);
}