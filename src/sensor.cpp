#include "sensor.h"
#include <iostream>

double Sensor::getTemperature() const { return temperature; }

double Sensor::getHumidity() const { return humidity; }

double Sensor::getCO2_Concentration() const { return CO2_Concentration; }

DeviceType Sensor::getDeviceType() const { return DeviceType::Sensor; }

void Sensor::update() { return; }

DeviceParam Sensor::getDeviceParam() const {
    DeviceParam param;
    param.type = DeviceType::Sensor;
    param.name = name;
    param.priorityLevel = priorityLevel;
    param.powerConsumption = powerConsumption;
    return param;
}

Device *SensorFactory::createDevice() {
    Sensor *sensor = new Sensor("Sensor", 0, 2.0);
    return sensor;
}

Device *SensorFactory::createDevice(DeviceParam &param) {
    Sensor *sensor =
        new Sensor(param.name, param.priorityLevel, param.powerConsumption);
    return sensor;
}

void SensorContainer::displayInfo() {
    std::cout << "Sensor Container Info" << std::endl;
    std::cout << "=====================\n";
    std::cout << "Number of sensors: " << size << std::endl;
    for (int i = 0; i < size; i++) {
        std::cout << "Sensor " << i << ":\n";
        std::cout << "ID: " << devices[i]->getId() << std::endl;
        std::cout << "Name: " << devices[i]->getName() << std::endl;
        std::cout << "Priority Level: " << devices[i]->getPriorityLevel()
                  << std::endl;
        std::cout << "Power Consumption: " << devices[i]->getPowerConsumption()
                  << std::endl;
        std::cout << "Temperature: " << devices[i]->getTemperature()
                  << std::endl;
        std::cout << "Humidity: " << devices[i]->getHumidity() << std::endl;
        std::cout << "CO2 Concentration: " << devices[i]->getCO2_Concentration()
                  << std::endl;
    }
    std::cout << "=====================\n";
    return;
}