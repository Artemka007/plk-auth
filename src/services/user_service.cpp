#include "services/UserService.hpp"
#include "user_dao.hpp"

UserService::UserService(std::shared_ptr<dao::UserDao> user_dao) 
    : user_dao_(std::move(user_dao)) {}

std::optional<models::User> UserService::find_by_email(const std::string& email) {
    auto user = user_dao_->find_by_email(email);
    if (user) {
        return *user;
    }
    return std::nullopt;
}

std::vector<models::User> UserService::get_all_users() {
    auto users = user_dao_->find_all();
    std::vector<models::User> result;
    for (const auto& user : users) {
        result.push_back(*user);
    }
    return result;
}

bool UserService::create_user(const std::string& first_name, const std::string& last_name, const std::string& email) {
    if (user_dao_->find_by_email(email) != nullptr) {
        return false;
    }
    
    auto new_user = std::make_shared<models::User>(first_name, last_name, email);
    return user_dao_->save(new_user);
}

bool UserService::delete_user(const std::string& email) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return false;
    }
    return user_dao_->remove(user->id);
}

bool UserService::add_role_to_user(const std::string& email, const models::UserRole role) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return false;
    }
    
    return user_dao_->assign_role(user, role);
}

bool UserService::remove_role_from_user(const std::string& email, const models::UserRole role) {
    auto user_ptr = user_dao_->find_by_email(email);
    if (!user_ptr) {
        return false;
    }

    auto role_ptr = std::make_shared<models::UserRole>(role);
    return user_dao_->remove_role(user_ptr, role_ptr);
}

bool UserService::is_admin(const std::shared_ptr<models::User>& user) const {
    if (!user) {
        return false;
    }
    
    return user_dao_->has_role(user, "admin");
}

bool UserService::can_manage_users(const std::shared_ptr<models::User>& user) const {
   if (!user) {
        return false;
    }
    
    return user_dao_->has_role(user, "admin") || 
           user_dao_->has_role(user, "user_manager");
}

bool UserService::has_role(const std::shared_ptr<models::User>& user, const std::string &role_name) const {
    if (!user || role_name.empty()) {
        return false;
    }
    
    return user_dao_->has_role(user, role_name);
}

bool UserService::requires_password_change(const std::shared_ptr<models::User> &user) const {
    if (!user) {
        return false;
    }
    return user->password_change_required();
}

bool UserService::is_user_active(const std::shared_ptr<models::User> &user) const {
if (!user) {
        return false;
    }
    
    return user->is_active();
}

std::vector<models::UserRole> UserService::user_roles(const std::string& email) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return {};
    }

    auto role_ptrs = user_dao_->user_roles(user);
    std::vector<models::UserRole> roles;

    for (const auto& role_ptr : role_ptrs) {
        if (role_ptr) {
            roles.push_back(*role_ptr);
        }
    }

    return roles;
}