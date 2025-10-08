#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "src/services/auth_service.hpp"
#include "src/services/log_service.hpp"
#include "src/services/user_service.hpp"

class BaseCommand;
class AppState;
class IOHandler;

class CommandFactory {
public:
    static std::vector<std::unique_ptr<BaseCommand>>
    create_all_commands(std::shared_ptr<AppState> app_state,
                        std::shared_ptr<IOHandler> io_handler,
                        std::shared_ptr<services::AuthService> auth_service,
                        std::shared_ptr<services::UserService> user_service,
                        std::shared_ptr<services::LogService> log_service);

private:
    CommandFactory() = delete;
};
