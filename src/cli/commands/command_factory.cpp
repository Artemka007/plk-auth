#include "command_factory.hpp"
#include "base_command.hpp"

// System commands
#include "system/exit_command.hpp"
#include "system/help_command.hpp"
#include "system/whoami_command.hpp"

std::vector<std::unique_ptr<BaseCommand>> CommandFactory::create_all_commands(
    std::shared_ptr<AppState> app_state, std::shared_ptr<IOHandler> io_handler,
    std::shared_ptr<services::AuthService> auth_service,
    std::shared_ptr<services::UserService> user_service,
    std::shared_ptr<services::LogService> log_service) {
    std::vector<std::unique_ptr<BaseCommand>> commands;

    // Help command
    auto help_cmd = std::make_unique<HelpCommand>(
        "help", "Show help", app_state, io_handler, auth_service, user_service,
        log_service);
    BaseCommand *help_ptr = help_cmd.get();
    commands.push_back(std::move(help_cmd));

    // Exit command
    commands.push_back(std::make_unique<ExitCommand>(
        "exit", "Exit the application", app_state, io_handler, auth_service,
        user_service, log_service));

    // WhoAmI command
    commands.push_back(std::make_unique<WhoAmICommand>(
        "whoami", "Print short information about yourself", app_state,
        io_handler, auth_service, user_service, log_service));

    // Build map for HelpCommand
    std::unordered_map<std::string, BaseCommand *> command_map;
    for (const auto &cmd : commands) {
        command_map[cmd->get_name()] = cmd.get();
    }

    static_cast<HelpCommand *>(help_ptr)->set_available_commands(command_map);

    return commands;
}
