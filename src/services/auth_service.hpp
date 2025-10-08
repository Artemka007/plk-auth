#pragma once

#include <memory>
#include <string>
#include "src/dao/user_dao.hpp"
#include "src/models/user.hpp"
#include "log_service.hpp"

namespace services {

struct LoginResult {
    bool success;
    std::shared_ptr<models::User> user;
    bool password_change_required;
    std::string message;
};

class AuthService {
public:
    explicit AuthService(std::shared_ptr<dao::UserDAO> user_dao, std::shared_ptr<services::LogService> log_service);

    // Основные методы аутентификации
    LoginResult login(const std::string &email, const std::string &password);
    bool authenticate(const std::string& email, const std::string& password);
    void logout();
    bool is_authenticated() const;

    // Управление паролями
    bool change_password(const std::string& email, const std::string& old_password, const std::string& new_password);
    bool change_password(const std::string& email, const std::string& new_password);


    // Информация о текущем пользователе
    std::shared_ptr<models::User> get_current_user() const;

    // Вспомогательные методы
    void update_last_login(const std::shared_ptr<models::User> &user);

private:
    std::shared_ptr<dao::UserDAO> user_dao_;
    std::shared_ptr<models::User> current_user_;
    std::shared_ptr<LogService> log_service_;
};

} // namespace services