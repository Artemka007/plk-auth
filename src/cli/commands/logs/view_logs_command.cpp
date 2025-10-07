#include "view_logs_command.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"
#include "src/models/system_log.hpp"
#include "src/services/log_service.hpp"
#include "src/services/user_service.hpp"

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
        logs_limit = static_cast<size_t>(parsed);
    } catch (const std::invalid_argument &) {
        io_handler_->error("Limit must be a number");
        return false;
    } catch (const std::out_of_range &) {
        io_handler_->error("Limit is too large");
        return false;
    }

    auto logs = log_service_->get_recent_logs(logs_limit);

    if (logs.empty()) {
        io_handler_->println("No logs found");
        return true;
    }

    // Print header
    io_handler_->println("Recent logs:");
    io_handler_->println("----------");

    for (const auto &log_entry : logs) {
        io_handler_->println("[" + log_entry->timestamp() + "] [" +
                             models::to_string(log_entry->level()) + "] " +
                             log_entry->message());
    }

    return true;
}

bool ViewLogsCommand::isVisible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->has_role(current_user, "ADMIN");
}
