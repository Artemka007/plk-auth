#include "logout_command.hpp"
#include "../command_registry.hpp"
#include "src/models/enums.hpp"

ValidationResult LogoutCommand::validate_args(const CommandArgs &args) const {
    if (!args.positional.empty() || !args.flags.empty() ||
        !args.options.empty()) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool LogoutCommand::execute(const CommandArgs &args) {
    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in");
        return false;
    }

    io_handler_->print("Are you sure you want to log out " + user->email() +
                       "? (y/N): ");
    std::string confirmation = io_handler_->read_line();

    if (confirmation == "y" || confirmation == "Y" || confirmation == "yes") {
        log_service_->info(models::ActionType::SYSTEM_LOGOUT, "User logged out",
                           user, user);

        app_state_->set_current_user(nullptr);
        io_handler_->println("Logged out successfully");
        return true;
    }

    io_handler_->println("Logout cancelled");
    return true;
}

bool LogoutCommand::is_visible() const {
    return app_state_->is_authenticated();
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "logout", [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<LogoutCommand>(
                "logout", "Log out of the application", "logout (no arguments)",
                app_state, io, auth, user, log, d);
        });
    return true;
}();
} // namespace