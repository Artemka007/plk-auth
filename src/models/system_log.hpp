#pragma once
#include <string>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>
#include "enums.hpp"
#include "user.hpp"

namespace models {

#pragma db object table("system_log")
class SystemLog {
public:
    SystemLog() = default;
    
    SystemLog(LogLevel level, ActionType action_type, std::string message)
        : level_(level)
        , action_type_(action_type)
        , message_(std::move(message)) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    LogLevel level() const { return level_; }
    ActionType action_type() const { return action_type_; }
    const std::string& message() const { return message_; }
    const std::string& timestamp() const { return timestamp_; }
    
    const odb::nullable<odb::lazy_shared_ptr<User>>& actor() const { return actor_; }
    const odb::nullable<odb::lazy_shared_ptr<User>>& subject() const { return subject_; }
    const odb::nullable<std::string>& ip_address() const { return ip_address_; }
    const odb::nullable<std::string>& user_agent() const { return user_agent_; }
    
    // Сеттеры
    void set_actor(const odb::lazy_shared_ptr<User>& actor) { actor_ = actor; }
    void set_subject(const odb::lazy_shared_ptr<User>& subject) { subject_ = subject; }
    void set_ip_address(const std::string& ip) { ip_address_ = ip; }
    void set_user_agent(const std::string& agent) { user_agent_ = agent; }

private:
    friend class odb::access;
    
    #pragma db id type("VARCHAR(36)")
    std::string id_;
    
    #pragma db not_null
    LogLevel level_;
    
    #pragma db not_null
    ActionType action_type_;
    
    #pragma db type("TEXT") not_null
    std::string message_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string timestamp_;
    
    #pragma db null
    #pragma db type("VARCHAR(36)")
    #pragma db column("actor_id")
    odb::nullable<odb::lazy_shared_ptr<User>> actor_;
    
    #pragma db null
    #pragma db type("VARCHAR(36)")
    #pragma db column("subject_id")
    odb::nullable<odb::lazy_shared_ptr<User>> subject_;
    
    #pragma db type("VARCHAR(45)") null
    odb::nullable<std::string> ip_address_;
    
    #pragma db type("TEXT") null
    odb::nullable<std::string> user_agent_;
};

}