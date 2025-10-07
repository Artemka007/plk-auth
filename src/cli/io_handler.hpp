#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class IOHandler {
public:
    virtual ~IOHandler() = default;

    virtual std::string read_line(const std::string &prompt = "") = 0;

    virtual void print(const std::string &message) = 0;

    virtual void println(const std::string &message = "") = 0;

    virtual void error(const std::string &message) = 0;

    virtual std::string read_password(const std::string &prompt = "Password: ") = 0;

    virtual std::vector<std::string> split_command(const std::string &input) const = 0;

    virtual bool is_eof() const = 0;
};
