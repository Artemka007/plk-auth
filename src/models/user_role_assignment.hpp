#pragma once
#include <string>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include "user.hpp"
#include "user_role.hpp"

namespace models {

#pragma db object table("user_role_assignment")
class UserRoleAssignment {
public:
    UserRoleAssignment() = default;
    
    UserRoleAssignment(
        const std::shared_ptr<User>& user_ptr,
        const std::shared_ptr<UserRole>& role_ptr)
        : user_(user_ptr)
        , role_(role_ptr) {
    }
    
    // Геттеры
    const std::shared_ptr<User>& user() const { return user_; }
    const std::shared_ptr<UserRole>& role() const { return role_; }
    const std::string& assigned_at() const { return assigned_at_; }

private:
    friend class odb::access;
    
    #pragma db id auto
    unsigned long id_;
    
    #pragma db not_null
    #pragma db column("user_id")
    std::shared_ptr<User> user_;
    
    #pragma db not_null
    #pragma db column("role_id")
    std::shared_ptr<UserRole> role_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string assigned_at_;
};

} // namespace models