#include "import_data.hpp"
#include "src/services/data_export_import_service.hpp"
#include <memory>
#include "../command_registry.hpp"

bool ImportDataCommand::execute(const CommandArgs &args) {
    auto current_user = auth_service_->get_current_user();
    if (args.positional.size() == 0) {
        io_handler_->error("❌ File path is required");
        return false;
    }

    std::string file_path = args.positional[0];

    // Подтверждение импорта
    io_handler_->println("⚠️  WARNING: This will overwrite existing data!");
    io_handler_->print("Continue? (yes/no): ");
    std::string confirmation = io_handler_->read_line();
    
    if (confirmation != "yes" && confirmation != "y") {
        io_handler_->println("Import cancelled");
        return false;
    }

    bool success = data_export_import_service_->import_data(file_path, current_user);
    
    if (success) {
        io_handler_->println("✅ Data imported successfully from: " + file_path);
    } else {
        io_handler_->error("❌ Failed to import data");
    }

    return success;
}

bool ImportDataCommand::is_visible() const {
    auto current_user = app_state_->get_current_user();
    return current_user && user_service_->can_manage_users(current_user);
}

ValidationResult ImportDataCommand::validate_args(const CommandArgs &args) const {
    if (args.positional.size() == 0) {
        return {false, "File path is required. Usage: import-users <file_path>"};
    }
    if (args.positional.size() > 1) {
        return {false, "Too many arguments. Usage: import-users <file_path>"};
    }
    return {true, ""};
}

namespace {
bool registered = []() {
    CommandRegistry::register_command(
        "import-users", [](auto app_state, auto io, auto auth, auto user, auto log, auto d) {
            return std::make_unique<ImportDataCommand>(
                "import-users", "Import user data", "import-users [filepath]", app_state, io, auth,
                user, log, d);
        });
    return true;
}();
}