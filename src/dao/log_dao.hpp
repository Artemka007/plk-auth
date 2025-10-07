#pragma once
#include <memory>
#include <vector>
#include <string>
#include <odb/database.hxx>
#include <system_log.hpp>

namespace dao {
struct LogFilter {
    LogLevel level;
    ActionType action_type;
    std::string actor_id;
    std::string subject_id;
    std::string message_pattern;
    std::string ip_address;
    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point end_time;
    
    bool has_time_range() const {
        return start_time != std::chrono::system_clock::time_point{} &&
               end_time != std::chrono::system_clock::time_point{};
    }
};

struct Pagination {
    size_t page = 1;
    size_t page_size = 50;
    
    size_t offset() const { return (page - 1) * page_size; }
};

struct LogQueryResult {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    size_t total_count;
    size_t total_pages;
};

class LogDAO {
public:
    explicit LogDAO(std::shared_ptr<odb::database> db);
    
    // операции с логами
    bool save(const std::shared_ptr<models::SystemLog>& log);
    std::shared_ptr<models::SystemLog> find_by_id(const std::string& id);
    bool remove(const std::shared_ptr<models::SystemLog>& log);
    
    // методы запросов
    std::vector<std::shared_ptr<models::SystemLog>> find_recent_logs(size_t limit = 100);
    LogQueryResult find_by_filter(const LogFilter& filter, const Pagination& pagination = {});
    std::vector<std::shared_ptr<models::SystemLog>> find_by_level(LogLevel level, size_t limit = 100);
    std::vector<std::shared_ptr<models::SystemLog>> find_by_action_type(ActionType action_type, size_t limit = 100);
    std::vector<std::shared_ptr<models::SystemLog>> find_by_actor(const std::string& actor_id, size_t limit = 100);
    std::vector<std::shared_ptr<models::SystemLog>> find_by_subject(const std::string& subject_id, size_t limit = 100);
    std::vector<std::shared_ptr<models::SystemLog>> find_by_ip_address(const std::string& ip_address, size_t limit = 100);
    
    // статистика
    size_t get_log_count(const LogFilter& filter = {});
    std::vector<std::pair<LogLevel, size_t>> get_log_level_distribution();
    std::vector<std::pair<ActionType, size_t>> get_action_type_distribution();
    
    // очистка логов
    bool cleanup_old_logs(const std::chrono::system_clock::time_point& before);
    bool delete_logs_by_filter(const LogFilter& filter);

private:
    std::shared_ptr<odb::database> database_;
    
    std::string build_filter_condition(const LogFilter& filter);
    std::string time_point_to_sql(const std::chrono::system_clock::time_point& tp);
};
}
