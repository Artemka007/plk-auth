#include "cli/commands/logs/view_logs_command.hpp"
#include "cli/app_state.hpp"
#include "cli/io_handler.hpp"
#include "services/log_service.hpp"

bool ViewLogsCommand::execute(const std::vector<std::string> &args) {
    size_t logs_limit = 100;

    if (args.size() != 1) {
        io_handler_->error("Usage: view-logs [limit]");
        return false;
    }

    try {
        int parsed = std::stoi(args[0]);
        if (parsed <= 0) {
            io_handler_->error("Limit must be positive");
            return false;
        }
        limit = static_cast<size_t>(parsed);
    } catch (const std::invalid_argument &) {
        io_handler_->error("Limit must be a number");
        return false;
    } catch (const std::out_of_range &) {
        io_handler_->error("Limit is too large");
        return false;
    }

    auto logs = log_service_->get_recent_logs(limit);

    if (logs.empty()) {
        io_handler_->println("No logs found");
        return true;
    }

    // Print header
    io_handler_->println("Recent logs:");
    io_handler_->println("----------");

    for (const auto &log_entry : logs) {
        io_handler_->println("[" + log_entry.timestamp + "] [" +
                             log_entry.level + "] " + log_entry.message);
    }

    return true;
}

bool ViewLogsCommand::isVisible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && current_user->has_role("admin");
}
