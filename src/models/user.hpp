#pragma once
#include <string>
#include <vector>
#include <memory>
#include <odb/core.hxx>
#include <odb/lazy-ptr.hxx>
#include <odb/nullable.hxx>

#pragma db model version(1, 1)

namespace models {

#pragma db object table("app_user")
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
    const odb::nullable<std::string>& patronymic() const { return patronymic_; }
    const std::string& email() const { return email_; }
    const odb::nullable<std::string>& phone() const { return phone_; }
    const std::string& password_hash() const { return password_hash_; }
    bool is_active() const { return is_active_; }
    bool is_password_change_required() const { return password_change_required_; }
    const std::string& created_at() const { return created_at_; }
    const std::string& updated_at() const { return updated_at_; }
    const odb::nullable<std::string>& last_login_at() const { return last_login_at_; }

    // Сеттеры
    void set_first_name(const std::string& name) { first_name_ = name; }
    void set_last_name(const std::string& name) { last_name_ = name; }
    void set_patronymic(const std::string& name) { patronymic_ = name; }
    void set_email(const std::string& email) { email_ = email; }
    void set_phone(const std::string& phone) { phone_ = phone; }
    void set_password_hash(const std::string& hash) {
        password_hash_ = hash;
        password_change_required_ = false;
    }
    void set_active(bool active) { is_active_ = active; }
    void require_password_change() { password_change_required_ = true; }
    void set_last_login_at(const std::string& timestamp) { last_login_at_ = timestamp; }

    // Вспомогательные методы
    std::string full_name() const {
        if (patronymic_.null()) {
            return first_name_ + " " + last_name_;
        }
        return first_name_ + " " + patronymic_.get() + " " + last_name_;
    }

private:
    friend class odb::access;

    #pragma db id type("VARCHAR(36)")
    std::string id_;

    #pragma db type("VARCHAR(100)") not_null
    std::string first_name_;

    #pragma db type("VARCHAR(100)") not_null
    std::string last_name_;

    #pragma db type("VARCHAR(100)")
    odb::nullable<std::string> patronymic_;

    #pragma db type("VARCHAR(255)") unique not_null
    std::string email_;

    #pragma db type("VARCHAR(20)")
    odb::nullable<std::string> phone_;

    #pragma db type("VARCHAR(255)") not_null
    std::string password_hash_;

    #pragma db not_null
    bool is_active_;

    #pragma db not_null
    bool password_change_required_;

    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string created_at_;

    #pragma db type("TIMESTAMP") default("CURRENT_TIMESTAMP")
    std::string updated_at_;

    #pragma db type("TIMESTAMP")
    odb::nullable<std::string> last_login_at_;
};

} // namespace models