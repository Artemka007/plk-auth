#include "cli/AppState.hpp"
#include "models/user.hpp"

void AppState::set_current_user(const std::shared_ptr<const User> &user) { current_user_ = user; }

void AppState::set_running(bool running) { running_ = running; }

void AppState::add_to_history(std::string command) {
    constexpr size_t MAX_HISTORY = 100;
    if (history_.size() >= MAX_HISTORY) {
        history_.erase(history_.begin());
    }
    history_.push_back(std::move(command));
}

std::vector<std::string> AppState::get_history() const { return history_; }

std::shared_ptr<const User> AppState::get_current_user() const { return current_user_; }

bool AppState::is_authenticated() const { return current_user_ != nullptr; }

bool AppState::is_running() const { return running_; }

bool AppState::is_admin() const {
    return current_user_ != nullptr && current_user_->can_manage_users();
}