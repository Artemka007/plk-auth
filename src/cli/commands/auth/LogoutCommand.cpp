#include "cli/commands/auth/LogoutCommand.hpp"
#include "cli/AppState.hpp"
#include "cli/IOHandler.hpp"
#include "services/AuthService.hpp"

bool LogoutCommand::execute(const std::vector<std::string> &args) {
    if (!args.empty()) {
        io_handler_->error("Usage: logout (no arguments)");
        return false;
    }

    // Check if user is not logged in
    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in");
        return true;
    }

    // Request confirmation
    io_handler_->print("Are you sure you want to log out " + user->email() + "? (y/N): ");
    std::string confirmation = io_handler_->read_line();

    io_handler_->println();

    if (confirmation == "y" || confirmation == "Y" || confirmation == "yes") {
        auth_service_->logout();
        io_handler_->println("Logged out successfully");
        return true;
    }

    io_handler_->println("Logout cancelled");
    return true;
}

bool LogoutCommand::isVisible() const { return app_state_->is_authenticated(); }
