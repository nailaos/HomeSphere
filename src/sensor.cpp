#include "sensor.h"
#include "common.h"
#include <iostream>

double Sensor::getTemperature() const { return temperature; }

double Sensor::getHumidity() const { return humidity; }

double Sensor::getCO2_Concentration() const { return CO2_Concentration; }

void Sensor::setTemperature(double temperature) { this->temperature = temperature; }

void Sensor::setHumidity(double humidity) { this->humidity = humidity; }

void Sensor::setCO2_Concentration(double CO2_Concentration) { this->CO2_Concentration = CO2_Concentration; }

DeviceType Sensor::getDeviceType() const { return DeviceType::Sensor; }

void Sensor::update() { return; }

json Sensor::toJson() const {
    return {{"id", id},
            {"name", name},
            {"priorityLevel", priorityLevel},
            {"powerConsumption", powerConsumption},
            {"updateFrequency", updateFrequency},
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
    if (!param.contains("temperature")) {
        throw InvalidParameterException(param, "Missing required field: temperature");
    }
    if (!param.contains("humidity")) {
        throw InvalidParameterException(param, "Missing required field: humidity");
    }
    if (!param.contains("CO2_Concentration")) {
        throw InvalidParameterException(param, "Missing required field: CO2_Concentration");
    }
    if (!param["temperature"].is_number()) {
        throw InvalidParameterException(param, "temperature must be a number");
    }
    if (!param["humidity"].is_number()) {
        throw InvalidParameterException(param, "humidity must be a number");
    }
    if (!param["CO2_Concentration"].is_number()) {
        throw InvalidParameterException(param, "CO2_Concentration must be a number");
    }
    if (param["temperature"] < MIN_TEMPERATURE || param["temperature"] > MAX_TEMPERATURE) {
        throw InvalidParameterException(param, "temperature must be between " + std::to_string(MIN_TEMPERATURE) + " and " + std::to_string(MAX_TEMPERATURE));
    }
    if (param["humidity"] < 0 || param["humidity"] > MAX_HUMIDITY) {
        throw InvalidParameterException(param, "humidity must be between " + std::to_string(0) + " and " + std::to_string(MAX_HUMIDITY));
    }
    if (param["CO2_Concentration"] < 0 || param["CO2_Concentration"] > MAX_CO2_CONCENTRATION) {
        throw InvalidParameterException(param, "CO2_Concentration must be between " + std::to_string(0) + " and " + std::to_string(MAX_CO2_CONCENTRATION));
    }

    std::string name = param["name"];
    int priorityLevel = param["priorityLevel"];
    double powerConsumption = param["powerConsumption"];
    double temperature = param["temperature"];
    double humidity = param["humidity"];
    double co2 = param["CO2_Concentration"];
    
    // 获取updateFrequency，如果不存在则使用默认值
    int updateFrequency = param.value("updateFrequency", 1000);

    return new Sensor(name, priorityLevel, powerConsumption, temperature,
                      humidity, co2);
}

void SensorContainer::changeDevice(int id) {
    Sensor *sensor = dynamic_cast<Sensor *>(getDevice(id));
    if (sensor) {
        std::cout << "当前设备信息为：\n";
        std::cout << sensor->toJson().dump(4) << std::endl;
        std::cout << "请输入新的设备优先级：\n";
        int priorityLevel;
        std::cin >> priorityLevel;
        sensor->setPriorityLevel(priorityLevel);
        std::cout << "请输入新的温度：\n";
        double temperature;
        std::cin >> temperature;
        sensor->setTemperature(temperature);
        std::cout << "请输入新的湿度：\n";
        double humidity;
        std::cin >> humidity;
        sensor->setHumidity(humidity);
        std::cout << "请输入新的CO2浓度：\n";
        double co2;
        std::cin >> co2;
        sensor->setCO2_Concentration(co2);
        std::cout << "请输入新的更新频率(ms)：\n";
        int updateFrequency;
        std::cin >> updateFrequency;
        sensor->setUpdateFrequency(updateFrequency);
        std::cout << "更改后当前设备信息：\n";
        std::cout << sensor->toJson().dump(4) << std::endl;
    } else {
        std::cerr << "Device with id " << id << " not found" << std::endl;
    }
}

