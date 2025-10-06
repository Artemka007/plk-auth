#pragma once

#include "AppState.hpp"
#include "commands/BaseCommand.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

class AuthService;
class UserService;
class LogService;

class CliApp {
public:
    CliApp(std::shared_ptr<UserService> user_service,
           std::shared_ptr<AuthService> auth_service,
           std::shared_ptr<LogService> log_service);

    void Run();
    void Stop();

private:
    std::shared_ptr<UserService> user_service_;
    std::shared_ptr<AuthService> auth_service_;
    std::shared_ptr<LogService> log_service_;

    std::shared_ptr<AppState> app_state_;

    std::vector<std::unique_ptr<BaseCommand>> commands_;
    std::unordered_map<std::string, BaseCommand *> command_map_;

    void initialize_commands();
    void update_command_current_user();
};
