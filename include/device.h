#pragma once

#include "deviceParam.h"
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
    virtual DeviceParam getDeviceParam() const = 0;
};

class DeviceFactory {
  public:
    DeviceFactory() = default;
    ~DeviceFactory() = default;

    virtual Device *createDevice() = 0;
    virtual Device *createDevice(DeviceParam &params) = 0;
};

template <typename T> class DeviceContainer {
  protected:
    T **devices;
    int size;
    int capacity;

    void expand();

  public:
    DeviceContainer();  // Constructor
    ~DeviceContainer(); // Destructor

    void addDevice(T *Device);
    T *getDevice(int id);
    int getSize() const;
    virtual void displayInfo() = 0;

    std::vector<DeviceParam> getDeviceParams() const;
};

// Constructor initializes the devices array and sets the size and capacity
template <typename T>
DeviceContainer<T>::DeviceContainer() : size(0), capacity(2) {
    devices = new T *[capacity];
}

// Destructor to delete all device objects and the array
template <typename T> DeviceContainer<T>::~DeviceContainer() {
    for (int i = 0; i < size; ++i) {
        delete devices[i];
    }
    delete[] devices;
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

// Adds a new device to the container
template <typename T> void DeviceContainer<T>::addDevice(T *Device) {
    if (size == capacity) {
        expand(); // If the array is full, expand its size
    }
    devices[size++] = Device;
}

// Gets a device by index
template <typename T> T *DeviceContainer<T>::getDevice(int index) {
    if (index < 0 || index >= size) {
        return nullptr;
    }
    return devices[index];
}

// Returns the current number of devices in the container
template <typename T> int DeviceContainer<T>::getSize() const { return size; }

// Displays the information of all devices in the container
template <typename T> void DeviceContainer<T>::displayInfo() {
    for (int i = 0; i < size; ++i) {
        devices[i]->displayInfo();
    }
}

template <typename T>
std::vector<DeviceParam> DeviceContainer<T>::getDeviceParams() const {
    std::vector<DeviceParam> params;
    for (int i = 0; i < size; ++i) {
        params.push_back(devices[i]->getDeviceParam());
    }
    return params;
}