#include "cli/commands/system/ExitCommand.hpp"
#include "cli/AppState.hpp"
#include "cli/IOHandler.hpp"

bool ExitCommand::execute(const std::vector<std::string> &args) {
    if (!args.empty()) {
        io_handler_->error("Usage: exit (no arguments)");
        return false;
    }

    app_state_->set_running(false);
    return true;
}
