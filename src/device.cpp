#include "device.h"

int Device::nextId = 0;

int Device::getId() const { return id; }

std::string Device::getName() const { return name; }

int Device::getPriorityLevel() const { return priorityLevel; }

double Device::getPowerConsumption() const { return powerConsumption; }

bool Device::getState() const { return state; }

void Device::setName(const std::string &name) { this->name = name; }

void Device::setPriorityLevel(int priorityLevel) {
    this->priorityLevel = priorityLevel;
}

void Device::setPowerConsumption(double powerConsumption) {
    this->powerConsumption = powerConsumption;
}

void Device::setState(bool state) { this->state = state; }