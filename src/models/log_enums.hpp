#pragma once
#include <string>
#include <odb/core.hxx>
#include <stdexcept>

namespace models {

#pragma db value
enum class LogLevel {
    DEBUG,
    INFO, 
    WARNING,
    ERROR,
    CRITICAL
};

#pragma db value  
enum class ActionType {
    // Действия с пользователями
    USER_CREATED,
    USER_UPDATED,
    USER_DELETED,
    USER_ROLE_CHANGED,
    USER_PASSWORD_CHANGED,
    USER_STATUS_CHANGED,
    
    ROLE_CREATED,
    ROLE_UPDATED, 
    ROLE_DELETED,
    
    // Системные действия
    SYSTEM_LOGIN,
    SYSTEM_LOGOUT,
    SYSTEM_BACKUP_CREATED,
    SYSTEM_BACKUP_RESTORED,
    SYSTEM_SETTINGS_CHANGED,
    
    // Безопасность
    SECURITY_VIOLATION,
    SECURITY_PASSWORD_RESET,
    SECURITY_ACCESS_DENIED,
    
    // Профиль
    PROFILE_UPDATED,
    PROFILE_VIEWED
};

std::string log_level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

LogLevel log_level_from_string(const std::string& str) {
    if (str == "DEBUG") return LogLevel::DEBUG;
    if (str == "INFO") return LogLevel::INFO;
    if (str == "WARNING") return LogLevel::WARNING;
    if (str == "ERROR") return LogLevel::ERROR;
    if (str == "CRITICAL") return LogLevel::CRITICAL;
    throw std::invalid_argument("Unknown log level: " + str);
}

std::string action_type_to_string(ActionType action) {
    switch (action) {
        // User actions
        case ActionType::USER_CREATED: return "USER_CREATED";
        case ActionType::USER_UPDATED: return "USER_UPDATED";
        case ActionType::USER_DELETED: return "USER_DELETED";
        case ActionType::USER_ROLE_CHANGED: return "USER_ROLE_CHANGED";
        case ActionType::USER_PASSWORD_CHANGED: return "USER_PASSWORD_CHANGED";
        case ActionType::USER_STATUS_CHANGED: return "USER_STATUS_CHANGED";
        
        // Role actions
        case ActionType::ROLE_CREATED: return "ROLE_CREATED";
        case ActionType::ROLE_UPDATED: return "ROLE_UPDATED";
        case ActionType::ROLE_DELETED: return "ROLE_DELETED";
        
        // System actions
        case ActionType::SYSTEM_LOGIN: return "SYSTEM_LOGIN";
        case ActionType::SYSTEM_LOGOUT: return "SYSTEM_LOGOUT";
        case ActionType::SYSTEM_BACKUP_CREATED: return "SYSTEM_BACKUP_CREATED";
        case ActionType::SYSTEM_BACKUP_RESTORED: return "SYSTEM_BACKUP_RESTORED";
        case ActionType::SYSTEM_SETTINGS_CHANGED: return "SYSTEM_SETTINGS_CHANGED";
        
        // Security actions
        case ActionType::SECURITY_VIOLATION: return "SECURITY_VIOLATION";
        case ActionType::SECURITY_PASSWORD_RESET: return "SECURITY_PASSWORD_RESET";
        case ActionType::SECURITY_ACCESS_DENIED: return "SECURITY_ACCESS_DENIED";
        
        // Profile actions
        case ActionType::PROFILE_UPDATED: return "PROFILE_UPDATED";
        case ActionType::PROFILE_VIEWED: return "PROFILE_VIEWED";
        
        default: return "UNKNOWN_ACTION";
    }
}

ActionType action_type_from_string(const std::string& str) {
    // User actions
    if (str == "USER_CREATED") return ActionType::USER_CREATED;
    if (str == "USER_UPDATED") return ActionType::USER_UPDATED;
    if (str == "USER_DELETED") return ActionType::USER_DELETED;
    if (str == "USER_ROLE_CHANGED") return ActionType::USER_ROLE_CHANGED;
    if (str == "USER_PASSWORD_CHANGED") return ActionType::USER_PASSWORD_CHANGED;
    if (str == "USER_STATUS_CHANGED") return ActionType::USER_STATUS_CHANGED;
    
    // Role actions
    if (str == "ROLE_CREATED") return ActionType::ROLE_CREATED;
    if (str == "ROLE_UPDATED") return ActionType::ROLE_UPDATED;
    if (str == "ROLE_DELETED") return ActionType::ROLE_DELETED;
    
    // System actions
    if (str == "SYSTEM_LOGIN") return ActionType::SYSTEM_LOGIN;
    if (str == "SYSTEM_LOGOUT") return ActionType::SYSTEM_LOGOUT;
    if (str == "SYSTEM_BACKUP_CREATED") return ActionType::SYSTEM_BACKUP_CREATED;
    if (str == "SYSTEM_BACKUP_RESTORED") return ActionType::SYSTEM_BACKUP_RESTORED;
    if (str == "SYSTEM_SETTINGS_CHANGED") return ActionType::SYSTEM_SETTINGS_CHANGED;
    
    // Security actions
    if (str == "SECURITY_VIOLATION") return ActionType::SECURITY_VIOLATION;
    if (str == "SECURITY_PASSWORD_RESET") return ActionType::SECURITY_PASSWORD_RESET;
    if (str == "SECURITY_ACCESS_DENIED") return ActionType::SECURITY_ACCESS_DENIED;
    
    // Profile actions
    if (str == "PROFILE_UPDATED") return ActionType::PROFILE_UPDATED;
    if (str == "PROFILE_VIEWED") return ActionType::PROFILE_VIEWED;
    
    throw std::invalid_argument("Unknown action type: " + str);
}

} 