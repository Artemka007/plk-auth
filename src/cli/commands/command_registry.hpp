#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "base_command.hpp"
#include "system/help_command.hpp"

class AppState;
class IOHandler;

namespace services {
class AuthService;
class UserService;
class LogService;
}

using CommandFactoryFn = std::function<std::unique_ptr<BaseCommand>(
    std::shared_ptr<AppState>, std::shared_ptr<IOHandler>,
    std::shared_ptr<services::AuthService>, std::shared_ptr<services::UserService>,
    std::shared_ptr<services::LogService>)>;

class CommandRegistry {
public:
    // Register a command by name
    static void register_command(const std::string &name, CommandFactoryFn factory) {
        get_registry()[name] = factory;
    }

    // Create all registered commands
    static std::vector<std::unique_ptr<BaseCommand>> create_all_commands(
        std::shared_ptr<AppState> app_state,
        std::shared_ptr<IOHandler> io_handler,
        std::shared_ptr<services::AuthService> auth_service,
        std::shared_ptr<services::UserService> user_service,
        std::shared_ptr<services::LogService> log_service)
    {
        std::vector<std::unique_ptr<BaseCommand>> commands;
        for (auto &[name, factory] : get_registry()) {
            commands.push_back(factory(app_state, io_handler, auth_service, user_service, log_service));
        }

        // Automatically populate HelpCommand
        for (auto &cmd : commands) {
            if (auto help_cmd = dynamic_cast<HelpCommand*>(cmd.get())) {
                std::unordered_map<std::string, BaseCommand*> command_map;
                for (auto &c : commands) command_map[c->get_name()] = c.get();
                help_cmd->set_available_commands(command_map);
                break;
            }
        }

        return commands;
    }

private:
    static std::unordered_map<std::string, CommandFactoryFn>& get_registry() {
        static std::unordered_map<std::string, CommandFactoryFn> registry;
        return registry;
    }
};
