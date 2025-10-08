#pragma once

#include "app_state.hpp"
#include "io_handler.hpp"
#include "commands/base_command.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "src/services/auth_service.hpp"
#include "src/services/log_service.hpp"
#include "src/services/user_service.hpp"
#include "src/services/data_export_import_service.hpp"

class CliApp {
public:
    CliApp(std::shared_ptr<services::UserService> user_service,
           std::shared_ptr<services::AuthService> auth_service,
           std::shared_ptr<services::LogService> log_service,
           std::shared_ptr<services::DataExportImportService> data_export_import_service,
           std::shared_ptr<IOHandler> io_handler);

    void Run();
    void Stop();

private:
    // Domain dependencies
    std::shared_ptr<services::UserService> user_service_;
    std::shared_ptr<services::AuthService> auth_service_;
    std::shared_ptr<services::LogService> log_service_;
    std::shared_ptr<services::DataExportImportService> data_export_import_service_;

    // Cli-specific dependencies
    std::shared_ptr<IOHandler> io_handler_;
    std::shared_ptr<AppState> app_state_;

    // Commands
    std::vector<std::unique_ptr<BaseCommand>> commands_;
    std::unordered_map<std::string, BaseCommand *> command_map_;

    void initialize_commands();
    void execute_command(const std::string &input);
};
