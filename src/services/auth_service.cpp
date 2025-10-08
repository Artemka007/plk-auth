#include "auth_service.hpp"
#include "src/dao/user_dao.hpp"
#include "src/models/user_role.hpp"
#include "src/utils/password_utils.hpp"
#include <string>

namespace services
{

// Добавьте конструктор
AuthService::AuthService(std::shared_ptr<dao::UserDAO> user_dao)
    : user_dao_(std::move(user_dao)) {
}

LoginResult AuthService::login(const std::string &email,
                               const std::string &password) {
    // Find user by email
    std::shared_ptr<models::User> user = user_dao_->find_by_email(email);
    if (!user) {
        // TODO: Log failed attempt
        return {false, nullptr, false, "User not found"};
    }

    // Check if user is not active
    if (!user->is_active()) {
        return {false, nullptr, false, "Account is inactive"};
    }

    // Check password
    if (!utils::PasswordUtils::verify_password_pbkdf2(password, user->password_hash())) {
        return {false, nullptr, false, "Invalid credentials"};
    }

    // Check if password change is required
    if (user->is_password_change_required()) {
        return {true, user, true, ""};
    }

    // Success
    update_last_login(user);
    return {true, user, false, ""};
}

void AuthService::update_last_login(const std::shared_ptr<models::User> &user) {
    user_dao_->update_last_login(user);
}

bool AuthService::authenticate(const std::string& email, const std::string& password) {
    auto result = login(email, password);
    return result.success && !result.password_change_required;
}

std::shared_ptr<models::User> AuthService::get_current_user() const {
    return current_user_;
}

void AuthService::logout() {
    current_user_.reset();
}

bool AuthService::is_authenticated() const {
    return current_user_ != nullptr;
}

bool AuthService::change_password(const std::string& email, const std::string& old_password, const std::string& new_password) {
    try {
        // Аутентифицируем пользователя
        if (!authenticate(email, old_password)) {
            return false;
        }

        auto user = user_dao_->find_by_email(email);
        if (!user) {
            return false;
        }

        // Проверяем силу нового пароля
        if (!utils::PasswordUtils::is_password_strong(new_password)) {
            return false;
        }

        // Хешируем новый пароль
        std::string new_password_hash = utils::PasswordUtils::hash_password_pbkdf2(new_password);

        // Обновляем пароль в базе данных
        return user_dao_->change_password(user, new_password_hash);

    } catch (const std::exception& e) {
        std::cerr << "Password change error: " << e.what() << std::endl;
        return false;
    }
}

bool AuthService::change_password(const std::string& email, const std::string& new_password) {
    try {
        auto user = user_dao_->find_by_email(email);
        if (!user) {
            return false;
        }

        // Проверяем силу нового пароля
        if (!utils::PasswordUtils::is_password_strong(new_password)) {
            return false;
        }

        // Хешируем новый пароль
        std::string new_password_hash = utils::PasswordUtils::hash_password_pbkdf2(new_password);

        // Обновляем пароль в базе данных
        return user_dao_->change_password(user, new_password_hash);

    } catch (const std::exception& e) {
        std::cerr << "Password change error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace services