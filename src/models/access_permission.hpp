#pragma once
#include <string>
#include <vector>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include "user_role.hpp"

namespace models {

#pragma db object table("access_permission")
class AccessPermission {
public:
    AccessPermission() = default;
    
    AccessPermission(std::string name, std::string description)
        : name_(std::move(name))
        , description_(std::move(description)) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }

private:
    friend class odb::access;
    
    #pragma db id type("VARCHAR(36)")
    std::string id_;
    
    #pragma db type("VARCHAR(50)") unique not_null
    std::string name_;
    
    #pragma db type("TEXT")
    std::string description_;
};

#pragma db object table("role_permission")
class RolePermission {
public:
    RolePermission() = default;
    
    RolePermission(
        const std::shared_ptr<UserRole>& role_ptr,
        const std::shared_ptr<AccessPermission>& permission_ptr)
        : role_(role_ptr)
        , permission_(permission_ptr) {
    }
    
    // Геттеры
    const std::shared_ptr<UserRole>& role() const { return role_; }
    const std::shared_ptr<AccessPermission>& permission() const { return permission_; }
    const std::string& granted_at() const { return granted_at_; }

private:
    friend class odb::access;
    
    #pragma db id auto
    unsigned long id_;
    
    #pragma db not_null
    #pragma db column("role_id")
    std::shared_ptr<UserRole> role_;
    
    #pragma db not_null
    #pragma db column("permission_id")
    std::shared_ptr<AccessPermission> permission_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string granted_at_;
};

} // namespace models