#pragma once
#include <string>
#include <memory>
#include <pqxx/pqxx>
#include <vector>

namespace models {

class UserRoleAssignment {
public:
    UserRoleAssignment() = default;
    
    UserRoleAssignment(
        const std::string& user_id,
        const std::string& role_id)
        : user_id_(user_id)
        , role_id_(role_id) {
    }
    
    const std::string& id() const { return id_; }
    const std::string& user_id() const { return user_id_; }
    const std::string& role_id() const { return role_id_; }
    const std::string& assigned_at() const { return assigned_at_; }

    void set_user_id(const std::string& user_id) { user_id_ = user_id; }
    void set_role_id(const std::string& role_id) { role_id_ = role_id; }

    std::vector<std::string> get_insert_values() const {
        return {user_id_, role_id_};
    }

    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        user_id_ = row["user_id"].as<std::string>();
        role_id_ = row["role_id"].as<std::string>();
        assigned_at_ = row["assigned_at"].as<std::string>();
    }

private:
    std::string id_;
    std::string user_id_;
    std::string role_id_;
    std::string assigned_at_;
};

}