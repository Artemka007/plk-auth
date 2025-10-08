#include "remove_role_command.hpp"
#include "../command_registry.hpp"
#include "src/services/user_service.hpp"

ValidationResult RemoveRoleCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 2) {
        return {false, "Usage: " + get_usage()};
    }

    const std::string &role = args.positional[1];
    if (role != "ADMIN" && role != "USER_MANAGER") {
        return {false, "Invalid role. Allowed: ADMIN, USER_MANAGER"};
    }

    return {true, ""};
}

bool RemoveRoleCommand::execute(const CommandArgs &args) {
    const std::string &email = args.positional[0];
    const std::string &role_name = args.positional[1];

    auto user = user_service_->find_by_email(email);
    if (!user) {
        io_handler_->error("User not found: " + email);
        return false;
    }

    auto role = user_service_->get_role_by_name(role_name);
    if (!role) {
        io_handler_->error("Role not found: " + role_name);
        return false;
    }

    if (!user_service_->remove_role_from_user(email, *role, app_state_->get_current_user())) {
        io_handler_->error("Failed to remove role " + role_name + " from " + email);
        log_service_->error(models::ActionType::USER_ROLE_CHANGED,
                            "Failed to remove role " + role_name + " from " + email,
                            user);
        return false;
    }

    io_handler_->println("Role " + role_name + " removed from " + email);
    log_service_->info(models::ActionType::USER_ROLE_CHANGED,
                       "Role " + role_name + " removed from " + email,
                       user);

    return true;
}

bool RemoveRoleCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "remove-role",
        [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<RemoveRoleCommand>(
                "remove-role",
                "Remove a role from a user",
                "remove-role <email> <role_name>",
                app_state, io, auth, user, log, d);
        });
    return true;
}();
}
