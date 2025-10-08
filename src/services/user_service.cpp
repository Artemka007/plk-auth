#include "user_service.hpp"
#include "src/cli/io_handler.hpp"
#include "src/dao/access_permission_dao.hpp"
#include "src/dao/user_dao.hpp"
#include "src/models/user.hpp"
#include "src/models/user_role.hpp"
#include "src/utils/password_utils.hpp"
#include "log_service.hpp"
#include "src/models/enums.hpp"
#include <iostream>

namespace services {

UserService::UserService(
    std::shared_ptr<IOHandler> io_handler, std::shared_ptr<dao::UserDAO> user_dao,
    std::shared_ptr<dao::AccessPermissionDAO> permission_dao,
    std::shared_ptr<LogService> log_service)
    : io_handler_(std::move(io_handler)), user_dao_(std::move(user_dao)),
      permission_dao_(std::move(permission_dao)), log_service_(std::move(log_service)) {}

void UserService::initialize_system() {
    try {
        io_handler_->println("Initializing system...");
        log_service_->info(models::ActionType::SYSTEM_STARTUP, "Starting system initialization");

        permission_dao_->initialize_system_permissions();

        create_system_roles();

        if (create_default_admin()) {
            io_handler_->println("System initialization completed successfully");
            log_service_->info(models::ActionType::SYSTEM_STARTUP, "System initialization completed successfully");
        } else {
            io_handler_->println("System initialization completed with warnings");
            log_service_->warning(models::ActionType::SYSTEM_STARTUP, "System initialization completed with warnings");
        }

    } catch (const std::exception &e) {
        io_handler_->error("❌ System initialization failed: " + std::string(e.what()));
        log_service_->critical(models::ActionType::SYSTEM_STARTUP, "System initialization failed: " + std::string(e.what()));
        throw;
    }
}

bool UserService::create_default_admin() {
    try {
        auto existing_admin = user_dao_->find_by_email("admin@sys.local");
        if (existing_admin) {
            io_handler_->println("ℹ️ System administrator already exists");
            log_service_->debug(models::ActionType::USER_CREATED, "System administrator already exists");
            return true;
        }

        io_handler_->println("👑 Creating system administrator...");
        log_service_->info(models::ActionType::USER_CREATED, "Creating system administrator account");

        auto admin_user = std::make_shared<models::User>(
            "System", "Administrator", "admin@sys.local");

        std::string password =
            utils::PasswordUtils::generate_random_password(12);
        std::string password_hash =
            utils::PasswordUtils::hash_password_pbkdf2(password);

        admin_user->set_password_hash(password_hash);
        admin_user->set_active(true);
        admin_user->require_password_change();

        if (!user_dao_->save(admin_user)) {
            io_handler_->error("❌ Failed to save admin user");
            log_service_->error(models::ActionType::USER_CREATED, "Failed to save admin user to database");
            return false;
        }

        auto admin_role = get_role_by_name("ADMIN");
        if (!admin_role) {
            io_handler_->error("❌ ADMIN role not found");
            log_service_->error(models::ActionType::USER_ROLE_CHANGED, "ADMIN role not found for system administrator");
            return false;
        }

        if (!user_dao_->assign_role(admin_user, admin_role)) {
            io_handler_->error("❌ Failed to assign ADMIN role");
            log_service_->error(models::ActionType::USER_ROLE_CHANGED, "Failed to assign ADMIN role to system administrator");
            return false;
        }

        io_handler_->println("==========================================");
        io_handler_->println("🎯 SYSTEM ADMINISTRATOR CREATED");
        io_handler_->println("==========================================");
        io_handler_->println("📧 Email: admin@sys.local");
        io_handler_->println("🔑 Password: " + password);
        io_handler_->println("⚠️  Please change password after first login!");
        io_handler_->println("==========================================");

        log_service_->info(models::ActionType::USER_CREATED, 
                          "System administrator created successfully with temporary password",
                          nullptr, admin_user);

        return true;

    } catch (const std::exception &e) {
        io_handler_->error("❌ Error creating default admin: " + std::string(e.what()));
        log_service_->critical(models::ActionType::USER_CREATED, "Error creating default admin: " + std::string(e.what()));
        return false;
    }
}

bool UserService::create_system_roles() {
    try {
        log_service_->debug(models::ActionType::ROLE_CREATED, "Creating system roles");
        return true;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Error creating system roles: " + std::string(e.what()));
        log_service_->error(models::ActionType::ROLE_CREATED, "Error creating system roles: " + std::string(e.what()));
        return false;
    }
}

CreateUserResult UserService::create_user(const std::string &first_name,
                                          const std::string &last_name,
                                          const std::string &email,
                                          const std::string &role_name,
                                          const std::shared_ptr<const models::User>& actor) {
    try {
        if (user_dao_->find_by_email(email) != nullptr) {
            log_service_->warning(models::ActionType::USER_CREATED, 
                                 "User creation failed - email already exists: " + email,
                                 actor, nullptr);
            return {false, "User with this email already exists", nullptr, ""};
        }

        auto new_user =
            std::make_shared<models::User>(first_name, last_name, email);

        std::string user_password =
            utils::PasswordUtils::generate_random_password(12);
        std::string password_hash =
            utils::PasswordUtils::hash_password_pbkdf2(user_password);

        new_user->set_password_hash(password_hash);
        new_user->require_password_change();
        new_user->set_active(true);

        if (!user_dao_->save(new_user)) {
            log_service_->error(models::ActionType::USER_CREATED,
                               "Failed to save user to database: " + email,
                               actor, nullptr);
            return {false, "Failed to save user to database", nullptr, ""};
        }

        auto role = get_role_by_name(role_name);
        if (role) {
            if (!user_dao_->assign_role(new_user, role)) {
                io_handler_->println("⚠️ Failed to assign role " + role_name + " to user " + email);
                log_service_->warning(models::ActionType::USER_ROLE_CHANGED,
                                     "Failed to assign role " + role_name + " to user: " + email,
                                     actor, new_user);
            } else {
                log_service_->info(models::ActionType::USER_ROLE_CHANGED,
                                  "Assigned role " + role_name + " to user: " + email,
                                  actor, new_user);
            }
        } else {
            io_handler_->println("⚠️ Role " + role_name + " not found for user " + email);
            log_service_->warning(models::ActionType::USER_ROLE_CHANGED,
                                 "Role " + role_name + " not found for user: " + email,
                                 actor, new_user);
        }

        log_service_->info(models::ActionType::USER_CREATED,
                          "User created successfully: " + email + " with role: " + role_name,
                          actor, new_user);

        return {true, "User created successfully", new_user, user_password};

    } catch (const std::exception &e) {
        io_handler_->error("❌ Error creating user: " + std::string(e.what()));
        log_service_->error(models::ActionType::USER_CREATED,
                           "Error creating user " + email + ": " + std::string(e.what()),
                           actor, nullptr);
        return {false, "Error creating user: " + std::string(e.what()), nullptr, ""};
    }
}

bool UserService::has_permission(
    const std::shared_ptr<const models::User> &user,
    const std::string &permission_name) const {
    if (!user) {
        return false;
    }

    auto non_const_user = std::const_pointer_cast<models::User>(user);
    auto roles = user_dao_->user_roles(non_const_user);

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

    std::sort(permissions.begin(), permissions.end());
    permissions.erase(std::unique(permissions.begin(), permissions.end()),
                      permissions.end());

    return permissions;
}

std::shared_ptr<models::UserRole> UserService::get_role_by_name(const std::string& role_name) {
    auto role = user_dao_->get_role_by_name(role_name);
    if (!role) {
        log_service_->debug(models::ActionType::ROLE_CREATED, "Role not found: " + role_name);
    }
    return role;
}

std::shared_ptr<models::User> UserService::find_by_email(const std::string &email) {
    auto user = user_dao_->find_by_email(email);
    if (user) {
        log_service_->debug(models::ActionType::PROFILE_VIEWED, "User found by email: " + email);
    } else {
        log_service_->debug(models::ActionType::PROFILE_VIEWED, "User not found by email: " + email);
    }
    return user;
}

std::vector<models::User> UserService::get_all_users() {
    auto users = user_dao_->find_all();
    log_service_->debug(models::ActionType::PROFILE_VIEWED, 
                       "Retrieved all users, count: " + std::to_string(users.size()));
    std::vector<models::User> result;
    for (const auto &user : users) {
        result.push_back(*user);
    }
    return result;
}

bool UserService::delete_user(const std::string &email, const std::shared_ptr<const models::User>& actor) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        log_service_->warning(models::ActionType::USER_DELETED,
                             "User deletion failed - user not found: " + email,
                             actor, nullptr);
        return false;
    }

    bool result = user_dao_->delete_by_id(user->id());
    if (result) {
        log_service_->info(models::ActionType::USER_DELETED,
                          "User deleted successfully: " + email,
                          actor, user);
    } else {
        log_service_->error(models::ActionType::USER_DELETED,
                           "User deletion failed: " + email,
                           actor, user);
    }
    return result;
}

bool UserService::add_role_to_user(const std::string &email, 
                                  const std::shared_ptr<models::UserRole> role,
                                  const std::shared_ptr<const models::User>& actor) {
    auto user = user_dao_->find_by_email(email);
    if (!user) {
        log_service_->warning(models::ActionType::USER_ROLE_CHANGED,
                             "Add role failed - user not found: " + email,
                             actor, nullptr);
        return false;
    }

    bool result = user_dao_->assign_role(user, role);
    if (result) {
        log_service_->info(models::ActionType::USER_ROLE_CHANGED,
                          "Role " + role->name() + " added to user: " + email,
                          actor, user);
    } else {
        log_service_->error(models::ActionType::USER_ROLE_CHANGED,
                           "Failed to add role " + role->name() + " to user: " + email,
                           actor, user);
    }
    return result;
}

bool UserService::remove_role_from_user(const std::string &email,
                                        const models::UserRole role,
                                        const std::shared_ptr<const models::User>& actor) {
    auto user_ptr = user_dao_->find_by_email(email);
    if (!user_ptr) {
        log_service_->warning(models::ActionType::USER_ROLE_CHANGED,
                             "Remove role failed - user not found: " + email,
                             actor, nullptr);
        return false;
    }

    auto role_ptr = std::make_shared<models::UserRole>(role);
    bool result = user_dao_->remove_role(user_ptr, role_ptr);
    
    if (result) {
        log_service_->info(models::ActionType::USER_ROLE_CHANGED,
                          "Role " + role.name() + " removed from user: " + email,
                          actor, user_ptr);
    } else {
        log_service_->error(models::ActionType::USER_ROLE_CHANGED,
                           "Failed to remove role " + role.name() + " from user: " + email,
                           actor, user_ptr);
    }
    return result;
}

bool UserService::is_admin(const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }
    auto non_const_user = std::const_pointer_cast<models::User>(user);
    return user_dao_->has_role(non_const_user, "ADMIN");
}

bool UserService::can_manage_users(const std::shared_ptr<const models::User> &user) const {
    if (!user) {
        return false;
    }

    return has_permission(user, "USER_CREATE") ||
           has_permission(user, "USER_UPDATE") ||
           has_permission(user, "USER_DELETE");
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

}