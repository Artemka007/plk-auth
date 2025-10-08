#pragma once
#include <string>
#include <vector>
#include <pqxx/pqxx>

namespace models {

class AccessPermission {
public:
    AccessPermission() = default;
    
    AccessPermission(std::string name, std::string description)
        : name_(std::move(name))
        , description_(std::move(description)) {
    }
    
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }

    void set_id(const std::string& id) { id_ = id; }
    void set_name(const std::string& name) { name_ = name; }
    void set_description(const std::string& description) { description_ = description; }

    std::vector<std::string> get_insert_values() const {
        return {id_, name_, description_};
    }

    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        name_ = row["name"].as<std::string>();
        description_ = row["description"].as<std::string>();
    }

private:
    std::string id_;
    std::string name_;
    std::string description_;
};

class RolePermission {
public:
    RolePermission() = default;
    
    RolePermission(
        const std::string& role_id,
        const std::string& permission_id)
        : role_id_(role_id)
        , permission_id_(permission_id) {
    }
    
    const std::string& id() const { return id_; }
    const std::string& role_id() const { return role_id_; }
    const std::string& permission_id() const { return permission_id_; }
    const std::string& granted_at() const { return granted_at_; }

    void set_role_id(const std::string& role_id) { role_id_ = role_id; }
    void set_permission_id(const std::string& permission_id) { permission_id_ = permission_id; }

    std::vector<std::string> get_insert_values() const {
        return {role_id_, permission_id_};
    }

    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        role_id_ = row["role_id"].as<std::string>();
        permission_id_ = row["permission_id"].as<std::string>();
        granted_at_ = row["granted_at"].as<std::string>();
    }

private:
    std::string id_;
    std::string role_id_;
    std::string permission_id_;
    std::string granted_at_;
};

struct SystemPermissions {
    static constexpr const char* USER_CREATE = "USER_CREATE";
    static constexpr const char* USER_READ = "USER_READ";
    static constexpr const char* USER_UPDATE = "USER_UPDATE";
    static constexpr const char* USER_DELETE = "USER_DELETE";
    static constexpr const char* USER_CHANGE_ROLE = "USER_CHANGE_ROLE";
    
    static constexpr const char* ROLE_CREATE = "ROLE_CREATE";
    static constexpr const char* ROLE_READ = "ROLE_READ";
    static constexpr const char* ROLE_UPDATE = "ROLE_UPDATE";
    static constexpr const char* ROLE_DELETE = "ROLE_DELETE";
    
    static constexpr const char* LOG_READ = "LOG_READ";
    static constexpr const char* LOG_EXPORT = "LOG_EXPORT";
    static constexpr const char* LOG_DELETE = "LOG_DELETE";
    
    static constexpr const char* SYSTEM_BACKUP = "SYSTEM_BACKUP";
    static constexpr const char* SYSTEM_RESTORE = "SYSTEM_RESTORE";
    static constexpr const char* SYSTEM_EXPORT = "SYSTEM_EXPORT";
    static constexpr const char* SYSTEM_IMPORT = "SYSTEM_IMPORT";
    
    static std::vector<std::string> getAllPermissions() {
        return {
            USER_CREATE, USER_READ, USER_UPDATE, USER_DELETE, USER_CHANGE_ROLE,
            ROLE_CREATE, ROLE_READ, ROLE_UPDATE, ROLE_DELETE,
            LOG_READ, LOG_EXPORT, LOG_DELETE,
            SYSTEM_BACKUP, SYSTEM_RESTORE, SYSTEM_EXPORT, SYSTEM_IMPORT
        };
    }
};

} 