#pragma once

#include "models/user.h"
#include <memory>
#include <vector>
#include <optional>

class UserDao;

class UserService {
public:
    explicit UserService(std::shared_ptr<UserDao> user_dao);

    std::optional<models::User> find_by_email(const std::string& email);
    std::vector<models::User> get_all_users();
    bool create_user(const std::string& first_name, const std::string& last_name, const std::string& email);
    bool delete_user(const std::string& email);
    bool add_role_to_user(const std::string& email, const models::UserRole role);
    bool remove_role_from_user(const std::string& email, const models::UserRole role);
    bool is_admin(const std::shared_ptr<models::User>& user) const;
    bool can_manage_users(const std::shared_ptr<models::User>& user) const;
    bool has_role(const std::shared_ptr<models::User>& user, const std::string &role_name) const;
    bool requires_password_change(const std::shared_ptr<models::User> &user) const;
    bool is_user_active(const std::shared_ptr<models::User> &user) const;

private:
    std::shared_ptr<UserDao> user_dao_;