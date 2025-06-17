#include "room.h"
#include <fstream>
#include <vector>

using json = nlohmann::json;

void Room::init() { return; }

void Room::printCurrentUser() {
    std::cout << "Current user: \n";
    std::cout << "Admin\n";
}

void Room::addDevicesFromFile() {
    std::cout << "Add devices from file\n";
    std::cout << "请输入文件名称(data文件夹里): \n";
    std::string filename;
    std::cin >> filename;

    std::string json_path = "../data/" + filename + ".json";

    std::ifstream ifs(json_path);
    json j;
    ifs >> j;
    std::vector<DeviceParam> device_params = j.get<std::vector<DeviceParam>>();
    for (auto device_param : device_params) {
        switch (device_param.type) {
        case DeviceType::Sensor: {
            Device *sensor = sensorFactory.createDevice(device_param);
            sensors.addDevice(static_cast<Sensor *>(sensor));
            break;
        }
        case DeviceType::Light: {
            Device *light = lightFactory.createDevice(device_param);
            lights.addDevice(static_cast<Light *>(light));
            break;
        }
        case DeviceType::AirConditioner: {
            Device *air_conditioner =
                airConditionerFactory.createDevice(device_param);
            airConditioners.addDevice(
                static_cast<AirConditioner *>(air_conditioner));
            break;
        }
        default:
            break;
        }
    }
}

void Room::addDevices() {
    std::cout << "Add devices\n";
    int n;
    std::cout << "请输入设备数量: \n";
    std::cin >> n;
    for (int i = 0; i < n; i++) {
        DeviceParam device_param;
        std::cout << "请输入设备类型(0: 传感器, 1: 灯, 2: 空调): \n";
        int t;
        std::cin >> t;
        device_param.type = static_cast<DeviceType>(t);
        std::cout << "请输入设备名称: \n";
        std::cin >> device_param.name;
        std::cout << "请输入设备优先级: \n";
        std::cin >> device_param.priorityLevel;
        std::cout << "请输入设备功耗: \n";
        std::cin >> device_param.powerConsumption;

        switch (device_param.type) {
        case DeviceType::Light: {
            std::cout << "请输入灯光亮度: \n";
            std::cin >> device_param.lightness;
            Device *light = lightFactory.createDevice(device_param);
            lights.addDevice(static_cast<Light *>(light));
            break;
        }
        case DeviceType::AirConditioner: {
            std::cout << "请输入目标温度: \n";
            std::cin >> device_param.targetTemperature;
            std::cout << "请输入空调速度: \n";
            std::cin >> device_param.speed;
            Device *air_conditioner =
                airConditionerFactory.createDevice(device_param);
            airConditioners.addDevice(
                static_cast<AirConditioner *>(air_conditioner));
            break;
        }
        default: {
            Device *sensor = sensorFactory.createDevice(device_param);
            sensors.addDevice(static_cast<Sensor *>(sensor));
            break;
        }
        }
    }
}

void Room::showDevices() {
    std::cout << "Show devices\n";
    sensors.displayInfo();
    lights.displayInfo();
    airConditioners.displayInfo();
}

void Room::findDevice() { std::cout << "Find device\n"; }

void Room::removeDevice() { std::cout << "Remove device\n"; }

void Room::saveDevices() {
    std::cout << "Save devices\n";
    std::cout << "请输入想要保存的文件名称(无后缀): \n";
    std::string filename;
    std::cin >> filename;
    std::vector<DeviceParam> device_params;
    std::vector<DeviceParam> sensors_params = sensors.getDeviceParams();
    device_params.insert(device_params.end(), sensors_params.begin(),
                         sensors_params.end());

    std::vector<DeviceParam> lights_params = lights.getDeviceParams();
    device_params.insert(device_params.end(), lights_params.begin(),
                         lights_params.end());

    std::vector<DeviceParam> air_conditioners_params =
        airConditioners.getDeviceParams();
    device_params.insert(device_params.end(), air_conditioners_params.begin(),
                         air_conditioners_params.end());

    std::string json_path = "../data/" + filename + ".json";

    json j = device_params;
    std::ofstream ofs(json_path);
    ofs << j.dump(4);
    ofs.close();
}

void Room::sceneSimulation() { std::cout << "Scene simulation\n"; }

void menu() {
    std::cout << "=========主菜单=========" << std::endl;
    std::cout << "1 ---- 初始化并打印当前用户" << std::endl;
    std::cout << "2 ---- 从文件导入设备" << std::endl;
    std::cout << "3 ---- 从键盘添加设备" << std::endl;
    std::cout << "4 ---- 列表显示当前所有设备" << std::endl;
    std::cout << "5 ---- 按指定ID查找设备" << std::endl;
    std::cout << "6 ---- 删除指定ID的设备" << std::endl;
    std::cout << "7 ---- 保存所有设备信息至文件中" << std::endl;
    std::cout << "8 --- 智能场景模拟" << std::endl;
    std::cout << "Q ---- 退出" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "请选择：" << std::endl;
}