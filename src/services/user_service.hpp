#pragma once

#include "src/dao/user_dao.hpp"
#include "src/dao/access_permission_dao.hpp"
#include "src/models/user.hpp"
#include "src/models/user_role.hpp"
#include "src/models/access_permission.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace services {
struct CreateUserResult {
    bool success = false;
    std::string message = "";
    std::shared_ptr<models::User> user = nullptr;
    std::string generated_password = "";
};

class UserService {
public:
    // Обновленный конструктор с AccessPermissionDAO
    explicit UserService(std::shared_ptr<dao::UserDAO> user_dao,
                        std::shared_ptr<dao::AccessPermissionDAO> permission_dao);

    // Системная инициализация
    void initialize_system();
    bool create_default_admin();

    // Search
    std::shared_ptr<models::User> find_by_email(const std::string &email);
    std::vector<models::User> get_all_users();

    // Create
    CreateUserResult create_user(const std::string &first_name,
                                 const std::string &last_name,
                                 const std::string &email,
                                 const std::string& role_name = "USER");

    // Delete
    bool delete_user(const std::string &email);
    bool add_role_to_user(const std::string &email,
                          const std::shared_ptr<models::UserRole> role);
    bool remove_role_from_user(const std::string &email,
                               const models::UserRole role);

    // Permissions and roles
    bool is_admin(const std::shared_ptr<const models::User> &user) const;
    bool
    can_manage_users(const std::shared_ptr<const models::User> &user) const;
    bool has_role(const std::shared_ptr<const models::User> &user,
                  const std::string &role_name) const;
    bool requires_password_change(
        const std::shared_ptr<const models::User> &user) const;
    bool is_user_active(const std::shared_ptr<const models::User> &user) const;
    std::vector<std::shared_ptr<models::UserRole>> user_roles(const std::shared_ptr<const models::User> &user) const;

    bool has_permission(const std::shared_ptr<const models::User>& user, const std::string& permission_name) const;
    std::vector<std::string> get_user_permissions(const std::shared_ptr<const models::User>& user) const;
    std::shared_ptr<models::UserRole> get_role_by_name(const std::string& role_name);

private:
    std::shared_ptr<dao::UserDAO> user_dao_;
    std::shared_ptr<dao::AccessPermissionDAO> permission_dao_;

    bool create_system_roles();
};
} // namespace services