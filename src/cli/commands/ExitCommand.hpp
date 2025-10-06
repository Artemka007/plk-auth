#pragma once

#include "BaseCommand.hpp"

class ExitCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    bool execute(const std::vector<std::string> &args) override;
    bool isVisible() const override { return true; }
};