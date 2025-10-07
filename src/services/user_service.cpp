#include "user_service.hpp"
#include "src/dao/user_dao.hpp"
#include "src/models/user.hpp"
#include "src/models/user_role.hpp"
#include "src/utils/password_utils.hpp"

UserService::UserService(std::shared_ptr<dao::UserDAO> user_dao)
    : user_dao_(std::move(user_dao)) {}

std::optional<models::User>
UserService::find_by_email(const std::string &email) {
    auto user = user_dao_->find_by_email(email);
    if (user) {
        return *user;
    }
    return std::nullopt;
}

std::vector<models::User> UserService::get_all_users() {
    auto users = user_dao_->find_all();
    std::vector<models::User> result;
    for (const auto &user : users) {
        result.push_back(*user);
    }
    return result;
}

CreateUserResult UserService::create_user(const std::string &first_name,
                                          const std::string &last_name,
                                          const std::string &email) {
    // If user with this email exists
    if (user_dao_->find_by_email(email) != nullptr) {
        return {false, "User with this email already exists", nullptr, ""};
    }

    auto new_user =
        std::make_shared<models::User>(first_name, last_name, email);

    // Generate random password
    std::string user_password =
        utils::PasswordUtils::generate_random_password(12);
    std::string password_hash =
        utils::PasswordUtils::hash_password(user_password);

    new_user->set_password_hash(password_hash);
    new_user->require_password_change();

    if (!user_dao_->save(new_user)) {
        return {false, "Failed to save user to database", nullptr, ""};
    }

    // Success
    return {true, "", new_user, user_password};
}

bool UserService::delete_user(const std::string &email) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return false;
    }
    return user_dao_->remove(user->id);
}

bool UserService::add_role_to_user(const std::string &email,
                                   const models::UserRole role) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return false;
    }

    return user_dao_->assign_role(user, role);
}

bool UserService::remove_role_from_user(const std::string &email,
                                        const models::UserRole role) {
    auto user_ptr = user_dao_->find_by_email(email);
    if (!user_ptr) {
        return false;
    }

    auto role_ptr = std::make_shared<models::UserRole>(role);
    return user_dao_->remove_role(user_ptr, role_ptr);
}

bool UserService::is_admin(std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    return user_dao_->has_role(user, "ADMIN");
}

bool UserService::can_manage_users(std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    return user_dao_->has_role(user, "ADMIN") ||
           user_dao_->has_role(user, "USER_MANAGER");
}

bool UserService::has_role(std::shared_ptr<const models::User> &user,
                           const std::string &role_name) const {
    if (!user || role_name.empty()) {
        return false;
    }

    return user_dao_->has_role(user, role_name);
}

bool UserService::requires_password_change(
    std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }
    return user->password_change_required();
}

bool UserService::is_user_active(
    std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    return user->is_active();
}

std::vector<std::shared_ptr<models::UserRole>>
UserService::user_roles(std::shared_ptr<const models::User> &user) {
    return user_dao_->user_roles(user);
}
