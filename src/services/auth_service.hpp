#pragma once

#include "models/user.hpp"
#include "models/user_role.hpp"
#include <memory>

struct LoginResult {
    std::shared_ptr<models::User> user;
    bool password_change_required;
    std::string message;
}

namespace dao {
    class UserDAO;
    class UserRole;
} // namespace dao

class AuthService {
public:
    explicit AuthService(std::shared_ptr<UserDAO> user_dao);

    LoginResult login(const std::string &email,  const std::string &password);

    // Login attempts management
    bool is_login_attempts_exceeded(const std::string &email) const;
    void reset_login_attempts(const std::string &email);
    void increment_login_attempts(const std::string &email);

    // Updates
    void update_last_login(const std::shared_ptr<models::User> &user);

private:
    std::shared_ptr<UserDAO> user_dao_;
};
