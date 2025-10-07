#pragma once

#include "models/user.hpp"
#include "models/user_role.hpp"
#include <memory>
#include <string>

struct LoginResult {
    bool success;
    std::shared_ptr<models::User> user;
    bool password_change_required;
    std::string message;
};

namespace dao {
    class UserDAO;
} // namespace dao

class AuthService {
public:
    explicit AuthService(std::shared_ptr<dao::UserDAO> user_dao);

    LoginResult login(const std::string &email, const std::string &password);

    void update_last_login(const std::shared_ptr<models::User> &user);

private:
    std::shared_ptr<dao::UserDAO> user_dao_;
};
