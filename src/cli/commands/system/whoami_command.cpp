#include "whoami_command.hpp"
#include "src/models/user.hpp"

ValidationResult WhoAmICommand::validate_args(const CommandArgs &args) const {
    if (!args.positional.empty() || !args.flags.empty() || !args.options.empty()) {
        return {false, "Usage: whoami (no arguments)"};
    }
    return {true, ""};
}

bool WhoAmICommand::execute(const CommandArgs &args) {
    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in");
        return false;
    }

    io_handler_->println("User: " + user->full_name());
    io_handler_->println("Roles:");
    for (const auto &role : user_service_->user_roles(user)) {
        io_handler_->println("  - " + role->name());
    }
    return true;
}

bool WhoAmICommand::is_visible() const { return true; }
