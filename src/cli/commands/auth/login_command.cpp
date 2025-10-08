#include "login_command.hpp"
#include "../command_registry.hpp"
#include "src/services/auth_service.hpp"

ValidationResult LoginCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 1) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool LoginCommand::execute(const CommandArgs &args) {
    const std::string &email = args.positional[0];

    // Secure password input
    std::string password =
        io_handler_->read_password("Enter password for " + email + ": ");

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
        bool changed = false;
        while (!changed) {
            std::string new_password =
                io_handler_->read_password("New password: ");
            std::string confirm_password =
                io_handler_->read_password("Confirm password: ");

            if (new_password != confirm_password) {
                io_handler_->error("Passwords do not match. Try again.");
                continue;
            }

            if (!auth_service_->change_password(user->email(), new_password)) {
                io_handler_->error("Failed to change password. Try again.");
                continue;
            }

            io_handler_->println("Password changed successfully.");
            changed = true;
        }
    }

    return true;
}

bool LoginCommand::is_visible() const {
    return !app_state_->is_authenticated();
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "login", [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<LoginCommand>(
                "login", "Login with your email", "login <email>", app_state,
                io, auth, user, log, d);
        });
    return true;
}();
} // namespace