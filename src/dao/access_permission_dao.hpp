#pragma once
#include <memory>
#include <vector>
#include <pqxx/pqxx>
#include "src/models/access_permission.hpp"
#include "src/models/user_role.hpp"
#include "src/models/user.hpp"

namespace dao {

class AccessPermissionDAO {
public:
    explicit AccessPermissionDAO(std::shared_ptr<pqxx::connection> conn);
    
    bool save(const std::shared_ptr<models::AccessPermission>& permission);
    std::shared_ptr<models::AccessPermission> find_by_id(const std::string& id);
    std::shared_ptr<models::AccessPermission> find_by_name(const std::string& name);
    std::vector<std::shared_ptr<models::AccessPermission>> find_all();
    bool remove(const std::string& id);
    
    bool assign_permission_to_role(const std::string& role_id, const std::string& permission_id);
    bool remove_permission_from_role(const std::string& role_id, const std::string& permission_id);
    std::vector<std::shared_ptr<models::AccessPermission>> get_role_permissions(const std::string& role_id);
    bool role_has_permission(const std::string& role_id, const std::string& permission_name);
    std::vector<std::shared_ptr<models::UserRole>> get_roles_with_permission(const std::string& permission_name);
    
    void initialize_system_permissions();

private:
    std::shared_ptr<pqxx::connection> connection_;
    std::shared_ptr<models::AccessPermission> permission_from_row(const pqxx::row& row);
};

} // namespace dao