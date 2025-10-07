#pragma once

#include "src/models/system_log.hpp"
#include "src/models/user.hpp"
#include "src/models/enums.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace dao {
    class LogDAO;
}

class LogService {
public:
    explicit LogService(std::shared_ptr<dao::LogDAO> log_dao);

    void log(models::LogLevel level, models::ActionType action_type, const std::string &message,
             const std::shared_ptr<const models::User> &actor = nullptr,
             const std::shared_ptr<const models::User> &subject = nullptr,
             const std::string &ip_address = "",
             const std::string &user_agent = "");

    void debug(models::ActionType action_type, const std::string &message,
               const std::shared_ptr<const models::User> &actor = nullptr,
               const std::shared_ptr<const models::User> &subject = nullptr,
               const std::string &ip_address = "",
               const std::string &user_agent = "");
    void info(models::ActionType action_type, const std::string &message,
              const std::shared_ptr<const models::User> &actor = nullptr,
              const std::shared_ptr<const models::User> &subject = nullptr,
              const std::string &ip_address = "",
              const std::string &user_agent = "");
    void warning(models::ActionType action_type, const std::string &message,
                 const std::shared_ptr<const models::User> &actor = nullptr,
                 const std::shared_ptr<const models::User> &subject = nullptr,
                 const std::string &ip_address = "",
                 const std::string &user_agent = "");
    void error(models::ActionType action_type, const std::string &message,
               const std::shared_ptr<const models::User> &actor = nullptr,
               const std::shared_ptr<const models::User> &subject = nullptr,
               const std::string &ip_address = "",
               const std::string &user_agent = "");
    void critical(models::ActionType action_type, const std::string &message,
                  const std::shared_ptr<const models::User> &actor = nullptr,
                  const std::shared_ptr<const models::User> &subject = nullptr,
                  const std::string &ip_address = "",
                  const std::string &user_agent = "");

    // Logs Receive
    std::vector<std::shared_ptr<models::SystemLog>>
    get_recent_logs(size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_level(models::LogLevel level, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_action(models::ActionType action_type, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_actor_id(const std::string &actor_id, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_subject_id(const std::string &subject_id, size_t limit = 100);

    std::vector<std::shared_ptr<models::SystemLog>>
    get_logs_by_date_range(const std::string &start_date,
                           const std::string &end_date, size_t limit);

    // Logs management
    bool cleanup_old_logs(int days_to_keep = 30);
    bool delete_logs(const std::vector<std::shared_ptr<models::SystemLog>> &logs);

    // Logs stats
    size_t get_total_log_count();

private:
    std::shared_ptr<dao::LogDAO> log_dao_;

    std::shared_ptr<models::SystemLog> create_log_entry(
        models::LogLevel level, models::ActionType action_type, const std::string &message,
        const std::shared_ptr<const models::User> &actor,
        const std::shared_ptr<const models::User> &subject,
        const std::string &ip_address, const std::string &user_agent);

    std::chrono::system_clock::time_point
    sql_string_to_time_point(const std::string &sql_time) const;
};
