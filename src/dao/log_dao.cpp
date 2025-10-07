#include "log_dao.hpp"
#include <pqxx/pqxx>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "../models/enums.hpp"
#include "../models/system_log.hpp"

namespace dao {

LogDAO::LogDAO(std::shared_ptr<pqxx::connection> conn) 
    : connection_(std::move(conn)) {
}

bool LogDAO::save(const std::shared_ptr<models::SystemLog>& log) {
    try {
        if (log->id().empty()) {
            log->set_id(generate_uuid());
        }
        
        pqxx::work txn(*connection_);
        
        std::string actor_id = log->actor_id().empty() ? "NULL" : txn.quote(log->actor_id());
        std::string subject_id = log->subject_id().empty() ? "NULL" : txn.quote(log->subject_id());
        std::string ip_address = log->ip_address().value_or("NULL");
        std::string user_agent = log->user_agent().value_or("NULL");
        
        if (ip_address != "NULL") ip_address = txn.quote(ip_address);
        if (user_agent != "NULL") user_agent = txn.quote(user_agent);
        
        txn.exec(
            "INSERT INTO system_log (id, level, action_type, message, "
            "actor_id, subject_id, ip_address, user_agent) "
            "VALUES (" + 
            txn.quote(log->id()) + ", " +
            txn.quote(log->level_string()) + ", " +
            txn.quote(log->action_type_string()) + ", " +
            txn.quote(log->message()) + ", " +
            actor_id + ", " +
            subject_id + ", " +
            ip_address + ", " +
            user_agent + ")"
        );
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::save: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<models::SystemLog> LogDAO::find_by_id(const std::string& id) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log WHERE id = " + txn.quote(id));
        
        txn.commit();
        
        if (result.empty()) {
            return nullptr;
        }
        
        auto log = std::make_shared<models::SystemLog>();
        log->from_row(result[0]);
        return log;
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_id: " << e.what() << std::endl;
        return nullptr;
    }
}

bool LogDAO::remove(const std::shared_ptr<models::SystemLog>& log) {
    try {
        pqxx::work txn(*connection_);
        txn.exec("DELETE FROM system_log WHERE id = " + txn.quote(log->id()));
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::remove: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_recent_logs(size_t limit) {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log ORDER BY timestamp DESC LIMIT " + std::to_string(limit));
        
        txn.commit();
        
        for (const auto& row : result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            logs.push_back(log);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_recent_logs: " << e.what() << std::endl;
    }
    
    return logs;
}

LogQueryResult LogDAO::find_by_filter(const LogFilter& filter, const Pagination& pagination) {
    LogQueryResult result;
    
    try {
        pqxx::work txn(*connection_);
        
        std::string where_clause = build_filter_condition(filter);
        std::string sql = "SELECT id, level, action_type, message, timestamp, "
                         "actor_id, subject_id, ip_address, user_agent "
                         "FROM system_log";
        
        if (!where_clause.empty()) {
            sql += " WHERE " + where_clause;
        }
        
        sql += " ORDER BY timestamp DESC "
               "LIMIT " + std::to_string(pagination.page_size) + 
               " OFFSET " + std::to_string(pagination.offset());
        
        auto query_result = txn.exec(sql);
        
        // Получаем общее количество для пагинации
        std::string count_sql = "SELECT COUNT(*) FROM system_log";
        if (!where_clause.empty()) {
            count_sql += " WHERE " + where_clause;
        }
        
        auto count_result = txn.exec(count_sql);
        size_t total_count = count_result[0][0].as<size_t>();
        
        txn.commit();
        
        // Заполняем результат
        for (const auto& row : query_result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            result.logs.push_back(log);
        }
        
        result.total_count = total_count;
        result.total_pages = (total_count + pagination.page_size - 1) / pagination.page_size;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_filter: " << e.what() << std::endl;
    }
    
    return result;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_level(models::LogLevel level, size_t limit) {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log WHERE level = " + txn.quote(models::to_string(level)) + 
            " ORDER BY timestamp DESC LIMIT " + std::to_string(limit));
        
        txn.commit();
        
        for (const auto& row : result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            logs.push_back(log);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_level: " << e.what() << std::endl;
    }
    
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_action_type(models::ActionType action_type, size_t limit) {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log WHERE action_type = " + txn.quote(models::to_string(action_type)) + 
            " ORDER BY timestamp DESC LIMIT " + std::to_string(limit));
        
        txn.commit();
        
        for (const auto& row : result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            logs.push_back(log);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_action_type: " << e.what() << std::endl;
    }
    
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_actor(const std::string& actor_id, size_t limit) {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log WHERE actor_id = " + txn.quote(actor_id) + 
            " ORDER BY timestamp DESC LIMIT " + std::to_string(limit));
        
        txn.commit();
        
        for (const auto& row : result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            logs.push_back(log);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_actor: " << e.what() << std::endl;
    }
    
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_subject(const std::string& subject_id, size_t limit) {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log WHERE subject_id = " + txn.quote(subject_id) + 
            " ORDER BY timestamp DESC LIMIT " + std::to_string(limit));
        
        txn.commit();
        
        for (const auto& row : result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            logs.push_back(log);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_subject: " << e.what() << std::endl;
    }
    
    return logs;
}

std::vector<std::shared_ptr<models::SystemLog>> LogDAO::find_by_ip_address(const std::string& ip_address, size_t limit) {
    std::vector<std::shared_ptr<models::SystemLog>> logs;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, level, action_type, message, timestamp, "
            "actor_id, subject_id, ip_address, user_agent "
            "FROM system_log WHERE ip_address = " + txn.quote(ip_address) + 
            " ORDER BY timestamp DESC LIMIT " + std::to_string(limit));
        
        txn.commit();
        
        for (const auto& row : result) {
            auto log = std::make_shared<models::SystemLog>();
            log->from_row(row);
            logs.push_back(log);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::find_by_ip_address: " << e.what() << std::endl;
    }
    
    return logs;
}

size_t LogDAO::get_log_count(const LogFilter& filter) {
    try {
        pqxx::work txn(*connection_);
        
        std::string where_clause = build_filter_condition(filter);
        std::string sql = "SELECT COUNT(*) FROM system_log";
        
        if (!where_clause.empty()) {
            sql += " WHERE " + where_clause;
        }
        
        auto result = txn.exec(sql);
        txn.commit();
        
        return result[0][0].as<size_t>();
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::get_log_count: " << e.what() << std::endl;
        return 0;
    }
}

std::vector<std::pair<models::LogLevel, size_t>> LogDAO::get_log_level_distribution() {
    std::vector<std::pair<models::LogLevel, size_t>> distribution;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT level, COUNT(*) FROM system_log "
            "GROUP BY level ORDER BY COUNT(*) DESC");
        
        txn.commit();
        
        for (const auto& row : result) {
            std::string level_str = row["level"].as<std::string>();
            models::LogLevel level = models::string_to_log_level(level_str);
            size_t count = row["count"].as<size_t>();
            distribution.emplace_back(level, count);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::get_log_level_distribution: " << e.what() << std::endl;
    }
    
    return distribution;
}

std::vector<std::pair<models::ActionType, size_t>> LogDAO::get_action_type_distribution() {
    std::vector<std::pair<models::ActionType, size_t>> distribution;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT action_type, COUNT(*) FROM system_log "
            "GROUP BY action_type ORDER BY COUNT(*) DESC");
        
        txn.commit();
        
        for (const auto& row : result) {
            std::string action_str = row["action_type"].as<std::string>();
            models::ActionType action_type = models::string_to_action_type(action_str);
            size_t count = row["count"].as<size_t>();
            distribution.emplace_back(action_type, count);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::get_action_type_distribution: " << e.what() << std::endl;
    }
    
    return distribution;
}

bool LogDAO::cleanup_old_logs(const std::chrono::system_clock::time_point& before) {
    try {
        pqxx::work txn(*connection_);
        
        std::string timestamp = time_point_to_sql(before);
        auto result = txn.exec(
            "DELETE FROM system_log WHERE timestamp < " + txn.quote(timestamp));
        
        txn.commit();
        
        std::cout << "Cleaned up " << result.affected_rows() << " old logs" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::cleanup_old_logs: " << e.what() << std::endl;
        return false;
    }
}

bool LogDAO::delete_logs_by_filter(const LogFilter& filter) {
    try {
        pqxx::work txn(*connection_);
        
        std::string where_clause = build_filter_condition(filter);
        if (where_clause.empty()) {
            std::cerr << "Warning: Attempt to delete all logs without filter" << std::endl;
            return false;
        }
        
        std::string sql = "DELETE FROM system_log WHERE " + where_clause;
        auto result = txn.exec(sql);
        
        txn.commit();
        
        std::cout << "Deleted " << result.affected_rows() << " logs by filter" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in LogDAO::delete_logs_by_filter: " << e.what() << std::endl;
        return false;
    }
}

// Вспомогательные методы
std::string LogDAO::build_filter_condition(const LogFilter& filter) {
    std::vector<std::string> conditions;
    
    if (filter.level != models::LogLevel{}) {
        conditions.push_back("level = '" + models::to_string(filter.level) + "'");
    }
    
    if (filter.action_type != models::ActionType{}) {
        conditions.push_back("action_type = '" + models::to_string(filter.action_type) + "'");
    }
    
    if (!filter.actor_id.empty()) {
        conditions.push_back("actor_id = '" + filter.actor_id + "'");
    }
    
    if (!filter.subject_id.empty()) {
        conditions.push_back("subject_id = '" + filter.subject_id + "'");
    }
    
    if (!filter.message_pattern.empty()) {
        conditions.push_back("message ILIKE '%" + filter.message_pattern + "%'");
    }
    
    if (!filter.ip_address.empty()) {
        conditions.push_back("ip_address = '" + filter.ip_address + "'");
    }
    
    if (filter.has_time_range()) {
        std::string start_time = time_point_to_sql(filter.start_time);
        std::string end_time = time_point_to_sql(filter.end_time);
        conditions.push_back("timestamp BETWEEN '" + start_time + "' AND '" + end_time + "'");
    }
    
    if (conditions.empty()) {
        return "";
    }
    
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
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Вспомогательная функция для генерации UUID
std::string generate_uuid() {
    return "ss.str()";
}

} // namespace dao