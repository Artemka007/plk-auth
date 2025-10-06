#pragma once

#include "cli/commands/BaseCommand.hpp"
#include <unordered_map>

class HelpCommand : public BaseCommand {
public:
    HelpCommand(std::string name,
                std::string description,
                std::shared_ptr<AppState> app_state,
                std::shared_ptr<IOHandler> io_handler,
                std::shared_ptr<AuthService> auth_service,
                std::shared_ptr<UserService> user_service,
                std::shared_ptr<LogService> log_service);

    bool execute(const std::vector<std::string> &args) override;
    bool isVisible() const override { return true; }
    void set_available_commands(std::unordered_map<std::string, BaseCommand *> &map);

private:
    std::unordered_map<std::string, BaseCommand *> available_commands_ = {};
};