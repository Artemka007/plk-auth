#include "delete_user_command.hpp"
#include "../command_registry.hpp"
#include "src/services/user_service.hpp"

ValidationResult
DeleteUserCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 1) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool DeleteUserCommand::execute(const CommandArgs &args) {
    const std::string &target_email = args.positional[0];

    auto target_user = user_service_->find_by_email(target_email);
    if (!target_user) {
        io_handler_->error("User not found: " + target_email);
        return false;
    }

    io_handler_->print("Are you sure you want to delete user " + target_email +
                       "? This action cannot be undone. (y/N): ");
    std::string confirmation = io_handler_->read_line();
    io_handler_->println();

    if (confirmation != "y" && confirmation != "Y" && confirmation != "yes") {
        io_handler_->println("Deletion cancelled");
        return true;
    }

    auto current_user = app_state_->get_current_user();
    if (current_user && current_user->email() == target_email) {
        io_handler_->error("You cannot delete yourself");
        return false;
    }

    if (user_service_->delete_user(target_email, current_user)) {
        io_handler_->println("User deleted: " + target_email);
        return true;
    }

    io_handler_->error("Failed to delete user: " + target_email);
    return false;
}

bool DeleteUserCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "delete-user",
        [](auto app_state, auto io, auto auth, auto user_svc, auto log, auto d) {
            return std::make_unique<DeleteUserCommand>(
                "delete-user", "Delete an existing user", "delete-user <email>",
                app_state, io, auth, user_svc, log, d);
        });
    return true;
}();
}