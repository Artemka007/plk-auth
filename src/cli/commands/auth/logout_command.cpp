#include "logout_command.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"
#include "src/services/auth_service.hpp"
#include "src/services/log_service.hpp"
#include "src/models/enums.hpp"

bool LogoutCommand::execute(const std::vector<std::string> &args) {
    if (!args.empty()) {
        io_handler_->error("Usage: logout (no arguments)");
        return false;
    }

    // Check if user is not logged in
    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in");
        return false;
    }

    // Request confirmation
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

bool LogoutCommand::isVisible() const { return app_state_->is_authenticated(); }
