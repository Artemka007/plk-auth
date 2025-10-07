#pragma once

#include "models/SystemLog.hpp"
#include <memory>
#include <string>
#include <vector>

namespace dao {
class LogDAO;
}

class LogService {
public:
    explicit LogService(std::shared_ptr<dao::LogDAO> log_dao);

    void log(LogLevel level, ActionType action_type, const std::string &message,
             const std::shared_ptr<models::User> &actor = nullptr,
             const std::shared_ptr<models::User> &subject = nullptr,
             const std::string &ip_address = "",
             const std::string &user_agent = "");

    void debug(ActionType action_type, const std::string &message,
               const std::shared_ptr<models::User> &actor = nullptr,
               const std::shared_ptr<models::User> &subject = nullptr,
               const std::string &ip_address = "",
               const std::string &user_agent = "");
    void info(ActionType action_type, const std::string &message,
              const std::shared_ptr<models::User> &actor = nullptr,
              const std::shared_ptr<models::User> &subject = nullptr,
              const std::string &ip_address = "",
              const std::string &user_agent = "");
    void warning(ActionType action_type, const std::string &message,
                 const std::shared_ptr<models::User> &actor = nullptr,
                 const std::shared_ptr<models::User> &subject = nullptr,
                 const std::string &ip_address = "",
                 const std::string &user_agent = "");
    void error(ActionType action_type, const std::string &message,
               const std::shared_ptr<models::User> &actor = nullptr,
               const std::shared_ptr<models::User> &subject = nullptr,
               const std::string &ip_address = "",
               const std::string &user_agent = "");
    void critical(ActionType action_type, const std::string &message,
                  const std::shared_ptr<models::User> &actor = nullptr,
                  const std::shared_ptr<models::User> &subject = nullptr,
                  const std::string &ip_address = "",
                  const std::string &user_agent = "");

    // Logs Receive
    std::vector<std::shared_ptr<models::SystemLog>>
    get_recent_logs(size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_level(LogLevel level, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_action(ActionType action_type, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_actor_id(const std::string &user_id, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_actor_email(const std::string &email, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_subject_id(const std::string &user_id, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_subject_email(const std::string &email, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_date_range(const std::string &start_date,
                           const std::string &end_date, size_t limit = 100);

    // Logs management
    bool cleanup_old_logs(int days_to_keep = 30);
    bool delete_log_entry(const std::string &log_id);

    // Logs stats
    size_t get_total_log_count();
    std::vector<std::shared_ptr<models::SystemLog>>
    search_logs(const std::string &query, size_t limit = 100);

private:
    std::shared_ptr<dao::LogDAO> log_dao_;

    std::shared_ptr<models::SystemLog> create_log_entry(
        LogLevel level, ActionType action_type, const std::string &message,
        const std::shared_ptr<models::User> &actor,
        const std::shared_ptr<models::User> &subject,
        const std::string &ip_address, const std::string &user_agent);
};
