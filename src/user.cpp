#include "user.h"
#include "airConditioner.h"
#include "light.h"
#include "sensor.h"
#include <iostream>

void Admin::show() {
    std::cout << "管理员: " << name << std::endl;
    std::cout << "可管理的设备范围: " << DeviceTypeToStr(DeviceType::Sensor)
              << ", " << DeviceTypeToStr(DeviceType::Light) << ", "
              << DeviceTypeToStr(DeviceType::AirConditioner) << std::endl;
}

void LightAdmin::show() {
    std::cout << "光照管理员: " << name << std::endl;
    std::cout << "可管理的设备范围: " << DeviceTypeToStr(DeviceType::Light)
              << std::endl;
}

void SensorAdmin::show() {
    std::cout << "传感器管理员: " << name << std::endl;
    std::cout << "可管理的设备范围: " << DeviceTypeToStr(DeviceType::Sensor)
              << std::endl;
}

void AirConditionerAdmin::show() {
    std::cout << "空调管理员: " << name << std::endl;
    std::cout << "可管理的设备范围: "
              << DeviceTypeToStr(DeviceType::AirConditioner) << std::endl;
}

void Visitor::show() {
    std::cout << "访客: " << name << std::endl;
    std::cout << "不可管理任何设备" << std::endl;
}

bool LightAdmin::canChangeDevice(Device *device) {
    if (device->getDeviceType() == DeviceType::Light) {
        return true;
    }
    return false;
}

bool SensorAdmin::canChangeDevice(Device *device) {
    if (device->getDeviceType() == DeviceType::Sensor) {
        return true;
    }
    return false;
}

bool AirConditionerAdmin::canChangeDevice(Device *device) {
    if (device->getDeviceType() == DeviceType::AirConditioner) {
        return true;
    }
    return false;
}

bool Admin::canChangeDevice(Device *device) { return true; }

bool Visitor::canChangeDevice(Device *device) { return false; }