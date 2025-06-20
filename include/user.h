#pragma once

#include "device.h"
#include <string>
#include <vector>

class User {
  protected:
    std::string name;

  public:
    User(const std::string &name) : name(name) {}
    virtual ~User() = default;
    virtual void show() = 0;
    std::string getName() const { return name; }
    virtual bool canChangeDevice(Device *device) = 0;
};

class Admin : public User {
  public:
    void show() override;
    Admin(const std::string &name) : User(name) {}
    bool canChangeDevice(Device *device) override;
};

class LightAdmin : public User {
  public:
    void show() override;
    LightAdmin(const std::string &name) : User(name) {}
    bool canChangeDevice(Device *device) override;
};

class SensorAdmin : public User {
  public:
    void show() override;
    SensorAdmin(const std::string &name) : User(name) {}
    bool canChangeDevice(Device *device) override;
};

class AirConditionerAdmin : public User {
  public:
    void show() override;
    AirConditionerAdmin(const std::string &name) : User(name) {}
    bool canChangeDevice(Device *device) override;
};

class Visitor : public User {
  public:
    void show() override;
    Visitor(const std::string &name) : User(name) {}
    bool canChangeDevice(Device *device) override;
};
