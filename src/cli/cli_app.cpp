#include "cli/cli_app.hpp"
#include "cli/commands/command_factory.hpp"

CliApp::CliApp(std::shared_ptr<UserService> user_service,
               std::shared_ptr<AuthService> auth_service,
               std::shared_ptr<LogService> log_service,
               std::shared_ptr<IOHandler> io_handler)
    : user_service_(std::move(user_service)),
      auth_service_(std::move(auth_service)),
      log_service_(std::move(log_service)),
      io_handler_(std::move(io_handler)),
      app_state_(std::make_shared<AppState>()) {
    initialize_commands();
}

void CliApp::initialize_commands() {
    commands_ = CommandFactory::create_all_commands(
        app_state_, io_handler_, auth_service_, user_service_, log_service_);

    command_map_.clear();
    for (const auto &cmd : commands_) {
        command_map_[cmd->get_name()] = cmd.get();
    }
}

void CliApp::Run() {
    app_state_->set_running(true);

    while (app_state_->is_running()) {
        if (app_state_->is_authenticated()) {
            auto user = app_state_->get_current_user();
            io_handler_->print("[" + user->email() + "]> ");
        } else {
            io_handler_->print("guest> ");
        }

        std::string input = io_handler_->read_line();

        if (input.empty() && io_handler_->is_eof()) {
            io_handler_->println();
            break;
        }

        if (input.empty()) {
            continue;
        }

        execute_command(input);
    }
}

void CliApp::execute_command(const std::string &input) {
    auto args = io_handler_->split_command(input);
    if (args.empty()) {
        return;
    }

    std::string cmd_name = args[0];
    args.erase(args.begin());

    auto it = command_map_.find(cmd_name);
    if (it == command_map_.end()) {
        io_handler_->error("Unknown command: " + cmd_name);
        return;
    }

    BaseCommand *cmd = it->second;

    if (!cmd->isVisible()) {
        io_handler_->error("Command not available");
        return;
    }

    cmd->execute(args);
}

void CliApp::Stop() {
    app_state_->set_running(false);
