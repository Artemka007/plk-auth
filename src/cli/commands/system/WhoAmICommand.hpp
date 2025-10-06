#pragma once

#include "cli/commands/BaseCommand.hpp"

class WhoAmICommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    bool execute(const std::vector<std::string> &args);
    bool isVisible() const override;
};
