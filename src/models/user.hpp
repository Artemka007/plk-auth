#pragma once
#include <string>
#include <optional>
#include <pqxx/pqxx>

namespace models {

class User {
public:
    User() = default;

    User(std::string first_name, std::string last_name, std::string email)
        : first_name_(std::move(first_name))
        , last_name_(std::move(last_name))
        , email_(std::move(email))
        , is_active_(true)
        , password_change_required_(true) {
    }

    // Геттеры
    const std::string& id() const { return id_; }
    const std::string& first_name() const { return first_name_; }
    const std::string& last_name() const { return last_name_; }
    const std::optional<std::string>& patronymic() const { return patronymic_; }
    const std::string& email() const { return email_; }
    const std::optional<std::string>& phone() const { return phone_; }
    const std::string& password_hash() const { return password_hash_; }
    bool is_active() const { return is_active_; }
    bool is_password_change_required() const { return password_change_required_; }
    const std::string& created_at() const { return created_at_; }
    const std::string& updated_at() const { return updated_at_; }
    const std::optional<std::string>& last_login_at() const { return last_login_at_; }
    
    // Сеттеры
    void set_id(const std::string& id) { id_ = id; }
    void set_first_name(const std::string& name) { first_name_ = name; }
    void set_last_name(const std::string& name) { last_name_ = name; }
    void set_patronymic(const std::optional<std::string>& name) { patronymic_ = name; }
    void set_email(const std::string& email) { email_ = email; }
    void set_phone(const std::optional<std::string>& phone) { phone_ = phone; }
    void set_password_hash(const std::string& hash) { 
        password_hash_ = hash; 
        password_change_required_ = false;
    }
    void set_active(bool active) { is_active_ = active; }
    void require_password_change() { password_change_required_ = true; }
    void set_last_login_at(const std::optional<std::string>& timestamp) { last_login_at_ = timestamp; }

    // Вспомогательные методы
    std::string full_name() const {
        if (!patronymic_.has_value()) {
            return first_name_ + " " + last_name_;
        }
        return first_name_ + " " + patronymic_.value() + " " + last_name_;
    }

    // Сериализация для вставки
    std::vector<std::string> get_insert_values() const {
        return {
            id_, first_name_, last_name_, 
            patronymic_.value_or("NULL"), 
            email_, 
            phone_.value_or("NULL"), 
            password_hash_,
            is_active_ ? "true" : "false",
            password_change_required_ ? "true" : "false"
        };
    }

    // Десериализация из pqxx
    void from_row(const pqxx::row& row) {
        id_ = row["id"].as<std::string>();
        first_name_ = row["first_name"].as<std::string>();
        last_name_ = row["last_name"].as<std::string>();
        
        if (!row["patronymic"].is_null()) {
            patronymic_ = row["patronymic"].as<std::string>();
        }
        
        email_ = row["email"].as<std::string>();
        
        if (!row["phone"].is_null()) {
            phone_ = row["phone"].as<std::string>();
        }
        
        password_hash_ = row["password_hash"].as<std::string>();
        is_active_ = row["is_active"].as<bool>();
        password_change_required_ = row["password_change_required"].as<bool>();
        created_at_ = row["created_at"].as<std::string>();
        updated_at_ = row["updated_at"].as<std::string>();
        
        if (!row["last_login_at"].is_null()) {
            last_login_at_ = row["last_login_at"].as<std::string>();
        }
    }

private:
    std::string id_;
    std::string first_name_;
    std::string last_name_;
    std::optional<std::string> patronymic_;
    std::string email_;
    std::optional<std::string> phone_;
    std::string password_hash_;
    bool is_active_;
    bool password_change_required_;
    std::string created_at_;
    std::string updated_at_;
    std::optional<std::string> last_login_at_;
};

} // namespace models