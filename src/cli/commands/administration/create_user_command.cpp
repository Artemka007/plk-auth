#include "create_user_command.hpp"
#include "../command_registry.hpp"
#include "src/models/enums.hpp"
#include "src/services/user_service.hpp"

ValidationResult
CreateUserCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() != 3) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool CreateUserCommand::execute(const CommandArgs &args) {
    const std::string &email = args.positional[0];
    const std::string &first_name = args.positional[1];
    const std::string &last_name = args.positional[2];

    services::CreateUserResult result =
        user_service_->create_user(first_name, last_name, email, "USER", app_state_->get_current_user());

    if (!result.success) {
        io_handler_->error("Failed to create user: " + result.message);
        return false;
    }

    auto current_user = app_state_->get_current_user();
    log_service_->info(models::ActionType::USER_CREATED,
                       "User created successfully: " + email, current_user,
                       result.user);

    io_handler_->println("User created: " + result.user->email());
    io_handler_->println("Temporary password: " + result.generated_password);

    return true;
}

bool CreateUserCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "create-user",
        [](auto app_state, auto io, auto auth, auto user_svc, auto log) {
            return std::make_unique<CreateUserCommand>(
                "create-user", "Create a new user",
                "create-user <email> <first_name> <last_name>",
                app_state, io, auth, user_svc, log);
        });
    return true;
}();
} // namespace