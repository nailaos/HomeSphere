#pragma once

#include "light.h"
#include "sensor.h"
#include "airConditioner.h"
#include <iostream>

class Room {
private:
    LightContainer lights;
    SensorContainer sensors;
    AirConditionerContainer airConditioners;

    LightFactory lightFactory;
    SensorFactory sensorFactory;
    AirConditionerFactory airConditionerFactory;

public:
    Room() {};
    ~Room() {};

    void init();
    void printCurrentUser();
    void addDevicesFromFile();
    void addDevices();
    void showDevices();
    void findDevice();
    void removeDevice();
    void saveDevices();
    void sceneSimulation();
};

void menu();