#pragma once
#include <string>
#include <pqxx/pqxx>

namespace models {

class UserRole {
public:
    UserRole() = default;
    
    UserRole(std::string name, std::string description)
        : name_(std::move(name))
        , description_(std::move(description))
        , is_system_(false) {
    }
    
    // Геттеры
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    bool is_system() const { return is_system_; }
    const std::string& created_at() const { return created_at_; }
    const std::string& updated_at() const { return updated_at_; }
    
    // Сеттеры
    void set_id(const std::string& id) { id_ = id; }
    void set_name(const std::string& name) { name_ = name; }
    void set_description(const std::string& description) { description_ = description; }
    void set_system(bool system) { is_system_ = system; }

    // Сериализация для вставки
    std::vector<std::string> get_insert_values() const {
        return {id_, name_, description_, is_system_ ? "true" : "false"};
    }

    // Десериализация из pqxx
    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        name_ = row["name"].as<std::string>();
        description_ = row["description"].as<std::string>();
        is_system_ = row["is_system"].as<bool>();
        created_at_ = row["created_at"].as<std::string>();
        updated_at_ = row["updated_at"].as<std::string>();
    }

private:
    std::string id_;
    std::string name_;
    std::string description_;
    bool is_system_;
    std::string created_at_;
    std::string updated_at_;
};

} // namespace models