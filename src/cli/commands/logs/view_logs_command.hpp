#pragma once
#include "../base_command.hpp"

class ViewLogsCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    ValidationResult validate_args(const CommandArgs &args) const override;
    bool execute(const CommandArgs &args) override;
    bool is_visible() const override;
};
