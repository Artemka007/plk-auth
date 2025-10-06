#include "ExitCommand.hpp"
#include "../AppState.hpp"

bool ExitCommand::execute(const std::vector<std::string> &args) {
    app_state_->set_running(false);
    return true;
}
