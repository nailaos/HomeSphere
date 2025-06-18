#include "room.h"
#include <fstream>
#include <vector>

void Room::init() {
    DeviceFactory *light_factory = new LightFactory();
    DeviceFactory *air_conditioner_factory = new AirConditionerFactory();
    DeviceFactory *sensor_factory = new SensorFactory();

    lights = new LightContainer(light_factory);
    airConditioners = new AirConditionerContainer(air_conditioner_factory);
    sensors = new SensorContainer(sensor_factory);
}

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
    if (!ifs.is_open()) {
        std::cerr << "无法打开文件: " << json_path << std::endl;
        return;
    }

    json j;
    ifs >> j;
    ifs.close();

    sensors->addDevice(j["Sensors"]);
    lights->addDevice(j["Lights"]);
    airConditioners->addDevice(j["AirConditioners"]);
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
            lights->addDevice(device_param);
            break;
        }
        case DeviceType::AirConditioner: {
            std::cout << "请输入目标温度: \n";
            std::cin >> device_param.targetTemperature;
            std::cout << "请输入空调速度: \n";
            std::cin >> device_param.speed;
            airConditioners->addDevice(device_param);
            break;
        }
        default: {
            sensors->addDevice(device_param);
            break;
        }
        }
    }
}

void Room::showDevices() {
    std::cout << "Show devices\n";
    json j = {{"Sensors", *sensors},
              {"Lights", *lights},
              {"AirConditioners", *airConditioners}};
    std::cout << j.dump(4) << std::endl;
}

void Room::findDevice() {
    std::cout << "Find device\n";
    std::cout << "请输入设备ID: \n";
    int id;
    std::cin >> id;
    bool found = sensors->findDevice(id) || lights->findDevice(id) || airConditioners->findDevice(id);

    if (!found)
        std::cout << "未找到设备" << std::endl;
}

void Room::removeDevice() { std::cout << "Remove device\n"; 
    std::cout << "请输入设备ID: \n";
    int id;
    std::cin >> id;
    bool found = sensors->removeDevice(id) || lights->removeDevice(id) || airConditioners->removeDevice(id);

    if (!found)
        std::cout << "未找到设备" << std::endl;
}

void Room::saveDevices() {
    std::cout << "Save devices\n";
    std::cout << "请输入想要保存的文件名称(无后缀): \n";
    std::string filename;
    std::cin >> filename;

    std::string json_path = "../data/" + filename + ".json";

    json j = {{"Sensors", *sensors},
              {"Lights", *lights},
              {"AirConditioners", *airConditioners}};

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