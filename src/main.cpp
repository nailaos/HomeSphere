#include "room.h"

int main() {
    Room room;
    menu();

    while (true) {
        int mode = 0;
        std::cout << "请选择要执行的选项：" << std::endl;
        std::cin >> mode;
        switch (mode) {
        case 1:
            room.init();
            room.printCurrentUser();
            break;
        case 2:
            room.addDevicesFromFile();
            break;
        case 3:
            room.addDevices();
            break;
        case 4:
            room.showDevices();
            break;
        case 5:
            room.findDevice();
            break;
        case 6:
            room.removeDevice();
            break;
        case 7:
            room.saveDevices();
            break;
        case 8:
            room.sceneSimulation();
            break;
        default:
            return 0;
        }
    }
}