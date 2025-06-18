#pragma once

#include "deviceParam.h"
#include "exception.h"
#include <iostream>
#include <vector>

class Device {
  protected:
    static int nextId;
    int id;
    std::string name;
    int priorityLevel;
    double powerConsumption;
    bool state;

  public:
    Device(std::string name, int priorityLevel, double powerConsumption)
        : id(nextId++), name(name), priorityLevel(priorityLevel),
          powerConsumption(powerConsumption), state(false) {};

    virtual ~Device() = default;

    int getId() const;
    std::string getName() const;
    int getPriorityLevel() const;
    double getPowerConsumption() const;
    bool getState() const;

    void setName(const std::string &name);
    void setPriorityLevel(int priorityLevel);
    void setPowerConsumption(double powerConsumption);
    void setState(bool state);

    virtual DeviceType getDeviceType() const = 0;
    virtual void update() = 0;

    virtual json toJson() const = 0;
};

class DeviceFactory {
  public:
    DeviceFactory() = default;
    ~DeviceFactory() = default;

    bool check(const json &param) const;

    virtual Device *createDevice() = 0;
    virtual Device *createDevice(const json &param) = 0;
    virtual Device *createDevice(DeviceParam &params) = 0;
};

template <typename T> class DeviceContainer {
  protected:
    T **devices;
    int size;
    int capacity;
    DeviceFactory *factory;

    void expand();

  public:
    DeviceContainer(DeviceFactory *factory); // Constructor
    ~DeviceContainer();                      // Destructor

    void addDevice();
    void addDevice(T *Device);
    void addDevice(json &params);
    void addDevice(DeviceParam &params);
    bool findDevice(int id);
    bool removeDevice(int id);
    Device* getDevice(int id);

    int getSize() const;

    std::vector<DeviceParam> getDeviceParams() const;

    json toJson() const;
};

// Constructor initializes the devices array and sets the size and capacity
template <typename T>
DeviceContainer<T>::DeviceContainer(DeviceFactory *factory)
    : size(0), capacity(2), factory(factory) {
    devices = new T *[capacity];
}

// Destructor to delete all device objects and the array
template <typename T> DeviceContainer<T>::~DeviceContainer() {
    for (int i = 0; i < size; ++i) {
        delete devices[i];
    }
    delete[] devices;
    devices = nullptr;
    delete factory;
    factory = nullptr;
}

// Expands the device array by doubling the capacity
template <typename T> void DeviceContainer<T>::expand() {
    capacity *= 2;
    T **newDevices = new T *[capacity];

    // Copy the old devices into the new array
    for (int i = 0; i < size; ++i) {
        newDevices[i] = devices[i];
    }

    delete[] devices;
    devices = newDevices;
}

// Adds a default device
template <typename T> void DeviceContainer<T>::addDevice() {
    if (size == capacity) {
        expand();
    }
    devices[size++] = factory->createDevice();
}

// Adds a new device to the container
template <typename T> void DeviceContainer<T>::addDevice(T *Device) {
    if (size == capacity) {
        expand(); // If the array is full, expand its size
    }
    devices[size++] = Device;
}

template <typename T> void DeviceContainer<T>::addDevice(json &params) {
    if (!params.is_array()) {
        throw InvalidParameterException(params, "params must be an array in addDevice(json &params)");
    }

    for (const auto &item : params) {
        T *device = static_cast<T *>(factory->createDevice(item));
        addDevice(device);
    }
}

template <typename T> void DeviceContainer<T>::addDevice(DeviceParam &params) {
    T *device = static_cast<T *>(factory->createDevice(params));
    addDevice(device);
}

// Gets a device by id
template <typename T> bool DeviceContainer<T>::findDevice(int id) {
    for (int i = 0; i < size; ++i) {
        if (devices[i]->getId() == id) {
            std::cout << "Found device with id " << id << "\n";
            json j = *devices[i];
            std::cout << j.dump(4) << "\n";
            return true;
        }
    }

    return false;
}

template <typename T> bool DeviceContainer<T>::removeDevice(int id) {
    for (int i = 0; i < size; ++i) {
        if (devices[i]->getId() == id) {
            std::cout << "Removed device with id " << id << "\n";
            json j = *devices[i];
            std::cout << j.dump(4) << "\n";
            delete devices[i];
            for (int j = i; j < size - 1; ++j) {
                devices[j] = devices[j + 1];
            }
            --size;
            return true;
        }
    }

    return false;
}

// Gets a device by id
template <typename T> Device *DeviceContainer<T>::getDevice(int id) {
    for (int i = 0; i < size; ++i) {
        if (devices[i]->getId() == id) {
            return devices[i];
        }
    }
    return nullptr;
}

// Returns the current number of devices in the container
template <typename T> int DeviceContainer<T>::getSize() const { return size; }

template <typename T> json DeviceContainer<T>::toJson() const {
    json j = json::array();
    for (int i = 0; i < size; ++i) {
        j.push_back(*(devices[i])); // 自动调用 T 的 to_json
    }
    return j;
}

template <typename T>
void to_json(nlohmann::ordered_json &j, const DeviceContainer<T> &container) {
    j = container.toJson();
}