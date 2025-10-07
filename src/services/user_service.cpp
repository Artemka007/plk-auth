#include "user_service.hpp"
#include "src/dao/user_dao.hpp"
#include "src/models/user.hpp"
#include "src/models/user_role.hpp"
#include "src/utils/password_utils.hpp"

namespace services
{

UserService::UserService(std::shared_ptr<dao::UserDAO> user_dao)
    : user_dao_(std::move(user_dao)) {}

std::optional<models::User> UserService::find_by_email(const std::string &email) {
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

    auto new_user = std::make_shared<models::User>(first_name, last_name, email);

    // Generate random password
    std::string user_password = utils::PasswordUtils::generate_random_password(12);
    std::string password_hash = utils::PasswordUtils::hash_password_pbkdf2(user_password);

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
    return user_dao_->delete_by_id(user->id());
}

bool UserService::add_role_to_user(const std::string &email,
                                   const models::UserRole role) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return false;
    }

    // Создаем shared_ptr для роли
    auto role_ptr = std::make_shared<models::UserRole>(role);
    return user_dao_->assign_role(user, role_ptr);
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

bool UserService::is_admin(const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    // Создаем не-const копию для вызова DAO
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->has_role(non_const_user, "ADMIN");
}

bool UserService::can_manage_users(const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    // Создаем не-const копию для вызова DAO
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->has_role(non_const_user, "ADMIN") ||
           user_dao_->has_role(non_const_user, "USER_MANAGER");
}

bool UserService::has_role(const std::shared_ptr<const models::User> &user,
                           const std::string &role_name) const {
    if (!user || role_name.empty()) {
        return false;
    }

    // Создаем не-const копию для вызова DAO
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->has_role(non_const_user, role_name);
}

bool UserService::requires_password_change(
    const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }
    return user->is_password_change_required();
}

bool UserService::is_user_active(
    const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    return user->is_active();
}

std::vector<std::shared_ptr<models::UserRole>>
UserService::user_roles(const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return {};
    }

    // Создаем не-const копию для вызова DAO
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->user_roles(non_const_user);
}

} // namespace services