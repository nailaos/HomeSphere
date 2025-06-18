#pragma once

#include "airConditioner.h"
#include "light.h"
#include "sensor.h"
#include <iostream>

class Room {
  private:
    LightContainer *lights;
    SensorContainer *sensors;
    AirConditionerContainer *airConditioners;

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