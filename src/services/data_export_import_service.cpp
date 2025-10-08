#include "data_export_import_service.hpp"
#include "log_service.hpp"
#include "src/cli/io_handler.hpp"
#include "src/dao/data_export_import_dao.hpp"
#include "src/models/enums.hpp"
#include "src/models/user.hpp"
#include <memory>
#include <string>

namespace services {

DataExportImportService::DataExportImportService(
    std::shared_ptr<dao::DataExportImportDAO> export_import_dao,
    std::shared_ptr<IOHandler> io_handler,
    std::shared_ptr<LogService> log_service)
    : export_import_dao_(std::move(export_import_dao)),
      io_handler_(std::move(io_handler)), log_service_(std::move(log_service)) {
}

bool DataExportImportService::export_data(
    const std::string &file_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Exporting data to: " + file_path);
        log_service_->info(models::ActionType::SYSTEM_EXPORT,
                           "Starting data export to: " + file_path, actor,
                           nullptr);

        bool result = export_import_dao_->export_users_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Data exported successfully");
            log_service_->info(models::ActionType::SYSTEM_EXPORT,
                               "Data export completed successfully: " +
                                   file_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Data export failed");
            log_service_->error(models::ActionType::SYSTEM_EXPORT,
                                "Data export failed: " + file_path, actor,
                                nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Export error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_EXPORT,
                            "Data export error: " + std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

bool DataExportImportService::import_data(
    const std::string &file_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Importing data from: " + file_path);
        log_service_->info(models::ActionType::SYSTEM_IMPORT,
                           "Starting data import from: " + file_path, actor,
                           nullptr);

        bool result = export_import_dao_->import_from_file(file_path);
        if (result) {
            io_handler_->println("✅ Data imported successfully");
            log_service_->info(models::ActionType::SYSTEM_IMPORT,
                               "Data import completed successfully: " +
                                   file_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Data import failed");
            log_service_->error(models::ActionType::SYSTEM_IMPORT,
                                "Data import failed: " + file_path, actor,
                                nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Import error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_IMPORT,
                            "Data import error: " + std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

bool DataExportImportService::create_backup(
    const std::string &backup_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Creating backup: " + backup_path);
        log_service_->info(models::ActionType::SYSTEM_BACKUP_CREATED,
                           "Starting backup creation: " + backup_path, actor,
                           nullptr);

        bool result = export_import_dao_->create_backup(backup_path);
        if (result) {
            io_handler_->println("✅ Backup created successfully");
            log_service_->info(models::ActionType::SYSTEM_BACKUP_CREATED,
                               "Backup created successfully: " + backup_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Backup creation failed");
            log_service_->error(models::ActionType::SYSTEM_BACKUP_CREATED,
                                "Backup creation failed: " + backup_path, actor,
                                nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Backup error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_BACKUP_CREATED,
                            "Backup creation error: " + std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

bool DataExportImportService::restore_backup(
    const std::string &backup_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Restoring from backup: " + backup_path);
        log_service_->warning(models::ActionType::SYSTEM_BACKUP_RESTORED,
                              "Starting backup restoration: " + backup_path,
                              actor, nullptr);

        bool result = export_import_dao_->restore_backup(backup_path);
        if (result) {
            io_handler_->println("✅ Backup restored successfully");
            log_service_->info(models::ActionType::SYSTEM_BACKUP_RESTORED,
                               "Backup restored successfully: " + backup_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Backup restoration failed");
            log_service_->error(models::ActionType::SYSTEM_BACKUP_RESTORED,
                                "Backup restoration failed: " + backup_path,
                                actor, nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Restore error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_BACKUP_RESTORED,
                            "Backup restoration error: " +
                                std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

bool DataExportImportService::export_logs_csv(
    const std::string &file_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Exporting logs to CSV: " + file_path);
        log_service_->info(models::ActionType::SYSTEM_EXPORT,
                           "Starting logs export to CSV: " + file_path, actor,
                           nullptr);

        bool result = export_import_dao_->export_logs_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Logs exported successfully");
            log_service_->info(models::ActionType::SYSTEM_EXPORT,
                               "Logs export completed successfully: " +
                                   file_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Logs export failed");
            log_service_->error(models::ActionType::SYSTEM_EXPORT,
                                "Logs export failed: " + file_path, actor,
                                nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Logs export error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_EXPORT,
                            "Logs export error: " + std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

bool DataExportImportService::export_users_csv(
    const std::string &file_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Exporting users to CSV: " + file_path);
        log_service_->info(models::ActionType::SYSTEM_EXPORT,
                           "Starting users export to CSV: " + file_path, actor,
                           nullptr);

        bool result = export_import_dao_->export_users_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Users exported successfully");
            log_service_->info(models::ActionType::SYSTEM_EXPORT,
                               "Users export completed successfully: " +
                                   file_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Users export failed");
            log_service_->error(models::ActionType::SYSTEM_EXPORT,
                                "Users export failed: " + file_path, actor,
                                nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Users export error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_EXPORT,
                            "Users export error: " + std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

bool DataExportImportService::export_roles_csv(
    const std::string &file_path,
    const std::shared_ptr<const models::User> &actor) {
    try {
        io_handler_->println("Exporting roles to CSV: " + file_path);
        log_service_->info(models::ActionType::SYSTEM_EXPORT,
                           "Starting roles export to CSV: " + file_path, actor,
                           nullptr);

        bool result = export_import_dao_->export_roles_to_csv(file_path);
        if (result) {
            io_handler_->println("✅ Roles exported successfully");
            log_service_->info(models::ActionType::SYSTEM_EXPORT,
                               "Roles export completed successfully: " +
                                   file_path,
                               actor, nullptr);
        } else {
            io_handler_->error("❌ Roles export failed");
            log_service_->error(models::ActionType::SYSTEM_EXPORT,
                                "Roles export failed: " + file_path, actor,
                                nullptr);
        }
        return result;
    } catch (const std::exception &e) {
        io_handler_->error("❌ Roles export error: " + std::string(e.what()));
        log_service_->error(models::ActionType::SYSTEM_EXPORT,
                            "Roles export error: " + std::string(e.what()),
                            actor, nullptr);
        return false;
    }
}

void DataExportImportService::show_statistics(
    const std::shared_ptr<const models::User> &actor) {
    try {
        size_t user_count = export_import_dao_->get_user_count();
        size_t log_count = export_import_dao_->get_log_count();
        size_t role_count = export_import_dao_->get_role_count();

        io_handler_->println("📊 Database Statistics:");
        io_handler_->println("  👥 Users: " + std::to_string(user_count));
        io_handler_->println("  📝 Logs: " + std::to_string(log_count));
        io_handler_->println("  🎭 Roles: " + std::to_string(role_count));
        io_handler_->println(
            "  💾 Total records: " +
            std::to_string(user_count + log_count + role_count));

        log_service_->debug(models::ActionType::PROFILE_VIEWED,
                            "Viewed database statistics - Users: " +
                                std::to_string(user_count) +
                                ", Logs: " + std::to_string(log_count) +
                                ", Roles: " + std::to_string(role_count),
                            actor, nullptr);
    } catch (const std::exception &e) {
        io_handler_->error("❌ Error getting statistics: " +
                           std::string(e.what()));
        log_service_->error(models::ActionType::SECURITY_VIOLATION,
                            "Error getting database statistics: " +
                                std::string(e.what()),
                            actor, nullptr);
    }
}

} // namespace services