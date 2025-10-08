#include "exit_command.hpp"
#include "../command_registry.hpp"

ValidationResult ExitCommand::validate_args(const CommandArgs &args) const {
    if (!args.positional.empty() || !args.flags.empty() ||
        !args.options.empty()) {
        return {false, "Usage: exit (no arguments)"};
    }
    return {true, ""};
}

bool ExitCommand::is_visible() const { return true; }

bool ExitCommand::execute(const CommandArgs &args) {
    app_state_->set_running(false);
    return true;
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "exit", [](auto app_state, auto io, auto auth, auto user, auto log) {
            return std::make_unique<ExitCommand>(
                "exit", "Exit the application", "exit (no arguments)",
                app_state, io, auth, user, log);
        });
    return true;
}();
} // namespace