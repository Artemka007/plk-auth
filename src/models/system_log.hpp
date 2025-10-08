#pragma once
#include <string>
#include <memory>
#include <pqxx/pqxx>
#include "enums.hpp"
#include "user.hpp"

namespace models {

class SystemLog {
public:
    SystemLog() = default;
    
    SystemLog(LogLevel level, ActionType action_type, std::string message)
        : level_(level)
        , action_type_(action_type)
        , message_(std::move(message)) {
    }
    
    const std::string& id() const { return id_; }
    LogLevel level() const { return level_; }
    ActionType action_type() const { return action_type_; }
    const std::string& message() const { return message_; }
    const std::string& timestamp() const { return timestamp_; }
    
    const std::string& actor_id() const { return actor_id_; }
    const std::string& subject_id() const { return subject_id_; }
    const std::optional<std::string>& ip_address() const { return ip_address_; }
    const std::optional<std::string>& user_agent() const { return user_agent_; }
    
    void set_id(const std::string& id) { id_ = id; }
    void set_level(LogLevel level) { level_ = level; }
    void set_action_type(ActionType action_type) { action_type_ = action_type; }
    void set_message(const std::string& message) { message_ = message; }
    void set_timestamp(const std::string& timestamp) { timestamp_ = timestamp; }
    void set_actor_id(const std::string& actor_id) { actor_id_ = actor_id; }
    void set_subject_id(const std::string& subject_id) { subject_id_ = subject_id; }
    void set_ip_address(const std::optional<std::string>& ip) { ip_address_ = ip; }
    void set_user_agent(const std::optional<std::string>& agent) { user_agent_ = agent; }

    std::string level_string() const {
        return to_string(level_);
    }
    
    std::string action_type_string() const {
        return to_string(action_type_);
    }
    
    std::vector<std::string> get_insert_values() const {
        return {
            id_,
            to_string(level_),
            to_string(action_type_),
            message_,
            actor_id_.empty() ? "NULL" : actor_id_,
            subject_id_.empty() ? "NULL" : subject_id_,
            ip_address_.value_or("NULL"),
            user_agent_.value_or("NULL")
        };
    }

    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        
        std::string level_str = row["level"].as<std::string>();
        level_ = string_to_log_level(level_str);
        
        std::string action_str = row["action_type"].as<std::string>();
        action_type_ = string_to_action_type(action_str);
        
        message_ = row["message"].as<std::string>();
        timestamp_ = row["timestamp"].as<std::string>();
        
        if (!row["actor_id"].is_null()) {
            actor_id_ = row["actor_id"].as<std::string>();
        } else {
            actor_id_.clear();
        }
        
        if (!row["subject_id"].is_null()) {
            subject_id_ = row["subject_id"].as<std::string>();
        } else {
            subject_id_.clear();
        }
        
        if (!row["ip_address"].is_null()) {
            ip_address_ = row["ip_address"].as<std::string>();
        } else {
            ip_address_.reset();
        }
        
        if (!row["user_agent"].is_null()) {
            user_agent_ = row["user_agent"].as<std::string>();
        } else {
            user_agent_.reset();
        }
    }

private:
    std::string id_;
    LogLevel level_;
    ActionType action_type_;
    std::string message_;
    std::string timestamp_;
    std::string actor_id_;
    std::string subject_id_;
    std::optional<std::string> ip_address_;
    std::optional<std::string> user_agent_;
};

}