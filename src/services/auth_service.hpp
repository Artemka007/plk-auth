#pragma once

#include "src/dao/user_dao.hpp"
#include "src/models/user.hpp"
#include "src/models/user_role.hpp"
#include <memory>
#include <string>

namespace services
{
struct LoginResult {
    bool success;
    std::shared_ptr<models::User> user;
    bool password_change_required;
    std::string message;
};

class AuthService {
public:
    explicit AuthService(std::shared_ptr<dao::UserDAO> user_dao);

    LoginResult login(const std::string &email, const std::string &password);

    void update_last_login(const std::shared_ptr<models::User> &user);

private:
    std::shared_ptr<dao::UserDAO> user_dao_;
};

}