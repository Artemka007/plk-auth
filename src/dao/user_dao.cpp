#include "user_dao.hpp"
#include <algorithm>
#include <random>
#include <sstream>
#include <iostream>
#include "../utils/uuid_generator.hpp"

namespace dao {

UserDAO::UserDAO(std::shared_ptr<pqxx::connection> conn)
    : connection_(std::move(conn)) {
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
        std::cerr << "Error in find_all: " << e.what() << std::endl;
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
        std::cerr << "Error in find_users_requiring_password_change: " << e.what() << std::endl;
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
        std::cerr << "Error in find_active_users: " << e.what() << std::endl;
    }

    return users;
}

std::shared_ptr<models::User> UserDAO::find_by_id(const std::string& id) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user WHERE id = " + txn.quote(id));

        txn.commit();

        if (result.empty()) {
            return nullptr;
        }

        auto user = std::make_shared<models::User>();
        user->from_row(result[0]);
        return user;
    } catch (const std::exception& e) {
        std::cerr << "Error in find_by_id: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<models::User> UserDAO::find_by_email(const std::string& email) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user WHERE email = " + txn.quote(email));

        txn.commit();

        if (result.empty()) {
            return nullptr;
        }

        auto user = std::make_shared<models::User>();
        user->from_row(result[0]);
        return user;
    } catch (const std::exception& e) {
        std::cerr << "Error in find_by_email: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<models::User> UserDAO::find_by_credentials(const std::string& email, const std::string& password_hash) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user "
            "WHERE email = " + txn.quote(email) +
            " AND password_hash = " + txn.quote(password_hash) +
            " AND is_active = true");

        txn.commit();

        if (result.empty()) {
            return nullptr;
        }

        auto user = std::make_shared<models::User>();
        user->from_row(result[0]);
        return user;
    } catch (const std::exception& e) {
        std::cerr << "Error in find_by_credentials: " << e.what() << std::endl;
        return nullptr;
    }
}

bool UserDAO::save(const std::shared_ptr<models::User>& user) {
    try {
        if (user->id().empty()) {
            user->set_id(utils::UUIDGenerator::generate_uuid());
        }

        pqxx::work txn(*connection_);

        std::string patronymic = user->patronymic().value_or("");
        std::string phone = user->phone().value_or("");

        txn.exec(
            "INSERT INTO app_user (id, first_name, last_name, patronymic, email, "
            "phone, password_hash, is_active, password_change_required) "
            "VALUES (" +
            txn.quote(user->id()) + ", " +
            txn.quote(user->first_name()) + ", " +
            txn.quote(user->last_name()) + ", " +
            txn.quote(patronymic) + ", " +
            txn.quote(user->email()) + ", " +
            txn.quote(phone) + ", " +
            txn.quote(user->password_hash()) + ", " +
            txn.quote(user->is_active()) + ", " +
            txn.quote(user->is_password_change_required()) + ")");

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in save: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::update(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);

        std::string patronymic = user->patronymic().value_or("");
        std::string phone = user->phone().value_or("");

        txn.exec(
            "UPDATE app_user SET first_name = " + txn.quote(user->first_name()) +
            ", last_name = " + txn.quote(user->last_name()) +
            ", patronymic = " + txn.quote(patronymic) +
            ", email = " + txn.quote(user->email()) +
            ", phone = " + txn.quote(phone) +
            ", password_hash = " + txn.quote(user->password_hash()) +
            ", is_active = " + txn.quote(user->is_active()) +
            ", password_change_required = " + txn.quote(user->is_password_change_required()) +
            ", updated_at = CURRENT_TIMESTAMP " +
            "WHERE id = " + txn.quote(user->id()));

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in update: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::delete_by_id(const std::string& id) {
    try {
        pqxx::work txn(*connection_);

        // Сначала удаляем связи с ролями
        txn.exec("DELETE FROM user_role_assignment WHERE user_id = " + txn.quote(id));

        // Затем удаляем пользователя
        txn.exec("DELETE FROM app_user WHERE id = " + txn.quote(id));

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in delete_by_id: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<models::UserRole>> UserDAO::user_roles(const std::shared_ptr<models::User>& user) {
    std::vector<std::shared_ptr<models::UserRole>> roles;

    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT ur.id, ur.name, ur.description, ur.is_system, ur.created_at, ur.updated_at "
            "FROM user_role ur "
            "INNER JOIN user_role_assignment ura ON ur.id = ura.role_id "
            "WHERE ura.user_id = " + txn.quote(user->id()));

        txn.commit();

        for (const auto& row : result) {
            auto role = std::make_shared<models::UserRole>();
            role->from_row(row);
            roles.push_back(role);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in user_roles: " << e.what() << std::endl;
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

        txn.exec(
            "INSERT INTO user_role_assignment (user_id, role_id) VALUES (" +
            txn.quote(user->id()) + ", " + txn.quote(role->id()) + ")");

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in assign_role: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::remove_role(const std::shared_ptr<models::User>& user, const std::shared_ptr<models::UserRole>& role) {
    try {
        pqxx::work txn(*connection_);

        txn.exec(
            "DELETE FROM user_role_assignment WHERE user_id = " + txn.quote(user->id()) +
            " AND role_id = " + txn.quote(role->id()));

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in remove_role: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::has_role(const std::shared_ptr<models::User>& user, const std::string& role_name) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT COUNT(*) FROM user_role_assignment ura "
            "INNER JOIN user_role ur ON ura.role_id = ur.id "
            "WHERE ura.user_id = " + txn.quote(user->id()) +
            " AND ur.name = " + txn.quote(role_name));

        txn.commit();

        return result[0][0].as<int>() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in has_role: " << e.what() << std::endl;
        return false;
    }
}



bool UserDAO::update_last_login(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);

        txn.exec(
            "UPDATE app_user SET last_login_at = CURRENT_TIMESTAMP WHERE id = " + txn.quote(user->id()));

        txn.commit();

        // Обновляем объект пользователя
        user->set_last_login_at("CURRENT_TIMESTAMP");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in update_last_login: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::change_password(const std::shared_ptr<models::User>& user, const std::string& new_password_hash) {
    try {
        pqxx::work txn(*connection_);

        txn.exec(
            "UPDATE app_user SET password_hash = " + txn.quote(new_password_hash) +
            ", password_change_required = false, updated_at = CURRENT_TIMESTAMP WHERE id = " + txn.quote(user->id()));

        txn.commit();

        // Обновляем объект пользователя
        user->set_password_hash(new_password_hash);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in change_password: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::deactivate_user(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);

        txn.exec(
            "UPDATE app_user SET is_active = false, updated_at = CURRENT_TIMESTAMP WHERE id = " + txn.quote(user->id()));

        txn.commit();

        // Обновляем объект пользователя
        user->set_active(false);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in deactivate_user: " << e.what() << std::endl;
        return false;
    }
}

bool UserDAO::activate_user(const std::shared_ptr<models::User>& user) {
    try {
        pqxx::work txn(*connection_);

        txn.exec(
            "UPDATE app_user SET is_active = true, updated_at = CURRENT_TIMESTAMP WHERE id = " + txn.quote(user->id()));

        txn.commit();

        // Обновляем объект пользователя
        user->set_active(true);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in activate_user: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_by_name(const std::string& first_name, const std::string& last_name) {
    std::vector<std::shared_ptr<models::User>> users;

    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, "
            "updated_at, last_login_at FROM app_user "
            "WHERE first_name ILIKE " + txn.quote(first_name + "%") +
            " AND last_name ILIKE " + txn.quote(last_name + "%") +
            " ORDER BY first_name, last_name");

        txn.commit();

        for (const auto& row : result) {
            auto user = std::make_shared<models::User>();
            user->from_row(row);
            users.push_back(user);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in find_by_name: " << e.what() << std::endl;
    }

    return users;
}

std::shared_ptr<models::UserRole> UserDAO::get_role_by_name(const std::string& role_name) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, name, description, is_system, created_at, updated_at "
            "FROM user_role WHERE name = " + txn.quote(role_name)
        );

        txn.commit();

        if (result.empty()) {
            return nullptr;
        }

        auto role = std::make_shared<models::UserRole>();
        role->from_row(result[0]);
        return role;

    } catch (const std::exception& e) {
        std::cerr << "Error in get_role_by_name: " << e.what() << std::endl;
        return nullptr;
    }
}

} // namespace dao