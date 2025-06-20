#include "user.h"
#include "light.h"
#include "sensor.h"
#include "airConditioner.h"
#include <iostream>

void Admin::show() {
    std::cout << "管理员: " << name << std::endl;
    // TODO: 输出可管理的设备范围
    std::cout << "可管理的设备范围: " << DeviceTypeToString(DeviceType::Sensor) << ", " << DeviceTypeToString(DeviceType::Light) << ", " << DeviceTypeToString(DeviceType::AirConditioner) << std::endl;
}

void Admin::manageDevice(Device* device) {
    
    // 可以添加对所有设备的管理逻辑
}

bool LightAdmin::canChangeDevice(Device *device) {
    if (device->getType() == DeviceType::Light) {
        return true;
    }
    return false;
}

void SensorAdmin::manageDevice(Device* device) {
    if (dynamic_cast<Sensor*>(device)) {
        std::cout << name << " 管理传感器设备: " << device->getName() << std::endl;
        // 传感器设备管理逻辑
    } else {
        std::cout << name << " 无权管理该设备: " << device->getName() << std::endl;
    }
}

void AcAdmin::manageDevice(Device* device) {
    if (dynamic_cast<AirConditioner*>(device)) {
        std::cout << name << " 管理空调设备: " << device->getName() << std::endl;
        // 空调设备管理逻辑
    } else {
        std::cout << name << " 无权管理该设备: " << device->getName() << std::endl;
    }
}

void Visitor::manageDevice(Device* device) {
    std::cout << name << " 仅有查看权限，无法管理设备: " << device->getName() << std::endl;
} 