#include "user_service.hpp"
#include "src/cli/io_handler.hpp"
#include "src/dao/access_permission_dao.hpp"
#include "src/dao/user_dao.hpp"
#include "src/models/user.hpp"
#include "src/models/user_role.hpp"
#include "src/utils/password_utils.hpp"
#include <iostream>

namespace services {

UserService::UserService(
    std::shared_ptr<IOHandler> io_handler, std::shared_ptr<dao::UserDAO> user_dao,
    std::shared_ptr<dao::AccessPermissionDAO> permission_dao)
    : io_handler_(std::move(io_handler)), user_dao_(std::move(user_dao)),
      permission_dao_(std::move(permission_dao)) {}

// Инициализация системы
void UserService::initialize_system() {
    try {
        io_handler_->println("Initializing system...");

        // 1. Инициализируем системные разрешения и роли
        permission_dao_->initialize_system_permissions();

        // 2. Создаем системные роли (если нужно)
        create_system_roles();

        // 3. Создаем администратора по умолчанию
        if (create_default_admin()) {
            io_handler_->println("System initialization completed successfully");
        } else {
            io_handler_->println("System initialization completed with warnings");
        }

    } catch (const std::exception &e) {
        io_handler_->error("❌ System initialization failed: " + std::string(e.what()));
        throw;
    }
}

bool UserService::create_default_admin() {
    try {
        // Проверяем, не существует ли уже администратор
        auto existing_admin = user_dao_->find_by_email("admin@system.local");
        if (existing_admin) {
            io_handler_->println("ℹ️ System administrator already exists");
            return true;
        }

        io_handler_->println("👑 Creating system administrator...");

        // Создаем администратора
        auto admin_user = std::make_shared<models::User>(
            "System", "Administrator", "admin@system.local");

        // Генерируем случайный пароль
        std::string password =
            utils::PasswordUtils::generate_random_password(12);
        std::string password_hash =
            utils::PasswordUtils::hash_password_pbkdf2(password);

        admin_user->set_password_hash(password_hash);
        admin_user->set_active(true);
        admin_user->require_password_change(); // Требуем смены пароля при
                                               // первом входе

        if (!user_dao_->save(admin_user)) {
            io_handler_->error("❌ Failed to save admin user");
            return false;
        }

        // Назначаем роль администратора
        auto admin_role = get_role_by_name("ADMIN");
        if (!admin_role) {
            io_handler_->error("❌ ADMIN role not found");
            return false;
        }

        if (!user_dao_->assign_role(admin_user, admin_role)) {
            io_handler_->error("❌ Failed to assign ADMIN role");
            return false;
        }

        // Выводим информацию для администратора
        io_handler_->println("==========================================");
        io_handler_->println("🎯 SYSTEM ADMINISTRATOR CREATED");
        io_handler_->println("==========================================");
        io_handler_->println("📧 Email: admin@system.local");
        io_handler_->println("🔑 Password: " + password);
        io_handler_->println("⚠️  Please change password after first login!");
        io_handler_->println("==========================================");

        return true;

    } catch (const std::exception &e) {
        io_handler_->error("❌ Error creating default admin: " + std::string(e.what()));
        return false;
    }
}

bool UserService::create_system_roles() {
    try {
        // Роли уже создаются в initialize_system_permissions()
        // Здесь можно добавить дополнительную логику если нужно
        return true;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Error creating system roles: " + std::string(e.what()));
        return false;
    }
}

// Обновленный метод создания пользователя с поддержкой ролей
CreateUserResult UserService::create_user(const std::string &first_name,
                                          const std::string &last_name,
                                          const std::string &email,
                                          const std::string &role_name) {
    try {
        // Если пользователь с таким email уже существует
        if (user_dao_->find_by_email(email) != nullptr) {
            return {false, "User with this email already exists", nullptr, ""};
        }

        auto new_user =
            std::make_shared<models::User>(first_name, last_name, email);

        // Генерируем случайный пароль
        std::string user_password =
            utils::PasswordUtils::generate_random_password(12);
        std::string password_hash =
            utils::PasswordUtils::hash_password_pbkdf2(user_password);

        new_user->set_password_hash(password_hash);
        new_user->require_password_change();
        new_user->set_active(true);

        if (!user_dao_->save(new_user)) {
            return {false, "Failed to save user to database", nullptr, ""};
        }

        // Назначаем роль пользователю
        auto role = get_role_by_name(role_name);
        if (role) {
            if (!user_dao_->assign_role(new_user, role)) {
                io_handler_->println("⚠️ Failed to assign role " + role_name + " to user " + email);
            }
        } else {
            io_handler_->println("⚠️ Role " + role_name + " not found for user " + email);
        }

        // Успех
        return {true, "User created successfully", new_user, user_password};

    } catch (const std::exception &e) {
        io_handler_->error("❌ Error creating user: " + std::string(e.what()));
        return {false, "Error creating user: " + std::string(e.what()), nullptr, ""};
    }
}

// Новые методы для проверки разрешений
bool UserService::has_permission(
    const std::shared_ptr<const models::User> &user,
    const std::string &permission_name) const {
    if (!user) {
        return false;
    }

    // Получаем роли пользователя
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    auto roles = user_dao_->user_roles(non_const_user);

    // Проверяем каждую роль на наличие разрешения
    for (const auto &role : roles) {
        if (permission_dao_->role_has_permission(role->id(), permission_name)) {
            return true;
        }
    }

    return false;
}

std::vector<std::string> UserService::get_user_permissions(
    const std::shared_ptr<const models::User> &user) const {
    std::vector<std::string> permissions;

    if (!user) {
        return permissions;
    }

    auto non_const_user = std::const_pointer_cast<models::User>(user);
    auto roles = user_dao_->user_roles(non_const_user);

    for (const auto &role : roles) {
        auto role_permissions =
            permission_dao_->get_role_permissions(role->id());
        for (const auto &perm : role_permissions) {
            permissions.push_back(perm->name());
        }
    }

    // Убираем дубликаты
    std::sort(permissions.begin(), permissions.end());
    permissions.erase(std::unique(permissions.begin(), permissions.end()),
                      permissions.end());

    return permissions;
}

std::shared_ptr<models::UserRole> UserService::get_role_by_name(const std::string& role_name) {
    return user_dao_->get_role_by_name(role_name);
}

std::shared_ptr<models::User> UserService::find_by_email(const std::string &email) {
    return user_dao_->find_by_email(email);
}

std::vector<models::User> UserService::get_all_users() {
    auto users = user_dao_->find_all();
    std::vector<models::User> result;
    for (const auto &user : users) {
        result.push_back(*user);
    }
    return result;
}

bool UserService::delete_user(const std::string &email) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        return false;
    }
    return user_dao_->delete_by_id(user->id());
}

bool UserService::add_role_to_user(const std::string &email, const std::shared_ptr<models::UserRole> role) {
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

bool UserService::is_admin(
    const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->has_role(non_const_user, "ADMIN");
}

bool UserService::can_manage_users(
    const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    // Используем новую систему разрешений
    return has_permission(user, models::SystemPermissions::USER_CREATE) ||
           has_permission(user, models::SystemPermissions::USER_UPDATE) ||
           has_permission(user, models::SystemPermissions::USER_DELETE);
}

bool UserService::has_role(const std::shared_ptr<const models::User> &user,
                           const std::string &role_name) const {
    if (!user || role_name.empty()) {
        return false;
    }
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
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->user_roles(non_const_user);
}

} // namespace services