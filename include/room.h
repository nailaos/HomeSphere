#pragma once

#include "airConditioner.h"
#include "light.h"
#include "sensor.h"
#include "user.h"
#include <iostream>

class Room {
  private:
    LightContainer *lights;
    SensorContainer *sensors;
    AirConditionerContainer *airConditioners;

    User *currentUser;

    Admin* admin;
    LightAdmin* lightAdmin;
    SensorAdmin* sensorAdmin;
    AirConditionerAdmin* airConditionerAdmin;
    Visitor* visitor;

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
    void roomSimulation();
    void changeDevice(int id);
    void changeUser();
    
    // 添加getter方法以便SceneSimulation访问
    LightContainer* getLights() const { return lights; }
    SensorContainer* getSensors() const { return sensors; }
    AirConditionerContainer* getAirConditioners() const { return airConditioners; }
};

void menu();