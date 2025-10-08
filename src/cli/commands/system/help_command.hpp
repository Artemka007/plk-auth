#pragma once

#include "../base_command.hpp"
#include <unordered_map>

class HelpCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    ValidationResult validate_args(const CommandArgs &args) const override;
    bool execute(const CommandArgs &args) override;
    bool is_visible() const override;

    void set_available_commands(const std::unordered_map<std::string, BaseCommand *> &map);

private:
    std::unordered_map<std::string, BaseCommand *> available_commands_ = {};
};
