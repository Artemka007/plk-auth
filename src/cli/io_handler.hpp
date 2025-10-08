#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

struct CommandArgs {
    std::vector<std::string> positional;        // Positional arguments
    std::map<std::string, std::string> options; // Named options (--key=value)
    std::vector<std::string> flags;             // Short flags (-f, --force)
};

class IOHandler {
public:
    virtual ~IOHandler() = default;

    virtual std::string read_line(const std::string &prompt = "") = 0;
    virtual void print(const std::string &message) = 0;
    virtual void println(const std::string &message = "") = 0;
    virtual void error(const std::string &message) = 0;
    virtual std::string read_password(const std::string &prompt = "Password: ") = 0;

    virtual CommandArgs parse_command(const std::string &input) const = 0;

    virtual bool is_eof() const = 0;
};
