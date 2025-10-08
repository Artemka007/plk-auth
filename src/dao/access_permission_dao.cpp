#include "access_permission_dao.hpp"
#include <algorithm>
#include <random>
#include <sstream>
#include <iostream>
#include <pqxx/pqxx>
#include "src/utils/uuid_generator.hpp"
#include "src/models/user_role.hpp"
#include "src/models/user.hpp"

namespace dao {

AccessPermissionDAO::AccessPermissionDAO(std::shared_ptr<pqxx::connection> conn)
    : connection_(std::move(conn)) {
}

bool AccessPermissionDAO::save(const std::shared_ptr<models::AccessPermission>& permission) {
    try {
        if (permission->id().empty()) {
            permission->set_id(utils::UUIDGenerator::generate_uuid());
        }
        
        pqxx::work txn(*connection_);
        
        txn.exec(
            "INSERT INTO access_permission (id, name, description) "
            "VALUES (" + 
            txn.quote(permission->id()) + ", " +
            txn.quote(permission->name()) + ", " +
            txn.quote(permission->description()) + ") "
            "ON CONFLICT (id) DO UPDATE SET "
            "name = EXCLUDED.name, "
            "description = EXCLUDED.description"
        );
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::save: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<models::AccessPermission> AccessPermissionDAO::find_by_id(const std::string& id) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, name, description FROM access_permission WHERE id = " + txn.quote(id));
        
        txn.commit();
        
        if (result.empty()) {
            return nullptr;
        }
        
        return permission_from_row(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::find_by_id: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<models::AccessPermission> AccessPermissionDAO::find_by_name(const std::string& name) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, name, description FROM access_permission WHERE name = " + txn.quote(name));
        
        txn.commit();
        
        if (result.empty()) {
            return nullptr;
        }
        
        return permission_from_row(result[0]);
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::find_by_name: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<std::shared_ptr<models::AccessPermission>> AccessPermissionDAO::find_all() {
    std::vector<std::shared_ptr<models::AccessPermission>> permissions;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT id, name, description FROM access_permission ORDER BY name");
        
        txn.commit();
        
        for (const auto& row : result) {
            permissions.push_back(permission_from_row(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::find_all: " << e.what() << std::endl;
    }
    
    return permissions;
}

bool AccessPermissionDAO::remove(const std::string& id) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec("DELETE FROM role_permission WHERE permission_id = " + txn.quote(id));
        
        txn.exec("DELETE FROM access_permission WHERE id = " + txn.quote(id));
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::remove: " << e.what() << std::endl;
        return false;
    }
}

bool AccessPermissionDAO::assign_permission_to_role(const std::string& role_id, const std::string& permission_id) {
    try {
        pqxx::work txn(*connection_);
        auto existing = txn.exec(
            "SELECT COUNT(*) FROM role_permission WHERE role_id = " + txn.quote(role_id) + 
            " AND permission_id = " + txn.quote(permission_id));
        
        if (existing[0][0].as<int>() > 0) {
            return true; // Уже назначено
        }
        
        txn.exec(
            "INSERT INTO role_permission (role_id, permission_id) VALUES (" +
            txn.quote(role_id) + ", " + txn.quote(permission_id) + ")");
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::assign_permission_to_role: " << e.what() << std::endl;
        return false;
    }
}

bool AccessPermissionDAO::remove_permission_from_role(const std::string& role_id, const std::string& permission_id) {
    try {
        pqxx::work txn(*connection_);
        
        txn.exec(
            "DELETE FROM role_permission WHERE role_id = " + txn.quote(role_id) + 
            " AND permission_id = " + txn.quote(permission_id));
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::remove_permission_from_role: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::shared_ptr<models::AccessPermission>> AccessPermissionDAO::get_role_permissions(const std::string& role_id) {
    std::vector<std::shared_ptr<models::AccessPermission>> permissions;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT ap.id, ap.name, ap.description "
            "FROM access_permission ap "
            "INNER JOIN role_permission rp ON ap.id = rp.permission_id "
            "WHERE rp.role_id = " + txn.quote(role_id) + " "
            "ORDER BY ap.name");
        
        txn.commit();
        
        for (const auto& row : result) {
            permissions.push_back(permission_from_row(row));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::get_role_permissions: " << e.what() << std::endl;
    }
    
    return permissions;
}

bool AccessPermissionDAO::role_has_permission(const std::string& role_id, const std::string& permission_name) {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT COUNT(*) FROM role_permission rp "
            "INNER JOIN access_permission ap ON rp.permission_id = ap.id "
            "WHERE rp.role_id = " + txn.quote(role_id) + 
            " AND ap.name = " + txn.quote(permission_name));
        
        txn.commit();
        
        return result[0][0].as<int>() > 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::role_has_permission: " << e.what() << std::endl;
        return false;
    }
}

void AccessPermissionDAO::initialize_system_permissions() {
    try {
        std::vector<std::pair<std::string, std::string>> system_permissions = {
            {"USER_CREATE", "Create new users"},
            {"USER_READ", "View users"},
            {"USER_UPDATE", "Update users"},
            {"USER_DELETE", "Delete users"},
            {"USER_CHANGE_ROLE", "Change user roles"},
            
            {"ROLE_CREATE", "Create roles"},
            {"ROLE_READ", "View roles"},
            {"ROLE_UPDATE", "Update roles"},
            {"ROLE_DELETE", "Delete roles"},
            
            {"LOG_READ", "View system logs"},
            {"LOG_EXPORT", "Export logs"},
            {"LOG_DELETE", "Delete logs"},
            
            {"SYSTEM_BACKUP", "Create system backups"},
            {"SYSTEM_RESTORE", "Restore system from backup"},
            {"SYSTEM_EXPORT", "Export system data"},
            {"SYSTEM_IMPORT", "Import system data"}
        };
        
        pqxx::work txn(*connection_);
        
        for (const auto& [name, description] : system_permissions) {
            auto permission_id = utils::UUIDGenerator::generate_uuid();
            
            txn.exec(
                "INSERT INTO access_permission (id, name, description) "
                "VALUES (" + 
                txn.quote(permission_id) + ", " +
                txn.quote(name) + ", " +
                txn.quote(description) + ") "
                "ON CONFLICT (name) DO NOTHING"
            );
        }
        
        txn.exec(
            "INSERT INTO user_role (id, name, description, is_system) VALUES "
            "('role-admin', 'ADMIN', 'System Administrator', true), "
            "('role-user', 'USER', 'Regular User', true) "
            "ON CONFLICT (id) DO NOTHING"
        );
        
        for (const auto& [name, description] : system_permissions) {
            txn.exec(
                "INSERT INTO role_permission (role_id, permission_id) "
                "SELECT 'role-admin', id FROM access_permission WHERE name = " + txn.quote(name) + " "
                "ON CONFLICT (role_id, permission_id) DO NOTHING"
            );
        }
        
        std::vector<std::string> user_permissions = {
            "USER_READ", "ROLE_READ", "LOG_READ"
        };
        
        for (const auto& permission_name : user_permissions) {
            txn.exec(
                "INSERT INTO role_permission (role_id, permission_id) "
                "SELECT 'role-user', id FROM access_permission WHERE name = " + txn.quote(permission_name) + " "
                "ON CONFLICT (role_id, permission_id) DO NOTHING"
            );
        }
        
        txn.commit();
        
        std::cout << "✅ System permissions initialized successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error initializing system permissions: " << e.what() << std::endl;
        throw;
    }
}

std::vector<std::shared_ptr<models::UserRole>> AccessPermissionDAO::get_roles_with_permission(const std::string& permission_name) {
    std::vector<std::shared_ptr<models::UserRole>> roles;
    
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec(
            "SELECT ur.id, ur.name, ur.description, ur.is_system, ur.created_at, ur.updated_at "
            "FROM user_role ur "
            "INNER JOIN role_permission rp ON ur.id = rp.role_id "
            "INNER JOIN access_permission ap ON rp.permission_id = ap.id "
            "WHERE ap.name = " + txn.quote(permission_name) + " "
            "ORDER BY ur.name");
        
        txn.commit();
        
        for (const auto& row : result) {
            auto role = std::make_shared<models::UserRole>();
            role->set_id(row["id"].as<std::string>());
            role->set_name(row["name"].as<std::string>());
            role->set_description(row["description"].as<std::string>());
            role->set_system(row["is_system"].as<bool>());
            // Можно добавить остальные поля при необходимости
            roles.push_back(role);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in AccessPermissionDAO::get_roles_with_permission: " << e.what() << std::endl;
    }
    
    return roles;
}

// Вспомогательные методы
std::shared_ptr<models::AccessPermission> AccessPermissionDAO::permission_from_row(const pqxx::row& row) {
    auto permission = std::make_shared<models::AccessPermission>();
    permission->set_id(row["id"].as<std::string>());
    permission->set_name(row["name"].as<std::string>());
    permission->set_description(row["description"].as<std::string>());
    return permission;
}
} // namespace dao