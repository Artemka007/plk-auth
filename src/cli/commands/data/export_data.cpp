#include "export_data.hpp"
#include "src/services/data_export_import_service.hpp"
#include <memory>
#include "../command_registry.hpp"

bool ExportDataCommand::execute(const CommandArgs &args) {
    auto current_user = auth_service_->get_current_user();
    std::string file_path = "export.csv";
    if (args.positional.size() > 0) {
        file_path = args.positional[0];
    }

    bool success = data_export_import_service_->export_data(file_path, current_user);
    
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
        return {false, "Too many arguments. Usage: export [file_path]"};
    }
    return {true, ""};
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "export-users", [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<ExportDataCommand>(
                "export-users", "Export user data", "export-users [filepath?]", app_state, io, auth,
                user, log, d);
        });
    return true;
}();
}