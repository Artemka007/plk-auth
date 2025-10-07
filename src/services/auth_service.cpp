#include "services/auth_service.hpp"
#include "dao/user_dao.hpp"
#include "models/user_role.hpp"
#include "utils/PasswordUtils.hpp"
#include <string>

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
    if (!utils::PasswordUtils::verify_password(password, user->password_hash())) {
        return {false, nullptr, false, "Invalid credentials"};
    }

    // Check if password change is required
    if (user->password_change_required()) {
        return {true, user, true, ""};
    }

    // Success
    update_last_login(user);
    return {true, user, false, ""};
}

void AuthService::update_last_login(const std::shared_ptr<models::User> &user) {
    user_dao_->update_last_login(user);
}
