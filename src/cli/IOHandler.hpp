#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class IOHandler {
    virtual ~IOHandler() = default;

    virtual std::string readLine(const std::string& prompt = "") = 0;

    virtual void print(const std::string& message) = 0;

    virtual void println(const std::string& message = "") = 0;

    virtual void error(const std::string& message) = 0;

    virtual std::vector<std::string> splitCommand(const std::string& input) const = 0;
};
