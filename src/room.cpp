#include "room.h"
#include "SmartLogger.h"
#include "exception.h"
#include "sceneSimulation.h"
#include <fstream>
#include <vector>

void Room::init() {
    LOG_INFO_SYS("开始初始化房间设备容器");

    DeviceFactory *light_factory = new LightFactory();
    DeviceFactory *air_conditioner_factory = new AirConditionerFactory();
    DeviceFactory *sensor_factory = new SensorFactory();

    lights = new LightContainer(light_factory);
    airConditioners = new AirConditionerContainer(air_conditioner_factory);
    sensors = new SensorContainer(sensor_factory);

    admin = new Admin("Admin");
    lightAdmin = new LightAdmin("LightAdmin");
    sensorAdmin = new SensorAdmin("SensorAdmin");
    airConditionerAdmin = new AirConditionerAdmin("AirConditionerAdmin");
    visitor = new Visitor("Visitor");

    currentUser = admin;

    LOG_INFO_SYS("房间设备容器初始化完成");
}

void Room::printCurrentUser() {
    LOG_INFO_SYS("打印当前用户信息");
    currentUser->show();
}

void Room::addDevicesFromFile() {
    LOG_INFO_SYS("开始从文件导入设备");
    std::cout << "Add devices from file\n";
    std::cout << "请输入文件名称(data文件夹里): \n";
    std::string filename;
    std::cin >> filename;

    std::string json_path = "../data/" + filename + ".json";
    LOG_INFO_SYS("尝试加载设备配置文件: " + json_path);

    try {
        std::ifstream ifs(json_path);
        json j;
        ifs >> j;
        ifs.close();

        int sensorCount = j["Sensors"].size();
        int lightCount = j["Lights"].size();
        int acCount = j["AirConditioners"].size();

        sensors->addDevice(j["Sensors"]);
        lights->addDevice(j["Lights"]);
        airConditioners->addDevice(j["AirConditioners"]);

        LOG_INFO_SYS("设备导入成功 - 传感器: " + std::to_string(sensorCount) +
                     ", 灯光: " + std::to_string(lightCount) +
                     ", 空调: " + std::to_string(acCount));

    } catch (const FactoryNotFoundException &e) {
        LOG_ALERT_SYS("工厂未找到异常: " + std::string(e.what()));
        std::cout << "工厂未找到异常: " << e.what() << std::endl;
    } catch (const InvalidParameterException &e) {
        LOG_ALERT_SYS("无效参数异常: " + std::string(e.what()));
        std::cout << "无效参数异常: " << e.what() << std::endl;
    } catch (const nlohmann::json::parse_error &e) {
        LOG_ALERT_SYS("JSON解析失败: " + std::string(e.what()));
        std::cout << "JSON解析失败: " << e.what() << std::endl;
    } catch (const nlohmann::json::out_of_range &e) {
        LOG_ALERT_SYS("JSON字段缺失: " + std::string(e.what()));
        std::cout << "JSON字段缺失: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        LOG_ALERT_SYS("其他异常: " + std::string(e.what()));
        std::cout << "其他异常: " << e.what() << std::endl;
    }
}

void Room::addDevices() {
    LOG_INFO_SYS("开始从键盘添加设备");
    std::cout << "Add devices\n";
    int n;
    std::cout << "请输入设备数量: \n";
    std::cin >> n;

    LOG_INFO_SYS("准备添加 " + std::to_string(n) + " 个设备");

    try {
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
            case DeviceType::Sensor: {
                sensors->addDevice(device_param);
                LOG_INFO_SYS("添加传感器设备: " + device_param.name);
                break;
            }
            case DeviceType::Light: {
                std::cout << "请输入灯光亮度: \n";
                std::cin >> device_param.lightness;
                lights->addDevice(device_param);
                LOG_INFO_SYS("添加灯光设备: " + device_param.name);
                break;
            }
            case DeviceType::AirConditioner: {
                std::cout << "请输入目标温度: \n";
                std::cin >> device_param.targetTemperature;
                std::cout << "请输入空调速度: \n";
                std::cin >> device_param.speed;
                airConditioners->addDevice(device_param);
                LOG_INFO_SYS("添加空调设备: " + device_param.name);
                break;
            }
            default:
                throw FactoryNotFoundException();
            }
        }
        LOG_INFO_SYS("设备添加完成");
    } catch (const FactoryNotFoundException &e) {
        LOG_ALERT_SYS("Factory error: " + std::string(e.what()));
        std::cout << "Factory error: " << e.what() << std::endl;
    } catch (const InvalidParameterException &e) {
        LOG_ALERT_SYS("Parameter error: " + std::string(e.what()));
        std::cout << "Parameter error: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        LOG_ALERT_SYS("Other error: " + std::string(e.what()));
        std::cout << "Other error: " << e.what() << std::endl;
    }
}

void Room::showDevices() {
    LOG_INFO_SYS("显示所有设备信息");
    std::cout << "Show devices\n";
    std::cout << "是否需要对设备进行排序? (y/n)\n";
    char c;
    std::cin >> c;
    if (c == 'y' || c == 'Y') {
        std::cout << "请输入想要排序的维度(0(默认):设备ID, 1: 设备重要程度, 2: "
                     "设备能耗)\n";
        int dimension;
        std::cin >> dimension;
        sensors->sortDevices(dimension);
        lights->sortDevices(dimension);
        airConditioners->sortDevices(dimension);
    }
    json j = {{"Sensors", *sensors},
              {"Lights", *lights},
              {"AirConditioners", *airConditioners}};
    std::cout << j.dump(4) << std::endl;
}

void Room::findDevice() {
    LOG_INFO_SYS("开始查找设备");
    std::cout << "Find device\n";
    std::cout << "请输入设备ID: \n";
    int id;
    std::cin >> id;

    LOG_INFO_SYS("查找设备ID: " + std::to_string(id));

    bool found = sensors->findDevice(id) || lights->findDevice(id) ||
                 airConditioners->findDevice(id);

    if (!found) {
        LOG_INFO_SYS("未找到设备ID: " + std::to_string(id));
        std::cout << "未找到设备" << std::endl;
    } else {
        LOG_INFO_SYS("找到设备ID: " + std::to_string(id));
        std::cout << "是否想要修改该设备? (y/n)" << std::endl;
        char c;
        std::cin >> c;
        if (c == 'y' || c == 'Y') {
            changeDevice(id);
        }
    }
}

void Room::removeDevice() {
    LOG_INFO_SYS("开始删除设备");
    std::cout << "Remove device\n";
    std::cout << "请输入设备ID: \n";
    int id;
    std::cin >> id;

    LOG_INFO_SYS("删除设备ID: " + std::to_string(id));

    bool found = sensors->removeDevice(id) || lights->removeDevice(id) ||
                 airConditioners->removeDevice(id);

    if (!found) {
        LOG_INFO_SYS("未找到要删除的设备ID: " + std::to_string(id));
        std::cout << "未找到设备" << std::endl;
    } else {
        LOG_INFO_SYS("成功删除设备ID: " + std::to_string(id));
    }
}

void Room::saveDevices() {
    LOG_INFO_SYS("开始保存设备信息");
    std::cout << "Save devices\n";
    std::cout << "请输入想要保存的文件名称(无后缀): \n";
    std::string filename;
    std::cin >> filename;

    std::string json_path = "../data/" + filename + ".json";
    LOG_INFO_SYS("保存设备信息到文件: " + json_path);

    json j = {{"Sensors", *sensors},
              {"Lights", *lights},
              {"AirConditioners", *airConditioners}};

    std::ofstream ofs(json_path);
    ofs << j.dump(4);
    ofs.close();

    LOG_INFO_SYS("设备信息保存成功");
}

void Room::roomSimulation() {
    LOG_INFO_SYS("开始智能场景模拟");
    std::cout << "Scene simulation\n";

    // 检查是否有设备
    if (sensors->getSize() == 0 && lights->getSize() == 0 &&
        airConditioners->getSize() == 0) {
        LOG_ALERT_SYS("警告: 没有设备可以模拟");
        std::cout << "警告: 没有设备可以模拟。请先添加一些设备。" << std::endl;
        return;
    }

    // 创建场景模拟对象
    std::cout << "正在创建智能场景模拟对象..." << std::endl;
    SceneSimulation *sceneSimulation = new SceneSimulation(this);
    std::cout << "智能场景模拟对象创建成功" << std::endl;

    // 加载环境配置文件
    sceneSimulation->loadEnvironmentConfig("../data/environment.json");

    std::cout << "开始智能场景模拟..." << std::endl;
    std::cout << "模拟将使用多线程控制不同类型的设备" << std::endl;
    std::cout << "环境参数将根据时间自动变化" << std::endl;
    std::cout << "自动化规则和紧急事件处理已启用" << std::endl;
    std::cout << "将自动触发预设的突发事件" << std::endl;

    // 启动场景模拟
    sceneSimulation->start();

    // 等待场景模拟结束
    LOG_INFO_SYS("场景模拟结束");
    std::cout << "场景模拟已结束，返回主菜单..." << std::endl;
}

void Room::changeDevice(int id) {
    LOG_INFO_SYS("开始修改设备信息");
    std::cout << "Change device\n";

    LOG_INFO_SYS("修改设备ID: " + std::to_string(id));

    bool found = sensors->findDevice(id) || lights->findDevice(id) ||
                 airConditioners->findDevice(id);

    if (!found) {
        LOG_INFO_SYS("未找到要修改的设备ID: " + std::to_string(id));
        std::cout << "未找到设备" << std::endl;
    } else {
        Device *device = sensors->getDevice(id);
        if (!device) {
            device = lights->getDevice(id);
        }
        if (!device) {
            device = airConditioners->getDevice(id);
        }
        if (currentUser->canChangeDevice(device)) {
            LOG_INFO_SYS(
                "当前用户可以修改设备ID: " + std::to_string(id) +
                " 设备类型: " + DeviceTypeToStr(device->getDeviceType()));
            switch (device->getDeviceType()) {
            case DeviceType::Sensor: {
                sensors->changeDevice(id);
                break;
            }
            case DeviceType::Light: {
                lights->changeDevice(id);
                break;
            }
            case DeviceType::AirConditioner: {
                airConditioners->changeDevice(id);
                break;
            }
            default:
                break;
            }
        } else {
            LOG_INFO_SYS(
                "当前用户无权修改设备ID: " + std::to_string(id) +
                " 设备类型: " + DeviceTypeToStr(device->getDeviceType()));
            std::cout << "当前用户无权修改"
                      << DeviceTypeToStr(device->getDeviceType()) << "设备"
                      << std::endl;
        }
    }
}

void Room::changeUser() {
    LOG_INFO_SYS("开始切换用户");
    std::cout << "Change user\n";
    std::cout << "请输入想切换的用户: (0: Admin, 1: LightAdmin, 2: SensorAdmin, 3: AirConditionerAdmin, 4: Visitor\n";
    int user_id;
    std::cin >> user_id;
    switch (user_id) {
    case 0:
        currentUser = admin;
        break;
    case 1:
        currentUser = lightAdmin;
        break;
    case 2:
        currentUser = sensorAdmin;
        break;
    case 3:
        currentUser = airConditionerAdmin;
        break;
    case 4:
        currentUser = visitor;
        break;
    default:
        break;
    }
    LOG_INFO_SYS("切换用户成功");
    std::cout << "切换用户成功" << std::endl;
    printCurrentUser();
}

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