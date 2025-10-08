#include "view_logs_command.hpp"
#include "../command_registry.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"
#include "src/models/system_log.hpp"
#include "src/services/log_service.hpp"
#include "src/services/user_service.hpp"

ValidationResult ViewLogsCommand::validate_args(const CommandArgs &args) const {
    ValidationResult result;
    result.valid = true;

    if (args.positional.size() > 1) {
        result.valid = false;
        result.error_message = "Usage: " + get_usage();
    } else if (!args.positional.empty()) {
        try {
            int parsed = std::stoi(args.positional[0]);
            if (parsed <= 0) {
                result.valid = false;
                result.error_message = "Limit must be positive";
            }
        } catch (...) {
            result.valid = false;
            result.error_message = "Limit must be a number";
        }
    }

    return result;
}

bool ViewLogsCommand::execute(const CommandArgs &args) {
    size_t logs_limit = 100;

    if (!args.positional.empty()) {
        logs_limit = static_cast<size_t>(std::stoi(args.positional[0]));
    }

    auto logs = log_service_->get_recent_logs(logs_limit);
    if (logs.empty()) {
        io_handler_->println("No logs found");
        return true;
    }

    io_handler_->println("Recent logs:");
    io_handler_->println("----------");

    for (const auto &log_entry : logs) {
        io_handler_->println("[" + log_entry->timestamp() + "] [" +
                             models::to_string(log_entry->level()) + "] " +
                             log_entry->message());
    }

    return true;
}

bool ViewLogsCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->has_role(current_user, "ADMIN");
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "view-logs",
        [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<ViewLogsCommand>(
                "view-logs", "Show recent system logs", "view-logs [limit], auto d",
                app_state, io, auth, user, log, d);
        });
    return true;
}();
} // namespace
