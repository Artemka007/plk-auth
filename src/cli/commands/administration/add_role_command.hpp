#pragma once
#include "../base_command.hpp"
#include "src/models/user_role.hpp"

class AddRoleCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    ValidationResult validate_args(const CommandArgs &args) const override;
    bool execute(const CommandArgs &args) override;
    bool is_visible() const override;
};
