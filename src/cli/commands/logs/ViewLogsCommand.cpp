#include "cli/commands/logs/ViewLogsCommand.hpp"
#include "cli/AppState.hpp"
#include "cli/IOHandler.hpp"
#include "services/LogService.hpp"

bool ViewLogsCommand::execute(const std::vector<std::string> &args) {
    int limit = 10;

    if (args.size() != 1) {
        io_handler_->error("Usage: view-logs [limit]");
        return false;
    }

    // TODO: Make better logs receive
    auto logs = log_service_->get_recent_logs(limit);

    if (logs.empty()) {
        io_handler_->println("No logs found");
        return true;
    }

    // Print header
    io_handler_->println("Recent logs:");
    io_handler_->println("----------");

    for (const auto &log_entry : logs) {
        io_handler_->println("[" + log_entry.timestamp + "] [" + log_entry.level + "] " +
                             log_entry.message);
    }

    return true;
}

bool ViewLogsCommand::isVisible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && current_user->has_role("admin");
}
