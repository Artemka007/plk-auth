#include "user_dao.hpp"
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/query.hxx>
#include "../models/odb-2.5.0/user-odb.hpp"
#include "../models/odb-2.5.0/user_role-odb.hpp"
#include "../models/odb-2.5.0/user_role_assignment-odb.hpp"

namespace dao {

using UserResult = odb::result<models::User>;

UserDAO::UserDAO(std::shared_ptr<odb::database> db) 
    : database_(std::move(db)) {}

std::shared_ptr<models::User> UserDAO::find_by_id(const std::string& id) {
    odb::transaction t(database_->begin());

    try {
        typedef odb::query<models::User> query;
        UserResult result = database_->query<models::User>(query::id == id);
        if (result.begin() != result.end()) {
            return std::make_shared<models::User>(*result.begin()); 
        }
    } catch (const odb::object_not_persistent&) {}

    t.commit();
    return nullptr;
}

std::shared_ptr<models::User> UserDAO::find_by_email(const std::string& email) {
    odb::transaction t(database_->begin());
    
    try {
        typedef odb::query<models::User> query;
        UserResult result = database_->query<models::User>(query::email == email);
        
        if (!result.empty()) {
            t.commit();
            return std::make_shared<models::User>(*result.begin());
        }
    } catch (const std::exception& e) {}

    t.commit();
    return nullptr;
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_all() {
    odb::transaction t(database_->begin());
    
    UserResult result = database_->query<models::User>();
    std::vector<std::shared_ptr<models::User>> users;
    
    for (auto it = result.begin(); it != result.end(); ++it) {
        users.push_back(std::make_shared<models::User>(*it));
    }
    
    t.commit();
    return users;
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_active_users() {
    odb::transaction t(database_->begin());
    std::vector<std::shared_ptr<models::User>> users;
    
    try {
        typedef odb::query<models::User> query;
        UserResult result = database_->query<models::User>(query::is_active == true);
        
        for (auto it = result.begin(); it != result.end(); ++it) {
            users.push_back(std::make_shared<models::User>(*it));
        }
        
    } catch (const std::exception& e) { }
        
    t.commit();
    return users;
}

bool UserDAO::save(const std::shared_ptr<models::User>& user) {
    try {
        odb::transaction t(database_->begin());
        database_->persist(user);
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool UserDAO::update(const std::shared_ptr<models::User>& user) {
    try {
        odb::transaction t(database_->begin());
        database_->update(user);
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool UserDAO::remove(const std::shared_ptr<models::User>& user) {
    try {
        odb::transaction t(database_->begin());
        database_->erase(user);
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool UserDAO::delete_by_id(const std::string& id) {
    try {
        odb::transaction t(database_->begin());
        database_->erase<models::User>(id);
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::shared_ptr<models::UserRole>> UserDAO::user_roles(
    const std::shared_ptr<models::User>& user) {
    
    odb::transaction t(database_->begin());
    
    try {
        typedef odb::query<models::UserRoleAssignment> query;
        odb::result<models::UserRoleAssignment> result = 
            database_->query<models::UserRoleAssignment>(query::user == user->id());
        
        std::vector<std::shared_ptr<models::UserRole>> roles;
        for (auto it = result.begin(); it != result.end(); ++it) {
            roles.push_back((*it).role());
        }
        
        t.commit();
        return roles;
        
    } catch (const std::exception& e) {
        t.commit();
        return {};
    }
}

bool UserDAO::assign_role(
    const std::shared_ptr<models::User>& user,
    const std::shared_ptr<models::UserRole>& role) {
    
    try {
        odb::transaction t(database_->begin());
        
        // Проверяем, нет ли уже такой роли
        typedef odb::query<models::UserRoleAssignment> query;
        auto existing = database_->query<models::UserRoleAssignment>(
            query::user == user->id() && query::role == role->id()
        );
        
        if (existing.size() > 0) {
            t.commit();
            return true; // Роль уже назначена
        }
        
        // Назначаем роль
        auto assignment = std::make_shared<models::UserRoleAssignment>(user, role);
        database_->persist(assignment);
        
        t.commit();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool UserDAO::remove_role(
    const std::shared_ptr<models::User>& user,
    const std::shared_ptr<models::UserRole>& role) {
    
    try {
        odb::transaction t(database_->begin());
        
        typedef odb::query<models::UserRoleAssignment> query;
        database_->erase_query<models::UserRoleAssignment>(
            query::user == user->id() && query::role == role->id()
        );
        
        t.commit();
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool UserDAO::has_role(
    const std::shared_ptr<models::User>& user,
    const std::string& role_name) {
    
    auto roles = user_roles(user);
    for (const auto& role : roles) {
        if (role->name() == role_name) {
            return true;
        }
    }
    return false;
}

bool UserDAO::update_last_login(const std::shared_ptr<models::User>& user) {
    // В ODB лучше использовать timestamp через SQL
    try {
        odb::transaction t(database_->begin());
        database_->execute("UPDATE app_user SET last_login_at = CURRENT_TIMESTAMP WHERE id = '" + user->id() + "'");
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool UserDAO::change_password(
    const std::shared_ptr<models::User>& user,
    const std::string& new_password_hash) {
    
    user->set_password_hash(new_password_hash);
    user->require_password_change();
    return update(user);
}

bool UserDAO::deactivate_user(const std::shared_ptr<models::User>& user) {
    user->set_active(false);
    return update(user);
}

bool UserDAO::activate_user(const std::shared_ptr<models::User>& user) {
    user->set_active(true);
    return update(user);
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_by_name(
    const std::string& first_name, 
    const std::string& last_name) {
    
    odb::transaction t(database_->begin());
    
    try {
        typedef odb::query<models::User> query;
        UserResult result = database_->query<models::User>(
            query::first_name == first_name && query::last_name == last_name
        );
        
        std::vector<std::shared_ptr<models::User>> users;
        for (auto it = result.begin(); it != result.end(); ++it) {
            users.push_back(std::make_shared<models::User>(*it));
        }
        
        t.commit();
        return users;
        
    } catch (const std::exception& e) {
        t.commit();
        return {};
    }
}

std::vector<std::shared_ptr<models::User>> UserDAO::find_users_requiring_password_change() {
    odb::transaction t(database_->begin());
    
    try {
        typedef odb::query<models::User> query;
        UserResult result = database_->query<models::User>(
            query::password_change_required == true && query::is_active == true
        );
        
        std::vector<std::shared_ptr<models::User>> users;
        for (auto it = result.begin(); it != result.end(); ++it) {
            users.push_back(std::make_shared<models::User>(*it));
        }
        
        t.commit();
        return users;
        
    } catch (const std::exception& e) {
        t.commit();
        return {};
    }
}

}