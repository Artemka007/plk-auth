#include "data_export_import_service.hpp"
#include <memory>
#include <string>
#include "src/dao/data_export_import_dao.hpp"
#include "src/cli/io_handler.hpp"

namespace services {

DataExportImportService::DataExportImportService(
    std::shared_ptr<dao::DataExportImportDAO> export_import_dao,
    std::shared_ptr<IOHandler> io_handler)
    : export_import_dao_(std::move(export_import_dao)),
      io_handler_(std::move(io_handler)) {}

bool DataExportImportService::export_data(const std::string& file_path) {
    try {
        io_handler_->println("Exporting data to: " + file_path);
        bool result = export_import_dao_->export_to_file(file_path);
        if (result) {
            io_handler_->println("✅ Data exported successfully");
        } else {
            io_handler_->error("❌ Data export failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Export error: " + std::string(e.what()));
        return false;
    }
}

bool DataExportImportService::import_data(const std::string& file_path) {
    try {
        io_handler_->println("Importing data from: " + file_path);
        bool result = export_import_dao_->import_from_file(file_path);
        if (result) {
            io_handler_->println("✅ Data imported successfully");
        } else {
            io_handler_->error("❌ Data import failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Import error: " + std::string(e.what()));
        return false;
    }
}

bool DataExportImportService::create_backup(const std::string& backup_path) {
    try {
        io_handler_->println("Creating backup: " + backup_path);
        bool result = export_import_dao_->create_backup(backup_path);
        if (result) {
            io_handler_->println("✅ Backup created successfully");
        } else {
            io_handler_->error("❌ Backup creation failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Backup error: " + std::string(e.what()));
        return false;
    }
}

bool DataExportImportService::restore_backup(const std::string& backup_path) {
    try {
        io_handler_->println("Restoring from backup: " + backup_path);
        bool result = export_import_dao_->restore_backup(backup_path);
        if (result) {
            io_handler_->println("✅ Backup restored successfully");
        } else {
            io_handler_->error("❌ Backup restoration failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Restore error: " + std::string(e.what()));
        return false;
    }
}

bool DataExportImportService::export_logs_csv(const std::string& file_path) {
    try {
        io_handler_->println("Exporting logs to CSV: " + file_path);
        bool result = export_import_dao_->export_logs_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Logs exported successfully");
        } else {
            io_handler_->error("❌ Logs export failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Logs export error: " + std::string(e.what()));
        return false;
    }
}

bool DataExportImportService::export_users_csv(const std::string& file_path) {
    try {
        io_handler_->println("Exporting users to CSV: " + file_path);
        bool result = export_import_dao_->export_users_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Users exported successfully");
        } else {
            io_handler_->error("❌ Users export failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Users export error: " + std::string(e.what()));
        return false;
    }
}

bool DataExportImportService::export_roles_csv(const std::string& file_path) {
    try {
        io_handler_->println("Exporting roles to CSV: " + file_path);
        bool result = export_import_dao_->export_roles_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Roles exported successfully");
        } else {
            io_handler_->error("❌ Roles export failed");
        }
        return result;
    } catch (const std::exception& e) {
        io_handler_->error("❌ Roles export error: " + std::string(e.what()));
        return false;
    }
}

void DataExportImportService::show_statistics() {
    try {
        size_t user_count = export_import_dao_->get_user_count();
        size_t log_count = export_import_dao_->get_log_count();
        size_t role_count = export_import_dao_->get_role_count();
        
        io_handler_->println("📊 Database Statistics:");
        io_handler_->println("  👥 Users: " + std::to_string(user_count));
        io_handler_->println("  📝 Logs: " + std::to_string(log_count));
        io_handler_->println("  🎭 Roles: " + std::to_string(role_count));
        io_handler_->println("  💾 Total records: " + std::to_string(user_count + log_count + role_count));
    } catch (const std::exception& e) {
        io_handler_->error("❌ Error getting statistics: " + std::string(e.what()));
    }
}

}