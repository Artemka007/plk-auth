#include "cli/commands/system/help_command.hpp"
#include "cli/io_handler.hpp"

HelpCommand::HelpCommand(std::string name,
                         std::string description,
                         std::shared_ptr<AppState> app_state,
                         std::shared_ptr<IOHandler> io_handler,
                         std::shared_ptr<AuthService> auth_service,
                         std::shared_ptr<UserService> user_service,
                         std::shared_ptr<LogService> log_service)
    : BaseCommand(std::move(name),
                  std::move(description),
                  std::move(app_state),
                  std::move(io_handler),
                  std::move(auth_service),
                  std::move(user_service),
                  std::move(log_service)) {}

bool HelpCommand::execute(const std::vector<std::string> &args) {
    if (args.empty()) {
        io_handler_->println("Available commands:");
        for (const auto &[name, cmd] : available_commands_) {
            if (cmd->isVisible()) {
                io_handler_->println("  " + name + " - " + cmd->get_description());
            }
        }
    } else {
        const std::string &target = args[0];
        auto it = available_commands_.find(target);
        if (it != available_commands_.end() && it->second->isVisible()) {
            io_handler_->println(target + ": " + it->second->get_description());
        } else {
            io_handler_->error("Unknown command: " + target);
            return false;
        }
    }
    return true;
}

void HelpCommand::set_available_commands(std::unordered_map<std::string, BaseCommand *> &map) {
    available_commands_ = map;
}
