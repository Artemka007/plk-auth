#include "delete_user_command.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"
#include "src/services/user_service.hpp"

bool DeleteUserCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 1) {
        io_handler_->error("Usage: delete-user <email>");
        return false;
    }

    const std::string &target_email = args[0];

    // Check if the user exists
    auto target_user = user_service_->find_by_email(target_email);
    if (!target_user) {
        io_handler_->error("User not found: " + target_email);
        return false;
    }

    // Request confirmation
    io_handler_->print("Are you sure you want to delete user " + target_email +
                       "? This action cannot be undone. (y/N): ");
    std::string confirmation = io_handler_->read_line();
    io_handler_->println();

    if (confirmation != "y" && confirmation != "Y" && confirmation != "yes") {
        io_handler_->println("Deletion cancelled");
        return true;
    }

    // Check if the user is trying to delete himself
    auto current_user = app_state_->get_current_user();
    if (current_user && current_user->email() == target_email) {
        io_handler_->error("You cannot delete yourself");
        return false;
    }

    // Delete user
    if (user_service_->delete_user(target_email)) {
        io_handler_->println("User deleted: " + target_email);
        return true;
    }

    io_handler_->error("Failed to delete user: " + target_email);
    return false;
}

bool DeleteUserCommand::isVisible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}
