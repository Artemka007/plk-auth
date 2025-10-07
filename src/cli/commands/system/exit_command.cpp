#include "exit_command.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"

bool ExitCommand::execute(const std::vector<std::string> &args) {
    if (!args.empty()) {
        io_handler_->error("Usage: exit (no arguments)");
        return false;
    }

    app_state_->set_running(false);
    return true;
}
