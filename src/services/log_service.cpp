#include "services/log_service.hpp"
#include "dao/log_dao.hpp"
#include "models/system_log.hpp"
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
    return log_dao_->find_recent(limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_level(LogLevel level, size_t limit) {
    return log_dao_->find_by_level(level, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_action(ActionType action_type, size_t limit) {
    return log_dao_->find_by_action(action_type, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_actor_email(const std::string &email, size_t limit) {
    return log_dao_->find_by_actor_email(email, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_subject_email(const std::string &email, size_t limit) {
    return log_dao_->find_by_subject_email(email, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_actor_id(const std::string &user_id, size_t limit) {
    return log_dao_->find_by_actor_id(user_id, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_subject_id(const std::string &user_id, size_t limit) {
    return log_dao_->find_by_subject_id(user_id, limit);
}

std::vector<std::shared_ptr<models::SystemLog>>
LogService::get_logs_by_date_range(const std::string &start_date,
                                   const std::string &end_date, size_t limit) {
    return log_dao_->find_by_date_range(start_date, end_date, limit);
}

bool LogService::cleanup_old_logs(int days_to_keep) {
    return log_dao_->delete_older_than(days_to_keep);
}

bool LogService::delete_log_entry(const std::string &log_id) {
    return log_dao_->delete_by_id(log_id);
}

size_t LogService::get_total_log_count() { return log_dao_->get_total_count(); }

std::vector<std::shared_ptr<models::SystemLog>>
LogService::search_logs(const std::string &query, size_t limit) {
    return log_dao_->search(query, limit);
}

} // namespace services
