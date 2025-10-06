#pragma once
#include <string>
#include <vector>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>
#include "user_role.hpp"

namespace models {

#pragma db object table("users")
class User {
public:
    User(const std::string& first_name, const std::string& last_name, const std::string& email)
        : first_name_(first_name), last_name_(last_name), email_(email),
          is_active_(true), password_change_required_(true) {
    }
    
    const std::string& id() const { return id_; }
    const std::string& first_name() const { return first_name_; }
    const std::string& last_name() const { return last_name_; }
    const odb::nullable<std::string>& patronymic() const { return patronymic_; }
    const std::string& email() const { return email_; }
    const odb::nullable<std::string>& phone() const { return phone_; }
    const std::string& password_hash() const { return password_hash_; }
    bool is_active() const { return is_active_; }
    bool password_change_required() const { return password_change_required_; }
    
    std::string full_name() const {
        if (patronymic_.null()) {
            return first_name_ + " " + last_name_;
        }
        return first_name_ + " " + patronymic_.get() + " " + last_name_;
    }
    
    void set_first_name(const std::string& name) { first_name_ = name; }
    void set_last_name(const std::string& name) { last_name_ = name; }
    void set_patronymic(const std::string& name) { patronymic_ = name; }
    void set_email(const std::string& email) { email_ = email; }
    void set_phone(const std::string& phone) { phone_ = phone; }
    void set_password_hash(const std::string& hash) { 
        password_hash_ = hash; 
        password_change_required_ = false;
    }
    void set_active(bool active) { is_active_ = active; }
    void require_password_change() { password_change_required_ = true; }
    
    void add_role(const odb::lazy_shared_ptr<UserRole>& role) {
        roles_.push_back(role);
    }
    
    void remove_role(const odb::lazy_shared_ptr<UserRole>& role) {
        roles_.erase(
            std::remove(roles_.begin(), roles_.end(), role),
            roles_.end()
        );
    }
    
    bool has_role(const std::string& role_name) const {
        for (const auto& role : roles_) {
            if (role->name() == role_name) return true;
        }
        return false;
    }
    
    const std::vector<odb::lazy_shared_ptr<UserRole>>& roles() const {
        return roles_;
    }
    
    bool has_permission(AccessPermission::Type permission) const {
        for (const auto& role : roles_) {
            if (role->has_permission(permission)) return true;
        }
        return false;
    }
    
    bool can_manage_users() const {
        return has_permission(AccessPermission::Type::USER_CREATE) ||
               has_permission(AccessPermission::Type::USER_UPDATE) ||
               has_permission(AccessPermission::Type::USER_DELETE);
    }
    
    bool is_valid() const {
        return !first_name_.empty() && !last_name_.empty() && !email_.empty();
    }

private:
    friend class odb::access;
    
    #pragma db id type("VARCHAR(36)") default("uuid_generate_v4()")
    std::string id_;
    
    #pragma db type("VARCHAR(100)") not_null
    std::string first_name_;
    
    #pragma db type("VARCHAR(100)") not_null
    std::string last_name_;
    
    #pragma db type("VARCHAR(100)")
    odb::nullable<std::string> patronymic_;
    
    #pragma db type("VARCHAR(255)") unique not_null
    std::string email_;
    
    #pragma db type("VARCHAR(20)")
    odb::nullable<std::string> phone_;
    
    #pragma db type("VARCHAR(255)") not_null
    std::string password_hash_;
    
    #pragma db value_not_null
    #pragma db unordered
    #pragma db table("user_role_assignments")
    #pragma db id column("user_id")
    #pragma db inverse(user_)
    std::vector<odb::lazy_shared_ptr<UserRole>> roles_;
    
    #pragma db not_null
    bool is_active_;
    
    #pragma db not_null
    bool password_change_required_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string created_at_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string updated_at_;
    
    #pragma db type("TIMESTAMP")
    odb::nullable<std::string> last_login_at_;
};

#pragma db view object(User)
class UserCount {
public:
    #pragma db column("count(" + User::id_ + ")")
    std::size_t count;
};

} 