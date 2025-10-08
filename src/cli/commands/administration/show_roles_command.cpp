#include "show_roles_command.hpp"
#include "../command_registry.hpp"
#include "src/services/user_service.hpp"

ValidationResult ShowRolesCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 1) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool ShowRolesCommand::execute(const CommandArgs &args) {
    const std::string &email = args.positional[0];

    auto user = user_service_->find_by_email(email);
    if (!user) {
        io_handler_->error("User not found: " + email);
        return false;
    }

    auto roles = user_service_->user_roles(user);
    if (roles.empty()) {
        io_handler_->println("User " + email + " has no roles assigned.");
        return true;
    }

    io_handler_->println("Roles for user " + email + ":");
    io_handler_->println("----------------------------");
    for (const auto &role : roles) {
        io_handler_->println("- " + role->name());
    }
    io_handler_->println();

    return true;
}

bool ShowRolesCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "show-roles",
        [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<ShowRolesCommand>(
                "show-roles",
                "Show all roles assigned to a user",
                "show-roles <email>",
                app_state, io, auth, user, log, d);
        });
    return true;
}();
} // namespace
