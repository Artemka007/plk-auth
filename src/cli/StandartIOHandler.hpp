#pragma once

#include "IOHandler.hpp"

class StandartIOHandler : public IOHandler {
public:
    std::string read_line(const std::string &prompt) override {
        if (!prompt.empty()) {
            std::cout << prompt << std::flush;
        }
        std::string line;
        if (!std::getline(std::cin, line)) {
            return "";
        }
        return line;
    }

    void print(const std::string &message) override { std::cout << message << std::flush; }

    void println(const std::string &message) override { std::cout << message << '\n'; }

    void error(const std::string &message) override { std::cerr << "Error: " << message << '\n'; }

    std::vector<std::string> split_command(const std::string &input) const override {
        if (input.empty()) {
            return {};
        }
        std::istringstream iss(input);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool is_eof() const override { return std::cin.eof(); }
};
