#include "view_logs_command.hpp"
#include "../command_registry.hpp"
#include "src/cli/app_state.hpp"
#include "src/cli/io_handler.hpp"
#include "src/models/system_log.hpp"
#include "src/models/enums.hpp"
#include "src/services/log_service.hpp"
#include "src/services/user_service.hpp"
#include <optional>
#include <sstream>

ValidationResult ViewLogsCommand::validate_args(const CommandArgs &args) const {
    ValidationResult result{true, ""};

    for (const auto &arg : args.options) {
        const auto &key = arg.first;
        const auto &val = arg.second;

        if (key == "limit") {
            try {
                int parsed = std::stoi(val);
                if (parsed <= 0) {
                    return {false, "limit must be positive"};
                }
            } catch (...) {
                return {false, "limit must be numeric"};
            }
        } else if (key == "level" || key == "action" ||
                   key == "actor" || key == "subject" ||
                   key == "start" || key == "end") {
            continue;
        } else {
            return {false, "Unknown parameter: " + key};
        }
    }

    return result;
}

bool ViewLogsCommand::execute(const CommandArgs &args) {
    std::optional<models::LogLevel> level;
    std::optional<models::ActionType> action;
    std::optional<std::string> actor_id;
    std::optional<std::string> subject_id;
    std::optional<std::chrono::system_clock::time_point> start_time;
    std::optional<std::chrono::system_clock::time_point> end_time;
    size_t limit = 100;

    // Parse options
    if (args.options.count("level")) {
    level = models::string_to_log_level_optional(args.options.at("level"));
    if (!level.has_value()) {
        io_handler_->println("Invalid log level: " + args.options.at("level"));
        return true;
        }
    }

    if (args.options.count("action")) {
        action = models::string_to_action_type_optional(args.options.at("action"));
        if (!action.has_value()) {
            io_handler_->println("Invalid action type: " + args.options.at("action"));
            return true;
        }
    }

    if (args.options.count("actor")) {
        actor_id = args.options.at("actor");
    }
    if (args.options.count("subject")) {
        subject_id = args.options.at("subject");
    }

    // Parse time safely using a public helper
    if (args.options.count("start")) {
        start_time = log_service_->parse_time(args.options.at("start"));
        if (!start_time.has_value()) {
            io_handler_->println("Invalid start time: " + args.options.at("start"));
            return true;
        }
    }
    if (args.options.count("end")) {
        end_time = log_service_->parse_time(args.options.at("end"));
        if (!end_time.has_value()) {
            io_handler_->println("Invalid end time: " + args.options.at("end"));
            return true;
        }
    }

    if (args.options.count("limit")) {
        try {
            limit = static_cast<size_t>(std::stoul(args.options.at("limit")));
            if (limit == 0) {
                io_handler_->println("Limit must be greater than 0");
                return true;
            }
    } catch (...) {
        io_handler_->println("Invalid limit value");
        return true;
        }
    }

    // Get logs
    auto logs = log_service_->get_logs(level, action, actor_id, subject_id,
                                       start_time, end_time, limit);

    if (logs.empty()) {
        io_handler_->println("No logs found");
        return true;
    }

    io_handler_->println("Requested logs:");
    io_handler_->println("----------");

    for (const auto &log_entry : logs) {
        std::ostringstream ss;
        ss << "[" << log_entry->timestamp() << "] ["
           << models::to_string(log_entry->level()) << "] "
           << log_entry->message();
        io_handler_->println(ss.str());
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
                "view-logs",
                "Show recent system logs (supports filters)",
                "view-logs [--limit=N] [--level=LEVEL] [--action=ACTION] "
                "[--actor=ID] [--subject=ID] [--start=\"YYYY-MM-DD HH:MM:SS\"] "
                "[--end=\"YYYY-MM-DD HH:MM:SS\"]",
                app_state, io, auth, user, log, d);
        });
    return true;
}();
} // namespace
