#pragma once
#include <string>
#include <unordered_map>

namespace models {

enum class LogLevel {
    DEBUG,
    INFO, 
    WARNING,
    ERROR,
    CRITICAL
};

enum class ActionType {
    USER_CREATED,
    USER_UPDATED,
    USER_DELETED,
    USER_ROLE_CHANGED,
    USER_PASSWORD_CHANGED,
    USER_STATUS_CHANGED,
    ROLE_CREATED,
    ROLE_UPDATED, 
    ROLE_DELETED,
    SYSTEM_LOGIN,
    SYSTEM_LOGOUT,
    SYSTEM_BACKUP_CREATED,
    SYSTEM_BACKUP_RESTORED,
    SYSTEM_SETTINGS_CHANGED,
    SECURITY_VIOLATION,
    SECURITY_PASSWORD_RESET,
    SECURITY_ACCESS_DENIED,
    PROFILE_UPDATED,
    PROFILE_VIEWED,
    SYSTEM_IMPORT,
    SYSTEM_EXPORT,
    SYSTEM_STARTUP
};

enum class AccessPermissionType {
    USER_CREATE,
    USER_READ,
    USER_UPDATE, 
    USER_DELETE,
    USER_CHANGE_ROLE,
    ROLE_CREATE,
    ROLE_UPDATE,
    ROLE_DELETE,
    SYSTEM_IMPORT,
    SYSTEM_EXPORT,
    SYSTEM_VIEW_LOGS,
    SYSTEM_MANAGE_SETTINGS,
    PROFILE_READ,
    PROFILE_UPDATE,
    PASSWORD_CHANGE
};

inline std::string to_string(LogLevel level) {
    static const std::unordered_map<LogLevel, std::string> names = {
        {LogLevel::DEBUG, "DEBUG"},
        {LogLevel::INFO, "INFO"},
        {LogLevel::WARNING, "WARNING"},
        {LogLevel::ERROR, "ERROR"},
        {LogLevel::CRITICAL, "CRITICAL"}
    };
    return names.at(level);
}

inline std::string to_string(ActionType action) {
    static const std::unordered_map<ActionType, std::string> names = {
        {ActionType::USER_CREATED, "USER_CREATED"},
        {ActionType::USER_UPDATED, "USER_UPDATED"},
        {ActionType::USER_DELETED, "USER_DELETED"},
        {ActionType::USER_ROLE_CHANGED, "USER_ROLE_CHANGED"},
        {ActionType::USER_PASSWORD_CHANGED, "USER_PASSWORD_CHANGED"},
        {ActionType::USER_STATUS_CHANGED, "USER_STATUS_CHANGED"},
        {ActionType::ROLE_CREATED, "ROLE_CREATED"},
        {ActionType::ROLE_UPDATED, "ROLE_UPDATED"},
        {ActionType::ROLE_DELETED, "ROLE_DELETED"},
        {ActionType::SYSTEM_LOGIN, "SYSTEM_LOGIN"},
        {ActionType::SYSTEM_LOGOUT, "SYSTEM_LOGOUT"},
        {ActionType::SYSTEM_BACKUP_CREATED, "SYSTEM_BACKUP_CREATED"},
        {ActionType::SYSTEM_BACKUP_RESTORED, "SYSTEM_BACKUP_RESTORED"},
        {ActionType::SYSTEM_SETTINGS_CHANGED, "SYSTEM_SETTINGS_CHANGED"},
        {ActionType::SECURITY_VIOLATION, "SECURITY_VIOLATION"},
        {ActionType::SECURITY_PASSWORD_RESET, "SECURITY_PASSWORD_RESET"},
        {ActionType::SECURITY_ACCESS_DENIED, "SECURITY_ACCESS_DENIED"},
        {ActionType::PROFILE_UPDATED, "PROFILE_UPDATED"},
        {ActionType::PROFILE_VIEWED, "PROFILE_VIEWED"},
        {ActionType::SYSTEM_IMPORT, "SYSTEM_IMPORT"},
        {ActionType::SYSTEM_EXPORT, "SYSTEM_EXPORT"},
        {ActionType::SYSTEM_STARTUP, "SYSTEM_STARTUP"}
    };
    return names.at(action);
}

inline std::string to_string(AccessPermissionType permission) {
    static const std::unordered_map<AccessPermissionType, std::string> names = {
        {AccessPermissionType::USER_CREATE, "USER_CREATE"},
        {AccessPermissionType::USER_READ, "USER_READ"},
        {AccessPermissionType::USER_UPDATE, "USER_UPDATE"},
        {AccessPermissionType::USER_DELETE, "USER_DELETE"},
        {AccessPermissionType::USER_CHANGE_ROLE, "USER_CHANGE_ROLE"},
        {AccessPermissionType::ROLE_CREATE, "ROLE_CREATE"},
        {AccessPermissionType::ROLE_UPDATE, "ROLE_UPDATE"},
        {AccessPermissionType::ROLE_DELETE, "ROLE_DELETE"},
        {AccessPermissionType::SYSTEM_IMPORT, "SYSTEM_IMPORT"},
        {AccessPermissionType::SYSTEM_EXPORT, "SYSTEM_EXPORT"},
        {AccessPermissionType::SYSTEM_VIEW_LOGS, "SYSTEM_VIEW_LOGS"},
        {AccessPermissionType::SYSTEM_MANAGE_SETTINGS, "SYSTEM_MANAGE_SETTINGS"},
        {AccessPermissionType::PROFILE_READ, "PROFILE_READ"},
        {AccessPermissionType::PROFILE_UPDATE, "PROFILE_UPDATE"},
        {AccessPermissionType::PASSWORD_CHANGE, "PASSWORD_CHANGE"}
    };
    return names.at(permission);
}

inline LogLevel string_to_log_level(const std::string& level_str) {
    static const std::unordered_map<std::string, LogLevel> level_map = {
        {"DEBUG", LogLevel::DEBUG},
        {"INFO", LogLevel::INFO},
        {"WARNING", LogLevel::WARNING},
        {"ERROR", LogLevel::ERROR},
        {"CRITICAL", LogLevel::CRITICAL}
    };
    
    auto it = level_map.find(level_str);
    if (it != level_map.end()) {
        return it->second;
    }
    return LogLevel::INFO;
}

inline ActionType string_to_action_type(const std::string& action_str) {
    static const std::unordered_map<std::string, ActionType> action_map = {
        {"USER_CREATED", ActionType::USER_CREATED},
        {"USER_UPDATED", ActionType::USER_UPDATED},
        {"USER_DELETED", ActionType::USER_DELETED},
        {"USER_ROLE_CHANGED", ActionType::USER_ROLE_CHANGED},
        {"USER_PASSWORD_CHANGED", ActionType::USER_PASSWORD_CHANGED},
        {"USER_STATUS_CHANGED", ActionType::USER_STATUS_CHANGED},
        {"ROLE_CREATED", ActionType::ROLE_CREATED},
        {"ROLE_UPDATED", ActionType::ROLE_UPDATED},
        {"ROLE_DELETED", ActionType::ROLE_DELETED},
        {"SYSTEM_LOGIN", ActionType::SYSTEM_LOGIN},
        {"SYSTEM_LOGOUT", ActionType::SYSTEM_LOGOUT},
        {"SYSTEM_BACKUP_CREATED", ActionType::SYSTEM_BACKUP_CREATED},
        {"SYSTEM_BACKUP_RESTORED", ActionType::SYSTEM_BACKUP_RESTORED},
        {"SYSTEM_SETTINGS_CHANGED", ActionType::SYSTEM_SETTINGS_CHANGED},
        {"SECURITY_VIOLATION", ActionType::SECURITY_VIOLATION},
        {"SECURITY_PASSWORD_RESET", ActionType::SECURITY_PASSWORD_RESET},
        {"SECURITY_ACCESS_DENIED", ActionType::SECURITY_ACCESS_DENIED},
        {"PROFILE_UPDATED", ActionType::PROFILE_UPDATED},
        {"PROFILE_VIEWED", ActionType::PROFILE_VIEWED},
        {"SYSTEM_IMPORT", ActionType::SYSTEM_IMPORT},
        {"SYSTEM_EXPORT", ActionType::SYSTEM_EXPORT},
        {"SYSTEM_STARTUP", ActionType::SYSTEM_STARTUP}
    };
    
    auto it = action_map.find(action_str);
    if (it != action_map.end()) {
        return it->second;
    }
    return ActionType::SYSTEM_LOGIN; 
}



inline std::optional<LogLevel> string_to_log_level_optional(const std::string& level_str) {
    static const std::unordered_map<std::string, LogLevel> level_map = {
        {"DEBUG", LogLevel::DEBUG},
        {"INFO", LogLevel::INFO},
        {"WARNING", LogLevel::WARNING},
        {"ERROR", LogLevel::ERROR},
        {"CRITICAL", LogLevel::CRITICAL}
    };
    
    auto it = level_map.find(level_str);
    if (it != level_map.end()) {
        return it->second;
    }
    return std::nullopt;
}

inline std::optional<ActionType> string_to_action_type_optional(const std::string& action_str) {
    static const std::unordered_map<std::string, ActionType> action_map = {
        {"USER_CREATED", ActionType::USER_CREATED},
        {"USER_UPDATED", ActionType::USER_UPDATED},
        {"USER_DELETED", ActionType::USER_DELETED},
        {"USER_ROLE_CHANGED", ActionType::USER_ROLE_CHANGED},
        {"USER_PASSWORD_CHANGED", ActionType::USER_PASSWORD_CHANGED},
        {"USER_STATUS_CHANGED", ActionType::USER_STATUS_CHANGED},
        {"ROLE_CREATED", ActionType::ROLE_CREATED},
        {"ROLE_UPDATED", ActionType::ROLE_UPDATED},
        {"ROLE_DELETED", ActionType::ROLE_DELETED},
        {"SYSTEM_LOGIN", ActionType::SYSTEM_LOGIN},
        {"SYSTEM_LOGOUT", ActionType::SYSTEM_LOGOUT},
        {"SYSTEM_BACKUP_CREATED", ActionType::SYSTEM_BACKUP_CREATED},
        {"SYSTEM_BACKUP_RESTORED", ActionType::SYSTEM_BACKUP_RESTORED},
        {"SYSTEM_SETTINGS_CHANGED", ActionType::SYSTEM_SETTINGS_CHANGED},
        {"SECURITY_VIOLATION", ActionType::SECURITY_VIOLATION},
        {"SECURITY_PASSWORD_RESET", ActionType::SECURITY_PASSWORD_RESET},
        {"SECURITY_ACCESS_DENIED", ActionType::SECURITY_ACCESS_DENIED},
        {"PROFILE_UPDATED", ActionType::PROFILE_UPDATED},
        {"PROFILE_VIEWED", ActionType::PROFILE_VIEWED},
        {"SYSTEM_IMPORT", ActionType::SYSTEM_IMPORT},
        {"SYSTEM_EXPORT", ActionType::SYSTEM_EXPORT},
        {"SYSTEM_STARTUP", ActionType::SYSTEM_STARTUP}
    };
    
    auto it = action_map.find(action_str);
    if (it != action_map.end()) {
        return it->second;
    }
    return std::nullopt; 
}


}