#include "log_dao.hpp"
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/query.hxx>
#include <odb/sql-query.hxx>
#include <spdlog/spdlog.h>
#include <sstream>
#include <iomanip>


namespace dao {

using LogResult = odb::result<models::SystemLog>;

LogDAO::LogDAO(std::shared_ptr<odb::database> db) : database_(std::move(db)) {}

bool LogDAO::save(const std::shared_ptr<models::SystemLog>& log) {
    if (!log) {
        SPDLOG_WARN("Attempt to save null log");
        return false;
    }
    try
    {
         odb::transaction t(database_->begin());
        database_->persist(*log);
        t.commit();
        
        SPDLOG_DEBUG("SystemLog saved: {} - {}", to_string(log->level()), log->message().substr(0, 50));
        return true;
    }
    catch(const std::exception& e)
    {
        SPDLOG_ERROR("Error saving SystemLog: {}", e.what());
        return false;
    }
}
    
std::shared_ptr<models::SystemLog> LogDAO::find_by_id(const std::string& id) {
    if (id.empty()) {
        SPDLOG_WARN("Attempt to find log with empty ID");
        return nullptr;
    }
    odb::transaction t(database_->begin());
    try {
        auto log = database_->load<models::SystemLog>(id);
        t.commit();
        return std::make_shared<models::SystemLog>(*log);
    } catch (const odb::object_not_persistent& e) {
        SPDLOG_DEBUG("SystemLog not found with ID: {}", id);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error finding SystemLog by ID {}: {}", id, e.what());
    }
    
    t.commit();
    return nullptr;
}

bool LogDAO::remove(const std::shared_ptr<models::SystemLog>& log) {
    if (!log) {
        SPDLOG_WARN("Attempt to remove null log");
        return false;
    }

    try
    {
        obd::transaction t(database_->begin());
        database_->erase(*log);
        t.commit();
        SPDLOG_INFO("SystemLog removed: {}", log->id());
        return true;
    }   catch(const std::exception& e)
    {
        SPDLOG_INFO("SystemLog removed: {}", log->id(), e.what());
        return false;
    }
    
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_recent_logs(size_t limit){
    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::SystemLog>> logs;

    try
    {
        typedef odb::query<models::SystemLog> query;
        LogResult result = database_->query<models::SystemLog>(
            query::true_expr,
            odb::query<models::SystemLog>::timestamp.desc(),
            limit
        );

        for(const auto& log : result) logs.push_back(std::make_shared<models::SystemLog>(log));
        
        SPDLOG_DEBUG("Found {} recent SystemLog entries", logs.size());
    }   catch(const std::exception& e)
    {
        SPDLOG_ERROR("Error finding recent SystemLogs: {}", e.what());
    }
    
    t.commit();
    return logs;
} 

LogQueryResult LogDAO::find_by_filter(const LogFilter& filter, const Pagination& pagination ){
    odb::transaction t(database_->begin());
    LogQueryResult result;
    try
    {   
        std::string condition = build_filter_condition(filter);
        std::string count_query_str = "SELECT COUNT(*) FROM system_log";
        
        if(!condition.empty()) count_query_str += " WHERE " + condition;

        odb::sql_query count_query(database_->connection(), count_query_str);
        odb::result<odb::result_size> count_result(count_query);
        result.total_count = *count_result.begin();
        result.total_pages = (result.total_count + pagination.page_size - 1) / pagination.page_size;

        std::string data_query_str = "SELECT * FROM system_log";
        if (!condition.empty()) data_query_str += " WHERE " + condition;
        data_query_str += " ORDER BY timestamp DESC LIMIT " + std::to_string(pagination.page_size) + " OFFSET " + std::to_string(pagination.offset());
        odb::sql_query data_query(database_->connection(), data_query_str);
        odb::result<models::SystemLog> data_result(data_query);

        for(const auto& log : data_result) result.logs.push_back(std::make_shared<models::SystemLog>(log));

        SPDLOG_DEBUG("Found {} SystemLog entries with filter", result.logs.size());
    }
    catch(const std::exception& e)
    {
        SPDLOG_ERROR("Error finding SystemLogs by filter: {}", e.what());
    }
    t.commit();
    
}


std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_level(LogLevel level, size_t limit) {
    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        typedef odb::query<models::SystemLog> query;
        LogResult result = database_->query<models::SystemLog>(
            query::level == level,
            odb::query<models::SystemLog>::timestamp.desc(),
            limit
        );
        
        for (const auto& log : result) logs.push_back(std::make_shared<models::SystemLog>(log));
        
        SPDLOG_DEBUG("Found {} SystemLog entries with level '{}'", logs.size(), to_string(level));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error finding SystemLogs by level: {}", e.what());
    }
    
    t.commit();
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_action_type(ActionType action_type, size_t limit) {
    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        typedef odb::query<models::SystemLog> query;
        LogResult result = database_->query<models::SystemLog>(
            query::action_type == action_type,
            odb::query<models::SystemLog>::timestamp.desc(),
            limit
        );
        
        for (const auto& log : result) logs.push_back(std::make_shared<models::SystemLog>(log));
        
        SPDLOG_DEBUG("Found {} SystemLog entries with action type '{}'", logs.size(), to_string(action_type));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error finding SystemLogs by action type: {}", e.what());
    }
    
    t.commit();
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_actor(const std::string& actor_id, size_t limit) {
    if (actor_id.empty()) return {};

    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        typedef odb::query<models::SystemLog> query;
        LogResult result = database_->query<models::SystemLog>(
            query::actor_id == actor_id,
            odb::query<models::SystemLog>::timestamp.desc(),
            limit
        );
        
        for (const auto& log : result) logs.push_back(std::make_shared<models::SystemLog>(log));
        
        SPDLOG_DEBUG("Found {} SystemLog entries for actor '{}'", logs.size(), actor_id);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error finding SystemLogs by actor: {}", e.what());
    }
    
    t.commit();
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_subject(const std::string& subject_id, size_t limit) {
    if (subject_id.empty()) return {};

    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        typedef odb::query<models::SystemLog> query;
        LogResult result = database_->query<models::SystemLog>(
            query::subject_id == subject_id,
            odb::query<models::SystemLog>::timestamp.desc(),
            limit
        );
        
        for (const auto& log : result) logs.push_back(std::make_shared<models::SystemLog>(log));
        
        SPDLOG_DEBUG("Found {} SystemLog entries for subject '{}'", logs.size(), subject_id);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error finding SystemLogs by subject: {}", e.what());
    }
    
    t.commit();
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_ip_address(const std::string& ip_address, size_t limit) {
    if (ip_address.empty()) return {};

    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        typedef odb::query<models::SystemLog> query;
        LogResult result = database_->query<models::SystemLog>(
            query::ip_address == ip_address,
            odb::query<models::SystemLog>::timestamp.desc(),
            limit
        );
        
        for (const auto& log : result) logs.push_back(std::make_shared<models::SystemLog>(log));
        
        SPDLOG_DEBUG("Found {} SystemLog entries for IP '{}'", logs.size(), ip_address);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error finding SystemLogs by IP: {}", e.what());
    }
    
    t.commit();
    return logs;
}

size_t LogDAO::get_log_count(const LogFilter& filter) {
    odb::transaction t(database_->begin());
    
    try {
        std::string condition = build_filter_condition(filter);
        std::string query_str = "SELECT COUNT(*) FROM system_log";
        
        if (!condition.empty()) query_str += " WHERE " + condition;
        
        odb::sql_query count_query(database_->connection(), query_str);
        odb::result<odb::result_size> result(count_query);
        
        t.commit();
        return *result.begin();
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error getting SystemLog count: {}", e.what());
        t.commit();
        return 0;
    }
}

std::vector<std::pair<LogLevel, size_t>> LogDAO::get_log_level_distribution() {
    odb::transaction t(database_->begin());
    std::vector<std::pair<LogLevel, size_t>> distribution;
    
    try {
        std::string query_str = "SELECT level, COUNT(*) FROM system_log GROUP BY level ORDER BY COUNT(*) DESC";
        
        odb::sql_query dist_query(database_->connection(), query_str);
        odb::result<odb::sql_result::select> result(dist_query);
        
        for (const auto& row : result) {
            LogLevel level;
            size_t count;
            row >> level >> count;
            distribution.emplace_back(level, count);
        }
        
        SPDLOG_DEBUG("Calculated level distribution for {} levels", distribution.size());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error getting SystemLog level distribution: {}", e.what());
    }
    
    t.commit();
    return distribution;
}

std::vector<std::pair<ActionType, size_t>> LogDAO::get_action_type_distribution() {
    odb::transaction t(database_->begin());
    std::vector<std::pair<ActionType, size_t>> distribution;
    
    try {
        std::string query_str = "SELECT action_type, COUNT(*) FROM system_log GROUP BY action_type ORDER BY COUNT(*) DESC";
        
        odb::sql_query dist_query(database_->connection(), query_str);
        odb::result<odb::sql_result::select> result(dist_query);
        
        for (const auto& row : result) {
            ActionType action_type;
            size_t count;
            row >> action_type >> count;
            distribution.emplace_back(action_type, count);
        }
        
        SPDLOG_DEBUG("Calculated action type distribution for {} types", distribution.size());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error getting SystemLog action type distribution: {}", e.what());
    }
    
    t.commit();
    return distribution;
}

bool LogDAO::cleanup_old_logs(const std::chrono::system_clock::time_point& before) {
    try {
        odb::transaction t(database_->begin());
        
        std::string timestamp_str = time_point_to_sql(before);
        std::string query_str = "DELETE FROM system_log WHERE timestamp < '" + timestamp_str + "'";
        
        odb::sql_query delete_query(database_->connection(), query_str);
        delete_query.execute();
        
        t.commit();
        
        SPDLOG_INFO("Cleaned up old SystemLog entries before {}", timestamp_str);
        return true;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error cleaning up old SystemLogs: {}", e.what());
        return false;
    }
}

bool LogDAO::delete_logs_by_filter(const LogFilter& filter) {
    try {
        odb::transaction t(database_->begin());
        
        std::string condition = build_filter_condition(filter);
        std::string query_str = "DELETE FROM system_log";
        
        if (!condition.empty()) {
            query_str += " WHERE " + condition;
        }
        
        odb::sql_query delete_query(database_->connection(), query_str);
        delete_query.execute();
        
        t.commit();
        
        SPDLOG_INFO("Deleted SystemLog entries by filter");
        return true;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error deleting SystemLogs by filter: {}", e.what());
        return false;
    }
}

std::string LogDAO::build_filter_condition(const LogFilter& filter) {
    std::vector<std::string> conditions;
    
    if (filter.level != LogLevel::UNKNOWN) conditions.push_back("level = " + std::to_string(static_cast<int>(filter.level)));
    
    if (filter.action_type != ActionType::UNKNOWN) conditions.push_back("action_type = " + std::to_string(static_cast<int>(filter.action_type)));
    
    if (!filter.actor_id.empty()) conditions.push_back("actor_id = " + database_->quote(filter.actor_id));
    
    if (!filter.subject_id.empty()) conditions.push_back("subject_id = " + database_->quote(filter.subject_id));
    
    if (!filter.ip_address.empty()) conditions.push_back("ip_address = " + database_->quote(filter.ip_address));
    
    if (filter.has_time_range()) conditions.push_back("timestamp >= '" + time_point_to_sql(filter.start_time) + "' AND timestamp <= '" + time_point_to_sql(filter.end_time) + "'");
    
    if (!filter.message_pattern.empty()) conditions.push_back("message LIKE " + database_->quote("%" + filter.message_pattern + "%"));
    
    if (conditions.empty()) return "";
    
    std::string result;
    for (size_t i = 0; i < conditions.size(); ++i) {
        if (i > 0) {
            result += " AND ";
        }
        result += conditions[i];
    }
    
    return result;
}

std::string LogDAO::time_point_to_sql(const std::chrono::system_clock::time_point& tp) {
    auto time_t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::gmtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

}
