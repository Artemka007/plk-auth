#include "cli/commands/system/help_command.hpp"
#include "cli/io_handler.hpp"

bool HelpCommand::execute(const std::vector<std::string> &args) {
    if (args.size() > 1) {
        io_handler_->error("Usage: help [command]");
        return false;
    }

    if (args.empty()) {
        io_handler_->println("Available commands:");
        for (const auto &[name, cmd] : available_commands_) {
            if (cmd->isVisible()) {
                io_handler_->println("  " + name + " - " +
                                     cmd->get_description());
            }
        }
        return true;
    }

    const std::string &target = args[0];
    auto it = available_commands_.find(target);
    if (it == available_commands_.end() || !it->second->isVisible()) {
        io_handler_->error("Unknown command: " + target);
        return false;
    }
    io_handler_->println(target + ": " + it->second->get_description());
    return true;
}

void HelpCommand::set_available_commands(
    std::unordered_map<std::string, BaseCommand *> &map) {
    available_commands_ = map;
}

bool HelpCommand::isVisible() const { return true; }
