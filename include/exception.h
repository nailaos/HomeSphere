#pragma once
#include "deviceParam.h"
#include <exception>
#include <string>

class FactoryNotFoundException : public std::exception {
  private:
    std::string message;

  public:
    FactoryNotFoundException()
        : message("Factory for type not found.") {}

    FactoryNotFoundException(const std::string &type)
        : message("Factory for type '" + type + "' not found.") {}

    const char *what() const noexcept override { return message.c_str(); }
};

class InvalidParameterException : public std::exception {
  private:
    std::string message;

  public:
    InvalidParameterException(const json &param, const std::string &msg) {
        message = "Invalid parameter [" + msg + "]: \n" + param.dump(4);
    }

    const char *what() const noexcept override { return message.c_str(); }
};
