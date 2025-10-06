#pragma once

#include "cli/AppState.hpp"
#include "cli/IOHandler.hpp"
#include "cli/commands/BaseCommand.hpp"
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
           std::shared_ptr<LogService> log_service,
           std::shared_ptr<IOHandler> io_handler);

    void Run();
    void Stop();

private:
    // Domain dependencies
    std::shared_ptr<UserService> user_service_;
    std::shared_ptr<AuthService> auth_service_;
    std::shared_ptr<LogService> log_service_;

    // Cli-specific dependencies
    std::shared_ptr<IOHandler> io_handler_;
    std::shared_ptr<AppState> app_state_;

    // Commands
    std::vector<std::unique_ptr<BaseCommand>> commands_;
    std::unordered_map<std::string, BaseCommand *> command_map_;

    void initialize_commands();
    void execute_command(const std::string &input);
};
