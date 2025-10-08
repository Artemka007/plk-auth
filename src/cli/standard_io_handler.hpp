#pragma once
#include "io_handler.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

class StandardIOHandler : public IOHandler {
public:
    std::string read_line(const std::string &prompt = "") override;
    void print(const std::string &message) override;
    void println(const std::string &message = "") override;
    void error(const std::string &message) override;
    std::string read_password(const std::string &prompt = "Password: ") override;
    CommandArgs parse_command(const std::string &input) const override;
    bool is_eof() const override;
};
