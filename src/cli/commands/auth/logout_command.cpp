#include "logout_command.hpp"
#include "src/models/enums.hpp"
#include "../command_registry.hpp"

ValidationResult LogoutCommand::validate_args(const CommandArgs &args) const {
    if (!args.positional.empty() || !args.flags.empty() || !args.options.empty()) {
        return {false, "Usage: logout (no arguments)"};
    }
    return {true, ""};
}

bool LogoutCommand::execute(const CommandArgs &args) {
    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in");
        return false;
    }

    io_handler_->print("Are you sure you want to log out " + user->email() + "? (y/N): ");
    std::string confirmation = io_handler_->read_line();

    if (confirmation == "y" || confirmation == "Y" || confirmation == "yes") {
        log_service_->info(
            models::ActionType::SYSTEM_LOGOUT,
            "User logged out",
            user,
            user
        );

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
        "logout", [](auto app_state, auto io, auto auth, auto user, auto log) {
            return std::make_unique<LogoutCommand>(
                "logout", "Log out of the application", app_state, io, auth, user, log);
        });
    return true;
}();
}