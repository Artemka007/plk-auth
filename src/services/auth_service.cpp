#include "services/AuthService.hpp"
#include "utils/PasswordUtils.hpp"
#include <string>

LoginResult AuthService::login(const std::string &email,
                               const std::string &password) {
    // Find user by email
    std::shared_ptr<models::User> user = user_dao_->find_by_email(email);
    if (!user) {
        // TODO: Log failed attempt
        increment_login_attempts(email);
        return {nullptr, false, "User not found"};
    }

    // Check if user is not active
    if (!user->is_active()) {
        increment_login_attempts(email);
        return {nullptr, false, "Account is inactive"};
    }

    // Check if login attempts exceeded
    if (is_login_attempts_exceeded(email)) {
        return {nullptr, false, "Login attempts exceeded"};
    }

    // Check password
    if (!utils::PasswordUtils::verify_password(password,
                                               user->password_hash())) {
        increment_login_attempts(email);
        return {nullptr, false, "Invalid credentials"};
    }

    reset_login_attempts(email);
    update_last_login(user);

    // Check if password change is required
    if (user->password_change_required()) {
        return {user, true, ""}
    }

    // Success
    return {user, false, ""};
}

// Todo: Implement user_dao method get_failed_login_attempts_count
bool AuthService::is_login_attempts_exceeded(const std::string &email) const {
    return user_dao_->get_failed_login_attempts_count(email) > 5;
}

// Todo: Implement user_dao method reset_failed_login_attempts
void AuthService::reset_login_attempts(const std::string &email) {
    user_dao_->reset_failed_login_attempts(email);
}

// Todo: Implement user_dao method increment_failed_login_attempts
void AuthService::increment_login_attempts(const std::string &email) {
    user_dao_->increment_failed_login_attempts(email);
}

void AuthService::update_last_login(const std::shared_ptr<models::User> &user) {
    user_dao_->update_last_login(user);
}
