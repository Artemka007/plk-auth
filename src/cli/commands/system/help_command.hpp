#pragma once

#include "cli/commands/base_command.hpp"
#include <unordered_map>

class HelpCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    bool execute(const std::vector<std::string> &args) override;
    bool isVisible() const override;
    void set_available_commands(std::unordered_map<std::string, BaseCommand *> &map);

private:
    std::unordered_map<std::string, BaseCommand *> available_commands_ = {};
};
