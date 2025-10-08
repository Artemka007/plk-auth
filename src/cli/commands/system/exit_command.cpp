#include "exit_command.hpp"

ValidationResult ExitCommand::validate_args(const CommandArgs &args) const {
    if (!args.positional.empty() || !args.flags.empty() || !args.options.empty()) {
        return {false, "Usage: exit (no arguments)"};
    }
    return {true, ""};
}

bool ExitCommand::is_visible() const { return true; }

bool ExitCommand::execute(const CommandArgs &args) {
    app_state_->set_running(false);
    return true;
}
