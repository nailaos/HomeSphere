#include "SmartLogger.h"
#include "room.h"
#include <chrono>
#include <iostream>
#include <limits>

std::string getTimestampForFilename() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%Y-%m-%d_%H-%M-%S");
    return ss.str();
}

int main() {
    // 初始化日志系统
    SmartLogger::getInstance()->setMinLevel(LogLevel::INFO);
    std::string filename =
        "../logs/simulation_" + getTimestampForFilename() + ".log";
    SmartLogger::getInstance()->addOutputter(
        std::make_unique<FileOutputter>(filename));
    LOG_INFO_SYS("智能家居系统启动");

    Room room;
    bool init = false;

    char choice;
    do {
        menu();
        std::cin >> choice;

        switch (choice) {
        case '1':
            if (init) {
                room.changeUser();
            } else {
                room.init();
                init = true;
            }
            room.printCurrentUser();
            break;
        case '2':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.addDevicesFromFile();
            break;
        case '3':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.addDevices();
            break;
        case '4':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.showDevices();
            break;
        case '5':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.findDevice();
            break;
        case '6':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.removeDevice();
            break;
        case '7':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.saveDevices();
            break;
        case '8':
            if (!init) {
                std::cout << "请先初始化房间设备容器" << std::endl;
                break;
            }
            room.roomSimulation();
            break;
        case 'Q':
        case 'q':
            LOG_INFO_SYS("用户选择退出系统");
            std::cout << "退出系统" << std::endl;
            break;
        default:
            LOG_INFO_SYS("用户输入无效选择: " + std::string(1, choice));
            std::cout << "无效选择，请重新输入" << std::endl;
            break;
        }
    } while (choice != 'Q' && choice != 'q');

    LOG_INFO_SYS("智能家居系统关闭");
    return 0;
}