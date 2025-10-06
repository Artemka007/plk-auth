#include "cli/commands/auth/LoginCommand.hpp"
#include "cli/AppState.hpp"
#include "cli/IOHandler.hpp"
#include "services/AuthService.hpp"

bool LoginCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        io_handler_->error("Usage: login <email>");
        return false;
    }

    const std::string &email = args[0];

    // Request password through secure input
    std::string password = io_handler_->read_password("Enter password for " + email + ": ");

    // Request to authentication service
    if (auth_service_->login(email, password)) {
        auto user = auth_service_->get_current_user();
        io_handler_->println("Login successful. Welcome, " + user->email());
        return true;
    }

    io_handler_->error("Login failed: invalid credentials");
    return false;
}

bool LoginCommand::isVisible() { return !app_state_->is_authenticated() }
