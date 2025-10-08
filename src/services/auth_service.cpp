#include "auth_service.hpp"
#include "src/models/user_role.hpp"
#include "src/utils/password_utils.hpp"
#include <string>
#include "log_service.hpp"
#include "src/models/enums.hpp"
#include "src/dao/user_dao.hpp"


namespace services {
AuthService::AuthService(std::shared_ptr<dao::UserDAO> user_dao, std::shared_ptr<services::LogService> log_service)
    : user_dao_(std::move(user_dao)),
    log_service_(std::move(log_service)) {}

LoginResult AuthService::login(const std::string &email,
                               const std::string &password) {
    // Find user by email
    std::shared_ptr<models::User> user = user_dao_->find_by_email(email);
    if (!user) {
        log_service_->error(models::ActionType::SYSTEM_LOGIN, 
                           "Failed login attempt - user not found: " + email,
                           nullptr, nullptr, "192.168.1.100", "CLI Client");
        return {false, nullptr, false, "User not found"};
    }

    // Check if user is not active
    if (!user->is_active()) {
        log_service_->warning(models::ActionType::SECURITY_ACCESS_DENIED,
                             "Login attempt to inactive account: " + email,
                             nullptr, user, "192.168.1.100", "CLI Client");
        return {false, nullptr, false, "Account is inactive"};
    }

    // Check password
    if (!utils::PasswordUtils::verify_password_pbkdf2(password,
                                                      user->password_hash())) {
        log_service_->warning(models::ActionType::SECURITY_ACCESS_DENIED,
                             "Invalid password for user: " + email,
                             nullptr, user, "192.168.1.100", "CLI Client");
        return {false, nullptr, false, "Invalid credentials"};
    }

    // Check if password change is required
    if (user->is_password_change_required()) {
        log_service_->info(models::ActionType::SYSTEM_LOGIN,
                          "User login successful - password change required: " + email,
                          user, nullptr, "192.168.1.100", "CLI Client");
        return {true, user, true, ""};
    }

    // Success
    log_service_->info(models::ActionType::SYSTEM_LOGIN,
                      "User login successful: " + email,
                      user, nullptr, "192.168.1.100", "CLI Client");
    update_last_login(user);
    return {true, user, false, ""};
}

void AuthService::update_last_login(const std::shared_ptr<models::User> &user) {
    user_dao_->update_last_login(user);
    log_service_->debug(models::ActionType::USER_UPDATED,
                       "Updated last login timestamp for user: " + user->email(),
                       user, nullptr, "192.168.1.100", "CLI Client");
}

bool AuthService::authenticate(const std::string &email,
                               const std::string &password) {
    auto result = login(email, password);
    return result.success && !result.password_change_required;
}

std::shared_ptr<models::User> AuthService::get_current_user() const {
    return current_user_;
}

void AuthService::logout() { 
    if (current_user_) {
        log_service_->info(models::ActionType::SYSTEM_LOGOUT,
                          "User logged out: " + current_user_->email(),
                          current_user_, nullptr, "192.168.1.100", "CLI Client");
    }
    current_user_.reset(); 
}

bool AuthService::is_authenticated() const { return current_user_ != nullptr; }

bool AuthService::change_password(const std::string &email,
                                  const std::string &old_password,
                                  const std::string &new_password) {
    try {
        // Аутентифицируем пользователя
        if (!authenticate(email, old_password)) {
            log_service_->warning(models::ActionType::SECURITY_PASSWORD_RESET,
                                "Password change failed - authentication failed: " + email,
                                nullptr, nullptr, "192.168.1.100", "CLI Client");
            return false;
        }

        auto user = user_dao_->find_by_email(email);
        if (!user) {
            log_service_->error(models::ActionType::SECURITY_PASSWORD_RESET,
                              "Password change failed - user not found: " + email,
                              nullptr, nullptr, "192.168.1.100", "CLI Client");
            return false;
        }

        // Проверяем силу нового пароля
        if (!utils::PasswordUtils::is_password_strong(new_password)) {
            log_service_->warning(models::ActionType::SECURITY_PASSWORD_RESET,
                                "Password change failed - weak password: " + email,
                                user, nullptr, "192.168.1.100", "CLI Client");
            return false;
        }

        // Хешируем новый пароль
        std::string new_password_hash =
            utils::PasswordUtils::hash_password_pbkdf2(new_password);

        // Обновляем пароль в базе данных
        bool success = user_dao_->change_password(user, new_password_hash);
        if (success) {
            log_service_->info(models::ActionType::USER_PASSWORD_CHANGED,
                             "Password changed successfully for user: " + email,
                             user, nullptr, "192.168.1.100", "CLI Client");
        } else {
            log_service_->error(models::ActionType::SECURITY_PASSWORD_RESET,
                              "Password change failed - database error: " + email,
                              user, nullptr, "192.168.1.100", "CLI Client");
        }
        return success;

    } catch (const std::exception &e) {
        log_service_->error(models::ActionType::SECURITY_VIOLATION,
                          "Password change exception: " + std::string(e.what()),
                          nullptr, nullptr, "192.168.1.100", "CLI Client");
        return false;
    }
}

bool AuthService::change_password(const std::string &email,
                                  const std::string &new_password) {
    try {
        auto user = user_dao_->find_by_email(email);
        if (!user) {
            log_service_->error(models::ActionType::SECURITY_PASSWORD_RESET,
                              "Admin password reset failed - user not found: " + email,
                              nullptr, nullptr, "192.168.1.100", "CLI Client");
            return false;
        }

        // Проверяем силу нового пароля
        if (!utils::PasswordUtils::is_password_strong(new_password)) {
            log_service_->warning(models::ActionType::SECURITY_PASSWORD_RESET,
                                "Admin password reset failed - weak password: " + email,
                                nullptr, user, "192.168.1.100", "CLI Client");
            return false;
        }

        // Хешируем новый пароль
        std::string new_password_hash =
            utils::PasswordUtils::hash_password_pbkdf2(new_password);

        // Обновляем пароль в базе данных
        bool success = user_dao_->change_password(user, new_password_hash);
        if (success) {
            log_service_->info(models::ActionType::SECURITY_PASSWORD_RESET,
                             "Admin password reset successful for user: " + email,
                             nullptr, user, "192.168.1.100", "CLI Client");
        } else {
            log_service_->error(models::ActionType::SECURITY_PASSWORD_RESET,
                              "Admin password reset failed - database error: " + email,
                              nullptr, user, "192.168.1.100", "CLI Client");
        }
        return success;

    } catch (const std::exception &e) {
        log_service_->error(models::ActionType::SECURITY_VIOLATION,
                          "Admin password reset exception: " + std::string(e.what()),
                          nullptr, nullptr, "192.168.1.100", "CLI Client");
        return false;
    }
}

}