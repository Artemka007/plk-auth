#pragma once
#include <string>
#include <vector>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include "access_permission.hpp"

namespace models {

#pragma db object table("user_roles")
class UserRole {
public:
    UserRole(const std::string& name, const std::string& description)
        : name_(name), description_(description), is_system_(false) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    bool is_system() const { return is_system_; }
    
    // Управление правами
    void add_permission(AccessPermission::Type permission) {
        permissions_.push_back(std::make_shared<AccessPermission>(permission));
    }
    
    void remove_permission(AccessPermission::Type permission) {
        if (!is_system_) {
            permissions_.erase(
                std::remove_if(permissions_.begin(), permissions_.end(),
                    [permission](const odb::lazy_shared_ptr<AccessPermission>& p) {
                        return p->type() == permission;
                    }),
                permissions_.end()
            );
        }
    }
    
    bool has_permission(AccessPermission::Type permission) const {
        for (const auto& perm : permissions_) {
            if (perm->type() == permission) return true;
        }
        return false;
    }
    
    const std::vector<odb::lazy_shared_ptr<AccessPermission>>& permissions() const {
        return permissions_;
    }
    
    // Системные роли
    static std::shared_ptr<UserRole> create_super_admin_role() {
        auto role = std::make_shared<UserRole>("SUPER_ADMIN", "Full system access");
        
        for (int i = static_cast<int>(AccessPermission::Type::USER_CREATE);
             i <= static_cast<int>(AccessPermission::Type::PASSWORD_CHANGE); ++i) {
            role->add_permission(static_cast<AccessPermission::Type>(i));
        }
        
        role->is_system_ = true;
        return role;
    }
    
    static std::shared_ptr<UserRole> create_admin_role() {
        auto role = std::make_shared<UserRole>("ADMIN", "Administrator with user management rights");
        
        role->add_permission(AccessPermission::Type::USER_CREATE);
        role->add_permission(AccessPermission::Type::USER_READ);
        role->add_permission(AccessPermission::Type::USER_UPDATE);
        role->add_permission(AccessPermission::Type::USER_DELETE);
        role->add_permission(AccessPermission::Type::USER_CHANGE_ROLE);
        role->add_permission(AccessPermission::Type::SYSTEM_IMPORT);
        role->add_permission(AccessPermission::Type::SYSTEM_EXPORT);
        role->add_permission(AccessPermission::Type::SYSTEM_VIEW_LOGS);
        role->add_permission(AccessPermission::Type::PROFILE_READ);
        role->add_permission(AccessPermission::Type::PROFILE_UPDATE);
        role->add_permission(AccessPermission::Type::PASSWORD_CHANGE);
        
        return role;
    }
    
    static std::shared_ptr<UserRole> create_user_role() {
        auto role = std::make_shared<UserRole>("USER", "Regular user");
        
        role->add_permission(AccessPermission::Type::PROFILE_READ);
        role->add_permission(AccessPermission::Type::PROFILE_UPDATE);
        role->add_permission(AccessPermission::Type::PASSWORD_CHANGE);
        
        return role;
    }

private:
    friend class odb::access;
    
    #pragma db id type("VARCHAR(36)") default("uuid_generate_v4()")
    std::string id_;
    
    #pragma db type("VARCHAR(50)") unique not_null
    std::string name_;
    
    #pragma db type("TEXT")
    std::string description_;
    
    #pragma db value_not_null
    #pragma db unordered
    #pragma db table("role_permissions")
    std::vector<odb::lazy_shared_ptr<AccessPermission>> permissions_;
    
    #pragma db not_null
    bool is_system_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string created_at_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string updated_at_;
};

#pragma db view object(UserRole)
class UserRoleCount {
public:
    #pragma db column("count(" + UserRole::id_ + ")")
    std::size_t count;
};

}