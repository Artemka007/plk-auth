#pragma once

#include "../base_command.hpp"

class ImportDataCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    bool execute(const CommandArgs &args) override;
    bool is_visible() const override;
    ValidationResult validate_args(const CommandArgs &args) const override;
};