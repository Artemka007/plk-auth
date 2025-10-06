#pragma once

#include <memory>
#include <string>
#include <vector>

namespace models {
class User;
}

class AppState {
public:
    // Setters
    void set_current_user(const std::shared_ptr<const models::User> &user);
    void set_running(bool running);
    void add_to_history(std::string command);

    // Getters
    std::vector<std::string> get_history() const;
    std::shared_ptr<const models::User> get_current_user() const;
    bool is_authenticated() const;
    bool is_running() const;
    bool is_admin() const;

private:
    std::shared_ptr<const models::User> current_user_ = nullptr;
    bool running_ = true;
    std::vector<std::string> history_;
};