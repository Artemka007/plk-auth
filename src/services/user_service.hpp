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
    bool add_role_to_user(const std::string& email, const std::string& role);
    bool remove_role_from_user(const std::string& email, const std::string& role);

private:
    std::shared_ptr<UserDao> user_dao_;