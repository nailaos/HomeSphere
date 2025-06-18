#include "device.h"
#include "common.h"
#include "exception.h"
#include <string>

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

bool DeviceFactory::check(const json &param) const {
    if (!param.contains("name")) {
        throw InvalidParameterException(param, "Missing required field: name");
    }

    if (!param.contains("priorityLevel")) {
        throw InvalidParameterException(
            param, "Missing required field: priorityLevel");
    }

    if (!param.contains("powerConsumption")) {
        throw InvalidParameterException(
            param, "Missing required field: powerConsumption");
    }

    if (!param["name"].is_string()) {
        throw InvalidParameterException(param, "'name' must be a string");
    }
    if (!param["priorityLevel"].is_number()) {
        throw InvalidParameterException(param,
                                        "'priorityLevel' must be a number");
    }
    if (!param["powerConsumption"].is_number()) {
        throw InvalidParameterException(param,
                                        "'powerConsumption' must be a number");
    }

    int priority = param["priorityLevel"].get<int>();
    if (priority < 0 || priority > MAX_PRIORITY_LEVEL) {
        throw InvalidParameterException(
            param, "\'priorityLevel\' must be between 0 and " + std::to_string(MAX_PRIORITY_LEVEL));
    }

    double power = param["powerConsumption"].get<double>();
    if (power < 0 || power > MAX_POWER_CONSUMPTION) {
        throw InvalidParameterException(param,
                                        "\'powerConsumption\' must be between 0 and " + std::to_string(MAX_POWER_CONSUMPTION));
    }

    return true;
}