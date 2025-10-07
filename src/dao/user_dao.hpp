#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../models/user.hpp"
#include "../models/user_role.hpp"

namespace dao {

class UserDAO {
private:
    std::shared_ptr<pqxx::connection> connection_;
    
public:
    explicit UserDAO(std::shared_ptr<pqxx::connection> conn);
    
    // CRUD операции
    std::shared_ptr<models::User> find_by_id(const std::string& id);
    std::shared_ptr<models::User> find_by_email(const std::string& email);
    std::shared_ptr<models::User> find_by_credentials(const std::string& email, const std::string& password);
    std::vector<std::shared_ptr<models::User>> find_all();
    std::vector<std::shared_ptr<models::User>> find_active_users();
    bool save(const std::shared_ptr<models::User>& user);
    bool update(const std::shared_ptr<models::User>& user);
    bool remove(const std::shared_ptr<models::User>& user);
    bool delete_by_id(const std::string& id);
    
    // Управление ролями
    std::vector<std::shared_ptr<models::UserRole>> user_roles(
        const std::shared_ptr<models::User>& user);
    bool assign_role(
        const std::shared_ptr<models::User>& user,
        const std::shared_ptr<models::UserRole>& role);
    bool remove_role(
        const std::shared_ptr<models::User>& user,
        const std::shared_ptr<models::UserRole>& role);
    bool has_role(
        const std::shared_ptr<models::User>& user,
        const std::string& role_name);
    
    // Бизнес-методы
    bool update_last_login(const std::shared_ptr<models::User>& user);
    bool change_password(
        const std::shared_ptr<models::User>& user,
        const std::string& new_password_hash);
    bool deactivate_user(const std::shared_ptr<models::User>& user);
    bool activate_user(const std::shared_ptr<models::User>& user);
    
    // Поиск и фильтрация
    std::vector<std::shared_ptr<models::User>> find_by_name(
        const std::string& first_name, 
        const std::string& last_name);
    std::vector<std::shared_ptr<models::User>> find_users_requiring_password_change();
};

} // namespace dao