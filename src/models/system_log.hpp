#pragma once
#include <string>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>
#include "log_enums.hpp"
#include "user.hpp"
#include <ctime>

namespace models {

#pragma db object table("system_logs")
class SystemLog {
public:
    // Конструкторы
    SystemLog(LogLevel level, ActionType action_type, const std::string& message)
        : level_(level), action_type_(action_type), message_(message),
          timestamp_(std::time(nullptr)) {
    }
    
    SystemLog(LogLevel level, ActionType action_type, const std::string& message,
              const odb::lazy_shared_ptr<User>& subject)
        : level_(level), action_type_(action_type), message_(message),
          subject_(subject), timestamp_(std::time(nullptr)) {
    }
    
    SystemLog(LogLevel level, ActionType action_type, const std::string& message,
              const odb::lazy_shared_ptr<User>& actor,
              const odb::lazy_shared_ptr<User>& subject)
        : level_(level), action_type_(action_type), message_(message),
          actor_(actor), subject_(subject), timestamp_(std::time(nullptr)) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    LogLevel level() const { return level_; }
    ActionType action_type() const { return action_type_; }
    const std::string& message() const { return message_; }
    std::time_t timestamp() const { return timestamp_; }
    
    const odb::nullable<odb::lazy_shared_ptr<User>>& actor() const { return actor_; }
    const odb::nullable<odb::lazy_shared_ptr<User>>& subject() const { return subject_; }
    const odb::nullable<std::string>& ip_address() const { return ip_address_; }
    const odb::nullable<std::string>& user_agent() const { return user_agent_; }
    const odb::nullable<std::string>& additional_data() const { return additional_data_; }
    
    // Сеттеры
    void set_actor(const odb::lazy_shared_ptr<User>& actor) { actor_ = actor; }
    void set_subject(const odb::lazy_shared_ptr<User>& subject) { subject_ = subject; }
    void set_ip_address(const std::string& ip) { ip_address_ = ip; }
    void set_user_agent(const std::string& agent) { user_agent_ = agent; }
    void set_additional_data(const std::string& data) { additional_data_ = data; }
    
    // Вспомогательные методы
    std::string level_string() const {
        return log_level_to_string(level_);
    }
    
    std::string action_type_string() const {
        return action_type_to_string(action_type_);
    }
    
    std::string formatted_timestamp() const {
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&timestamp_));
        return std::string(buffer);
    }
    
    // Factory methods для частых сценариев
    static std::shared_ptr<SystemLog> create_user_creation_log(
        const odb::lazy_shared_ptr<User>& actor,
        const odb::lazy_shared_ptr<User>& subject) {
        
        auto log = std::make_shared<SystemLog>(
            LogLevel::INFO,
            ActionType::USER_CREATED,
            "User account created: " + subject->email(),
            actor,
            subject
        );
        return log;
    }
    
    static std::shared_ptr<SystemLog> create_user_deletion_log(
        const odb::lazy_shared_ptr<User>& actor,
        const odb::lazy_shared_ptr<User>& subject) {
        
        auto log = std::make_shared<SystemLog>(
            LogLevel::WARNING,
            ActionType::USER_DELETED,
            "User account deleted: " + subject->email(),
            actor,
            subject
        );
        return log;
    }
    
    static std::shared_ptr<SystemLog> create_role_change_log(
        const odb::lazy_shared_ptr<User>& actor,
        const odb::lazy_shared_ptr<User>& subject,
        const std::string& role_changes) {
        
        auto log = std::make_shared<SystemLog>(
            LogLevel::INFO,
            ActionType::USER_ROLE_CHANGED,
            "User roles changed: " + subject->email() + " - " + role_changes,
            actor,
            subject
        );
        return log;
    }
    
    static std::shared_ptr<SystemLog> create_admin_login_log(
        const odb::lazy_shared_ptr<User>& actor,
        const std::string& ip_address = "") {
        
        auto log = std::make_shared<SystemLog>(
            LogLevel::INFO,
            ActionType::SYSTEM_LOGIN,
            "Administrator logged in: " + actor->email(),
            actor
        );
        
        if (!ip_address.empty()) {
            log->set_ip_address(ip_address);
        }
        
        return log;
    }
    
    static std::shared_ptr<SystemLog> create_security_violation_log(
        const std::string& message,
        const std::string& ip_address = "") {
        
        auto log = std::make_shared<SystemLog>(
            LogLevel::ERROR,
            ActionType::SECURITY_VIOLATION,
            message
        );
        
        if (!ip_address.empty()) {
            log->set_ip_address(ip_address);
        }
        
        return log;
    }

private:
    friend class odb::access;
    
    #pragma db id type("VARCHAR(36)") default("uuid_generate_v4()")
    std::string id_;
    
    #pragma db not_null
    LogLevel level_;
    
    #pragma db not_null
    ActionType action_type_;
    
    #pragma db type("TEXT") not_null
    std::string message_;
    
    #pragma db type("TIMESTAMP") not_null default("CURRENT_TIMESTAMP")
    std::time_t timestamp_;
    
    // Кто выполнил действие (опционально - для системных событий)
    #pragma db null
    odb::nullable<odb::lazy_shared_ptr<User>> actor_;
    
    // На ком выполнено действие (опционально)
    #pragma db null
    odb::nullable<odb::lazy_shared_ptr<User>> subject_;
    
    // Дополнительная информация
    #pragma db type("VARCHAR(45)") null // IPv6 support
    odb::nullable<std::string> ip_address_;
    
    #pragma db type("TEXT") null
    odb::nullable<std::string> user_agent_;
    
    #pragma db type("JSONB") null // Для хранения дополнительных данных в формате JSON
    odb::nullable<std::string> additional_data_;
};

#pragma db view object(SystemLog)
class SystemLogCount {
public:
    #pragma db column("count(" + SystemLog::id_ + ")")
    std::size_t count;
};

}