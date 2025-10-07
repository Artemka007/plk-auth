#include "cli/commands/system/whoami_command.hpp"
#include "cli/app_state.hpp"
#include "cli/io_handler.hpp"
#include "models/user.hpp"

bool WhoAmICommand::execute(const std::vector<std::string> &args) {
    if (!args.empty()) {
        io_handler_->error("Usage: whoami (no arguments)");
        return false;
    }

    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in");
        return false;
    }

    io_handler_->println("User: " + user->full_name());
    io_handler_->println("Roles: ");
    for (const auto &role : user_service_->user_roles(user)) {
        io_handler_->print("  - " + role->name() + "\n");
    }
    return true;
}

bool WhoAmICommand::isVisible() { return true; }
