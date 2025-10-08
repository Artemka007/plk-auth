#include "add_role_command.hpp"
#include "../command_registry.hpp"
#include "src/services/user_service.hpp"

ValidationResult AddRoleCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 2) {
        return {false, "Usage: " + get_usage()};
    }
    const std::string &role = args.positional[1];
    if (role != "ADMIN" && role != "USER_MANAGER") {
        return {false, "Invalid role. Allowed: ADMIN, USER_MANAGER"};
    }
    return {true, ""};
}

bool AddRoleCommand::execute(const CommandArgs &args) {
    const std::string &email = args.positional[0];
    const std::string &role_name = args.positional[1];

    auto current_user = app_state_->get_current_user();
    auto subject = user_service_->find_by_email(email);
    auto role = user_service_->get_role_by_name(role_name);

    if (!user_service_->add_role_to_user(email, role)) {
        io_handler_->error("Failed to add role " + role_name + " to " + email);
        log_service_->error(models::ActionType::USER_ROLE_CHANGED,
                            "Failed to add role " + role_name + " to " + email,
                            current_user, subject);
        return false;
    }

    io_handler_->println("Role " + role_name + " added to " + email);
    log_service_->info(models::ActionType::USER_ROLE_CHANGED,
                       "Role " + role_name + " added to " + email, current_user, subject);
    return true;
}

bool AddRoleCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "add-role",
        [](auto app_state, auto io, auto auth, auto user, auto log) {
            return std::make_unique<AddRoleCommand>(
                "add-role", "Assign a role to a user",
                "add-role <email> <role_name>", app_state, io, auth, user, log);
        });
    return true;
}();
} // namespace
