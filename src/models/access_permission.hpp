#pragma once
#include <string>

namespace models {

class AccessPermission {
public:
    AccessPermission() = default;
    
    AccessPermission(std::string name, std::string description)
        : name_(std::move(name))
        , description_(std::move(description)) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }

    // Сеттеры
    void set_id(const std::string& id) { id_ = id; }
    void set_name(const std::string& name) { name_ = name; }
    void set_description(const std::string& description) { description_ = description; }

    // Сериализация для вставки
    std::vector<std::string> get_insert_values() const {
        return {id_, name_, description_};
    }

    // Десериализация из pqxx
    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        name_ = row["name"].as<std::string>();
        description_ = row["description"].as<std::string>();
    }

private:
    std::string id_;
    std::string name_;
    std::string description_;
};

class RolePermission {
public:
    RolePermission() = default;
    
    RolePermission(
        const std::string& role_id,
        const std::string& permission_id)
        : role_id_(role_id)
        , permission_id_(permission_id) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    const std::string& role_id() const { return role_id_; }
    const std::string& permission_id() const { return permission_id_; }
    const std::string& granted_at() const { return granted_at_; }

    // Сеттеры
    void set_role_id(const std::string& role_id) { role_id_ = role_id; }
    void set_permission_id(const std::string& permission_id) { permission_id_ = permission_id; }

    // Сериализация для вставки
    std::vector<std::string> get_insert_values() const {
        return {role_id_, permission_id_};
    }

    // Десериализация из pqxx
    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        role_id_ = row["role_id"].as<std::string>();
        permission_id_ = row["permission_id"].as<std::string>();
        granted_at_ = row["granted_at"].as<std::string>();
    }

private:
    std::string id_;
    std::string role_id_;
    std::string permission_id_;
    std::string granted_at_;
};

} // namespace models