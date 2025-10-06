#pragma once
#include <string>
#include <odb/core.hxx>
#include <stdexcept>

#pragma db model version(1, 1)

namespace models {

#pragma db value
class AccessPermission {
public:
    enum class Type {
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

    AccessPermission() = default;
    AccessPermission(Type type) : type_(type) {}
    
    #pragma db id auto
    Type type() const { return type_; }
    
    std::string to_string() const {
        switch (type_) {
            case Type::USER_CREATE: return "USER_CREATE";
            case Type::USER_READ: return "USER_READ";
            case Type::USER_UPDATE: return "USER_UPDATE";
            case Type::USER_DELETE: return "USER_DELETE";
            case Type::USER_CHANGE_ROLE: return "USER_CHANGE_ROLE";
            case Type::ROLE_CREATE: return "ROLE_CREATE";
            case Type::ROLE_UPDATE: return "ROLE_UPDATE";
            case Type::ROLE_DELETE: return "ROLE_DELETE";
            case Type::SYSTEM_IMPORT: return "SYSTEM_IMPORT";
            case Type::SYSTEM_EXPORT: return "SYSTEM_EXPORT";
            case Type::SYSTEM_VIEW_LOGS: return "SYSTEM_VIEW_LOGS";
            case Type::SYSTEM_MANAGE_SETTINGS: return "SYSTEM_MANAGE_SETTINGS";
            case Type::PROFILE_READ: return "PROFILE_READ";
            case Type::PROFILE_UPDATE: return "PROFILE_UPDATE";
            case Type::PASSWORD_CHANGE: return "PASSWORD_CHANGE";
            default: return "UNKNOWN";
        }
    }
    
    static Type from_string(const std::string& str) {
        if (str == "USER_CREATE") return Type::USER_CREATE;
        if (str == "USER_READ") return Type::USER_READ;
        if (str == "USER_UPDATE") return Type::USER_UPDATE;
        if (str == "USER_DELETE") return Type::USER_DELETE;
        if (str == "USER_CHANGE_ROLE") return Type::USER_CHANGE_ROLE;
        if (str == "ROLE_CREATE") return Type::ROLE_CREATE;
        if (str == "ROLE_UPDATE") return Type::ROLE_UPDATE;
        if (str == "ROLE_DELETE") return Type::ROLE_DELETE;
        if (str == "SYSTEM_IMPORT") return Type::SYSTEM_IMPORT;
        if (str == "SYSTEM_EXPORT") return Type::SYSTEM_EXPORT;
        if (str == "SYSTEM_VIEW_LOGS") return Type::SYSTEM_VIEW_LOGS;
        if (str == "SYSTEM_MANAGE_SETTINGS") return Type::SYSTEM_MANAGE_SETTINGS;
        if (str == "PROFILE_READ") return Type::PROFILE_READ;
        if (str == "PROFILE_UPDATE") return Type::PROFILE_UPDATE;
        if (str == "PASSWORD_CHANGE") return Type::PASSWORD_CHANGE;
        throw std::invalid_argument("Unknown permission: " + str);
    }

private:
    friend class odb::access;
    
    #pragma db id
    Type type_;
};

} // namespace models