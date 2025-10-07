#include "log_service.hpp"
#include "src/cli_app.hpp"
#include "src/models/system_log.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

LogService::LogService(std::shared_ptr<dao::LogDAO> log_dao)
    : log_dao_(std::move(log_dao)) {}

void LogService::log(LogLevel level, ActionType action_type,
                     const std::string &message,
                     const std::shared_ptr<models::User> &actor,
                     const std::shared_ptr<models::User> &subject,
                     const std::string &ip_address,
                     const std::string &user_agent) {
    std::shared_ptr<models::SystemLog> entry = create_log_entry(
        level, action_type, message, actor, subject, ip_address, user_agent);
    log_dao_->save(entry);
}

std::shared_ptr<models::SystemLog> LogService::create_log_entry(
    LogLevel level, ActionType action_type, const std::string &message,
    const std::shared_ptr<models::User> &actor,
    const std::shared_ptr<models::User> &subject, const std::string &ip_address,
    const std::string &user_agent) {

    std::shared_ptr<models::SystemLog> entry =
        std::make_shared<models::SystemLog>(level, action_type, message);

    if (actor) {
        entry->set_actor(odb::lazy_shared_ptr<models::User>(actor));
    }
    if (subject) {
        entry->set_subject(odb::lazy_shared_ptr<models::User>(subject));
    }
    if (!ip_address.empty()) {
        entry->set_ip_address(ip_address);
    }
    if (!user_agent.empty()) {
        entry->set_user_agent(user_agent);
    }

    return entry;
}

void LogService::debug(ActionType action_type, const std::string &message,
                       const std::shared_ptr<models::User> &actor,
                       const std::shared_ptr<models::User> &subject,
                       const std::string &ip_address,
                       const std::string &user_agent) {
    log(LogLevel::DEBUG, action_type, message, actor, subject, ip_address,
        user_agent);
}

void LogService::info(ActionType action_type, const std::string &message,
                      const std::shared_ptr<models::User> &actor,
                      const std::shared_ptr<models::User> &subject,
                      const std::string &ip_address,
                      const std::string &user_agent) {
    log(LogLevel::INFO, action_type, message, actor, subject, ip_address,
        user_agent);
}

void LogService::warning(ActionType action_type, const std::string &message,
                         const std::shared_ptr<models::User> &actor,
                         const std::shared_ptr<models::User> &subject,
                         const std::string &ip_address,
                         const std::string &user_agent) {
    log(LogLevel::WARNING, action_type, message, actor, subject, ip_address,
        user_agent);
}

void LogService::error(ActionType action_type, const std::string &message,
                       const std::shared_ptr<models::User> &actor,
                       const std::shared_ptr<models::User> &subject,
                       const std::string &ip_address,
                       const std::string &user_agent) {
    log(LogLevel::ERROR, action_type, message, actor, subject, ip_address,
        user_agent);
}

void LogService::critical(ActionType action_type, const std::string &message,
                          const std::shared_ptr<models::User> &actor,
                          const std::shared_ptr<models::User> &subject,
                          const std::string &ip_address,
                          const std::string &user_agent) {
    log(LogLevel::CRITICAL, action_type, message, actor, subject, ip_address,
        user_agent);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_recent_logs(size_t limit) {
    return log_dao_->find_recent_logs(limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_level(LogLevel level, size_t limit) {
    return log_dao_->find_by_level(level, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_action(ActionType action_type, size_t limit) {
    return log_dao_->find_by_action_type(action_type, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_actor_id(const std::string &actor_id, size_t limit) {
    return log_dao_->find_by_actor(actor_id, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_subject_id(const std::string &subject_id,
                                   size_t limit) {
    return log_dao_->find_by_subject(subject_id, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_date_range(const std::string &start_date,
                                   const std::string &end_date, size_t limit) {
    auto start_tp = sql_string_to_time_point(start_date);
    auto end_tp = sql_string_to_time_point(end_date);

    if (start_tp.time_since_epoch().count() == 0 ||
        end_tp.time_since_epoch().count() == 0) {
        return {};
    }

    return get_logs_by_time_range(start_tp, end_tp, limit);
}

// TODO: Optionally implement search by time range in LogDAO
// std::vector<std::shared_ptr<models::SystemLog>>
// LogService::get_logs_by_date_range(const std::string &start_date,
//                                    const std::string &end_date, size_t limit)
//                                    {
//     return log_dao_->find_by_date_range(start_date, end_date, limit);
// }

bool LogService::cleanup_old_logs(int days_to_keep) {
    std::chrono::time_point now = std::chrono::system_clock::now();
    std::chrono::time_point days_to_substract =
        std::chrono::hours(24 * days_to_keep);
    std::chrono::time_point cutoff_time = now - days_to_substract;

    return log_dao_->cleanup_old_logs(cutoff_time);
}

bool LogService::delete_logs(const std::vector<std::shared_ptr<models::SystemLog>> &logs) {
    bool all_deleted = true;
    for (auto &log : logs) {
        if (!log_dao_->remove(log)) {
            all_deleted = false;
        }
    }
    return all_deleted;
}

size_t LogService::get_total_log_count() { return log_dao_->get_log_count(); }

std::chrono::system_clock::time_point
LogService::sql_string_to_time_point(const std::string &sql_time) const {
    std::tm tm = {};
    std::istringstream ss(sql_time);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        return std::chrono::system_clock::time_point{};
    }

    std::time_t time_t_val = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time_t_val);
}

} // namespace services
