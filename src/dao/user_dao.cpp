#include "user_dao.hpp"
#include <algorithm>
#include <random>
#include <sstream>
#include <pqxx/pqxx>

namespace dao {

UserDAO::UserDAO(std::shared_ptr<pqxx::connection> conn) 
    : connection_(std::move(conn)) {
}

std::shared_ptr<models::User> UserDAO::find_by_id(const std::string& id) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec_params(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user WHERE id = $1", id);
        
        txn.commit();
        
        if (result.empty()) {
            return nullptr;
        }
        
        auto user = std::make_shared<models::User>();
        user->from_row(result[0]);
        return user;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return nullptr;
    }
}

std::shared_ptr<models::User> UserDAO::find_by_email(const std::string& email) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec_params(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user WHERE email = $1", email);
        
        txn.commit();
        
        if (result.empty()) {
            return nullptr;
        }
        
        auto user = std::make_shared<models::User>();
        user->from_row(result[0]);
        return user;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return nullptr;
    }
}

std::shared_ptr<models::User> UserDAO::find_by_credentials(const std::string& email, const std::string& password_hash) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec_params(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user "
            "WHERE email = $1 AND password_hash = $2 AND is_active = true", 
            email, password_hash);
        
        txn.commit();
        
        if (result.empty()) {
            return nullptr;
        }
        
        auto user = std::make_shared<models::User>();
        user->from_row(result[0]);
        return user;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return nullptr;
    }
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_all() {
    std::vector<std::shared_ptr<models::User>> users;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user ORDER BY created_at DESC");
        
        txn.commit();
        
        for (const auto& row : result) {
            auto user = std::make_shared<models::User>();
            user->from_row(row);
            users.push_back(user);
        }
    } catch (const std::exception& e) {
        // Логирование ошибки
    }
    
    return users;
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_active_users() {
    std::vector<std::shared_ptr<models::User>> users;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user "
            "WHERE is_active = true ORDER BY created_at DESC");
        
        txn.commit();
        
        for (const auto& row : result) {
            auto user = std::make_shared<models::User>();
            user->from_row(row);
            users.push_back(user);
        }
    } catch (const std::exception& e) {
        // Логирование ошибки
    }
    
    return users;
}

bool UserDAO::save(const std::shared_ptr<models::User>& user) {
    try {
        if (user->id().empty()) {
            user->set_id(generate_uuid());
        }
        
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "INSERT INTO app_user (id, first_name, last_name, patronymic, email, "
            "phone, password_hash, is_active, password_change_required) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9)",
            user->id(),
            user->first_name(),
            user->last_name(),
            user->patronymic().value_or(""),
            user->email(),
            user->phone().value_or(""),
            user->password_hash(),
            user->is_active(),
            user->is_password_change_required()
        );
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::update(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "UPDATE app_user SET first_name = $1, last_name = $2, patronymic = $3, "
            "email = $4, phone = $5, password_hash = $6, is_active = $7, "
            "password_change_required = $8, updated_at = CURRENT_TIMESTAMP "
            "WHERE id = $9",
            user->first_name(),
            user->last_name(),
            user->patronymic().value_or(""),
            user->email(),
            user->phone().value_or(""),
            user->password_hash(),
            user->is_active(),
            user->is_password_change_required(),
            user->id()
        );
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::remove(const std::shared_ptr<models::User>& user) {
    return delete_by_id(user->id());
}

bool UserDAO::delete_by_id(const std::string& id) {
    try {
        pqxx::work txn(*connection_);
        
        // Сначала удаляем связи с ролями
        txn.exec_params("DELETE FROM user_role_assignment WHERE user_id = $1", id);
        
        // Затем удаляем пользователя
        txn.exec_params("DELETE FROM app_user WHERE id = $1", id);
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

std::vector<std::shared_ptr<models::UserRole>> UserDAO::user_roles(const std::shared_ptr<models::User>& user) {
    std::vector<std::shared_ptr<models::UserRole>> roles;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec_params(
            "SELECT ur.id, ur.name, ur.description, ur.is_system, ur.created_at, ur.updated_at "
            "FROM user_role ur "
            "INNER JOIN user_role_assignment ura ON ur.id = ura.role_id "
            "WHERE ura.user_id = $1", user->id());
        
        txn.commit();
        
        for (const auto& row : result) {
            auto role = std::make_shared<models::UserRole>();
            role->from_row(row);
            roles.push_back(role);
        }
    } catch (const std::exception& e) {
        // Логирование ошибки
    }
    
    return roles;
}

bool UserDAO::assign_role(const std::shared_ptr<models::User>& user, const std::shared_ptr<models::UserRole>& role) {
    try {
        // Проверяем, не назначена ли уже эта роль
        if (has_role(user, role->name())) {
            return true; // Роль уже назначена
        }
        
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "INSERT INTO user_role_assignment (user_id, role_id) VALUES ($1, $2)",
            user->id(), role->id());
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::remove_role(const std::shared_ptr<models::User>& user, const std::shared_ptr<models::UserRole>& role) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "DELETE FROM user_role_assignment WHERE user_id = $1 AND role_id = $2",
            user->id(), role->id());
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::has_role(const std::shared_ptr<models::User>& user, const std::string& role_name) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec_params(
            "SELECT COUNT(*) FROM user_role_assignment ura "
            "INNER JOIN user_role ur ON ura.role_id = ur.id "
            "WHERE ura.user_id = $1 AND ur.name = $2", user->id(), role_name);
        
        txn.commit();
        
        return result[0][0].as<int>() > 0;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::update_last_login(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "UPDATE app_user SET last_login_at = CURRENT_TIMESTAMP WHERE id = $1",
            user->id());
        
        txn.commit();
        
        // Обновляем объект пользователя
        user->set_last_login_at("CURRENT_TIMESTAMP");
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::change_password(const std::shared_ptr<models::User>& user, const std::string& new_password_hash) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "UPDATE app_user SET password_hash = $1, password_change_required = false, "
            "updated_at = CURRENT_TIMESTAMP WHERE id = $2",
            new_password_hash, user->id());
        
        txn.commit();
        
        // Обновляем объект пользователя
        user->set_password_hash(new_password_hash);
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::deactivate_user(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "UPDATE app_user SET is_active = false, updated_at = CURRENT_TIMESTAMP WHERE id = $1",
            user->id());
        
        txn.commit();
        
        // Обновляем объект пользователя
        user->set_active(false);
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

bool UserDAO::activate_user(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec_params(
            "UPDATE app_user SET is_active = true, updated_at = CURRENT_TIMESTAMP WHERE id = $1",
            user->id());
        
        txn.commit();
        
        // Обновляем объект пользователя
        user->set_active(true);
        return true;
    } catch (const std::exception& e) {
        // Логирование ошибки
        return false;
    }
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_by_name(const std::string& first_name, const std::string& last_name) {
    std::vector<std::shared_ptr<models::User>> users;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec_params(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user "
            "WHERE first_name ILIKE $1 AND last_name ILIKE $2 "
            "ORDER BY first_name, last_name",
            first_name + "%", last_name + "%");
        
        txn.commit();
        
        for (const auto& row : result) {
            auto user = std::make_shared<models::User>();
            user->from_row(row);
            users.push_back(user);
        }
    } catch (const std::exception& e) {
        // Логирование ошибки
    }
    
    return users;
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_users_requiring_password_change() {
    std::vector<std::shared_ptr<models::User>> users;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user "
            "WHERE password_change_required = true AND is_active = true "
            "ORDER BY created_at DESC");
        
        txn.commit();
        
        for (const auto& row : result) {
            auto user = std::make_shared<models::User>();
            user->from_row(row);
            users.push_back(user);
        }
    } catch (const std::exception& e) {
        // Логирование ошибки
    }
    
    return users;
}

std::string UserDAO::generate_uuid() {
    // Простая реализация генерации UUID v4
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    ss << std::hex;
    
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4"; // UUID version 4
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen); // UUID variant
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    
    return ss.str();
}

} // namespace dao