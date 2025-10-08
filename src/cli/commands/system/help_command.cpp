#include "help_command.hpp"
#include "../command_registry.hpp"

ValidationResult HelpCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() > 1) {
        return {false, "Usage: " + get_usage()};
    }
    return {true, ""};
}

bool HelpCommand::execute(const CommandArgs &args) {
    if (args.positional.empty()) {
        io_handler_->println("Available commands:");
        for (const auto &[name, cmd] : available_commands_) {
            if (cmd->is_visible()) {
                io_handler_->println("  " + name + " - " +
                                     cmd->get_description());
            }
        }
        return true;
    }

    const std::string &target = args.positional[0];
    auto it = available_commands_.find(target);
    if (it == available_commands_.end() || !it->second->is_visible()) {
        io_handler_->error("Unknown command: " + target);
        return false;
    }

    io_handler_->println(target + ": " + it->second->get_description());
    io_handler_->println("Usage: " + it->second->get_usage());
    io_handler_->println();
    return true;
}

void HelpCommand::set_available_commands(
    const std::unordered_map<std::string, BaseCommand *> &map) {
    available_commands_ = map;
}

bool HelpCommand::is_visible() const { return true; }

// --- Static registration ---
namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "help", [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<HelpCommand>(
                "help", "Show help", "help [command]", app_state, io, auth,
                user, log, d);
        });
    return true;
}();
} // namespace
