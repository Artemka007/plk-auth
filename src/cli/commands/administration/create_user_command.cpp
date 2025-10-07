#include "cli/commands/administration/create_user_command.hpp"
#include "cli/io_handler.hpp"
#include "services/user_service.hpp"

bool CreateUserCommand::execute(const std::vector<std::string> &args) {
    if (args.size() != 3) {
        io_handler_->error("Usage: create-user <email> <first_name> <last_name>");
        return false;
    }

    const std::string &email = args[0];
    const std::string &first_name = args[1];
    const std::string &last_name = args[2];

    auto user = user_service_->create_user(first_name, last_name, email);
    if (!user) {
        io_handler_->error("Failed to create user");
        return false;
    }

    io_handler_->println("User created: " + user->email());
    return true;
}

bool CreateUserCommand::isVisible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && current_user->can_manage_users();
}
