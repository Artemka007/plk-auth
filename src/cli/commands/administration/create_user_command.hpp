#pragma once

#include "cli/commands/base_command.hpp"

class CreateUserCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    bool execute(const std::vector<std::string> &args) override;
    bool isVisible() const override;
};