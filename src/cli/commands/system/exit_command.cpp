#include "cli/commands/system/exit_command.hpp"
#include "cli/app_state.hpp"
#include "cli/io_handler.hpp"

bool ExitCommand::execute(const std::vector<std::string> &args) {
    if (!args.empty()) {
        io_handler_->error("Usage: exit (no arguments)");
        return false;
    }

    app_state_->set_running(false);
    return true;
}
