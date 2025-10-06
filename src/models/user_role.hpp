#pragma once
#include <string>
#include <vector>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include "enums.hpp"

namespace models {

#pragma db object table("user_role")
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
    void set_name(const std::string& name) { name_ = name; }
    void set_description(const std::string& description) { description_ = description; }
    void set_system(bool system) { is_system_ = system; }

private:
    friend class odb::access;
    
    #pragma db id type("VARCHAR(36)")
    std::string id_;
    
    #pragma db type("VARCHAR(50)") unique not_null
    std::string name_;
    
    #pragma db type("TEXT")
    std::string description_;
    
    #pragma db not_null
    bool is_system_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string created_at_;
    
    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string updated_at_;
};

} // namespace models