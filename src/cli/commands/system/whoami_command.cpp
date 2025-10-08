#include "whoami_command.hpp"
#include "../command_registry.hpp"
#include "src/models/user.hpp"

ValidationResult WhoAmICommand::validate_args(const CommandArgs &args) const {
    if (!args.positional.empty() || !args.flags.empty() ||
        !args.options.empty()) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool WhoAmICommand::execute(const CommandArgs &args) {
    auto user = app_state_->get_current_user();
    if (!user) {
        io_handler_->println("Not logged in (guest)");
        return false;
    }

    io_handler_->println("User: " + user->full_name());
    io_handler_->println("Roles:");
    for (const auto &role : user_service_->user_roles(user)) {
        io_handler_->println("  - " + role->name());
    }
    return true;
}

bool WhoAmICommand::is_visible() const { return true; }

// --- Static registration ---
namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "whoami",
        [](auto app_state, auto io, auto auth, auto user_svc, auto log, auto d) {
            return std::make_unique<WhoAmICommand>(
                "whoami", "Print short information about yourself",
                "whoami (no arguments)", app_state, io, auth, user_svc, log, d);
        });
    return true;
}();
} // namespace
