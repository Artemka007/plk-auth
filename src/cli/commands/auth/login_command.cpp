#include "login_command.hpp"
#include "src/services/auth_service.hpp"

ValidationResult LoginCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 1) {
        return {false, "Usage: login [email]"};
    }
    return {true, ""};
}

bool LoginCommand::execute(const CommandArgs &args) {
    const std::string &email = args.positional[0];

    // Secure password input
    std::string password = io_handler_->read_password("Enter password for " + email + ": ");

    services::LoginResult result = auth_service_->login(email, password);
    if (!result.success) {
        io_handler_->error("Login failed: invalid credentials");
        return false;
    }

    auto user = result.user;
    app_state_->set_current_user(user);

    // TODO: create login log
    io_handler_->println("Login successful. Welcome, " + user->email());

    // Force password change if required
    if (result.password_change_required) {
        io_handler_->println("You must change your password now.");
        // TODO: Trigger password change flow
    }

    return true;
}

bool LoginCommand::is_visible() const {
    return !app_state_->is_authenticated();
}
