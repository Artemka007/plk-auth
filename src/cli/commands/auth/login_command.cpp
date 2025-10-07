#include "login_command.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"
#include "src/services/auth_service.hpp"

bool LoginCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        io_handler_->error("Usage: login [email]");
        return false;
    }

    const std::string &email = args[0];

    // Request password through secure input
    std::string password = io_handler_->read_password("Enter password for " + email + ": ");

    LoginResult result = auth_service_->login(email, password);
    // Request to authentication service
    if (!result.success) {
        // TODO: Create Log
        io_handler_->error("Login failed: invalid credentials");
        return false;
    }

    auto user = result.user;
    app_state_->set_current_user(user);

    // TODO: Create Log
    io_handler_->println("Login successful. Welcome, " + user->email());
    return true;
}

bool LoginCommand::isVisible() const { return !app_state_->is_authenticated(); }
