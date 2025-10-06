#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class BaseCommand;
class AppState;
class IOHandler;
class AuthService;
class UserService;
class LogService;

class CommandFactory {
public:
    static std::vector<std::unique_ptr<BaseCommand>>
    create_all_commands(std::shared_ptr<AppState> app_state,
                        std::shared_ptr<IOHandler> io_handler,
                        std::shared_ptr<AuthService> auth_service,
                        std::shared_ptr<UserService> user_service,
                        std::shared_ptr<LogService> log_service);

private:
    CommandFactory() = delete;
};
