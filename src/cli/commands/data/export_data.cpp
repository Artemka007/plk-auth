#include "export_data.hpp"
#include "src/services/data_export_import_service.hpp"
#include <memory>
#include "../command_registry.hpp"

bool ExportDataCommand::execute(const CommandArgs &args) {
    auto current_user = auth_service_->get_current_user();
    std::string file_path = "export.csv";
    
    // Исправленный доступ к options
    auto output_path_it = args.options.find("outputPath");
    if (output_path_it != args.options.end()) {
        file_path = output_path_it->second;  // Используем .second вместо *
    }

    std::string type = "user";
    auto type_it = args.options.find("type");
    if (type_it != args.options.end()) {
        type = type_it->second;  // Используем .second вместо *
    }

    bool success = false;

    if (type == "user") {
        success = data_export_import_service_->export_data(file_path, current_user);
    } else if (type == "log") {
        success = data_export_import_service_->export_logs_csv(file_path, current_user);
    } else {
        io_handler_->error("Invalid type. Use 'user' or 'log'");
        return false;
    }
    
    if (success) {
        io_handler_->println("✅ Data exported successfully to: " + file_path);
    } else {
        io_handler_->error("❌ Failed to export data");
    }

    return success;
}

bool ExportDataCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

ValidationResult ExportDataCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() > 1) {
        return {false, "Too many arguments. Usage: export-data --type=user|log --outputPath=/path/to/save"};
    }
    
    // Дополнительная валидация опций
    auto type_it = args.options.find("type");
    if (type_it != args.options.end()) {
        std::string type = type_it->second;
        if (type != "user" && type != "log") {
            return {false, "Invalid type. Use 'user' or 'log'"};
        }
    }
    
    return {true, ""};
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "export-data", [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<ExportDataCommand>(
                "export-data", "Export data", "export-data --type=user|log --outputPath=/path/to/save", app_state, io, auth,
                user, log, d);
        });
    return true;
}();
}