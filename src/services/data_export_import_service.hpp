#pragma once

#include <memory>
#include <string>
#include "src/dao/data_export_import_dao.hpp"
#include "src/cli/io_handler.hpp"
#include "log_service.hpp"
#include "src/models/user.hpp"
#include "src/models/enums.hpp"

namespace services {

class DataExportImportService {
public:
    explicit DataExportImportService(
        std::shared_ptr<dao::DataExportImportDAO> export_import_dao,
        std::shared_ptr<IOHandler> io_handler,
        std::shared_ptr<LogService> log_service);

    bool export_data(const std::string& file_path, const std::shared_ptr<const models::User>& actor = nullptr);
    bool import_data(const std::string& file_path, const std::shared_ptr<const models::User>& actor = nullptr);
    bool create_backup(const std::string& backup_path, const std::shared_ptr<const models::User>& actor = nullptr);
    bool restore_backup(const std::string& backup_path, const std::shared_ptr<const models::User>& actor = nullptr);
    bool export_logs_csv(const std::string& file_path, const std::shared_ptr<const models::User>& actor = nullptr);
    bool export_users_csv(const std::string& file_path, const std::shared_ptr<const models::User>& actor = nullptr);
    bool export_roles_csv(const std::string& file_path, const std::shared_ptr<const models::User>& actor = nullptr);
    void show_statistics(const std::shared_ptr<const models::User>& actor = nullptr);

private:
    std::shared_ptr<dao::DataExportImportDAO> export_import_dao_;
    std::shared_ptr<IOHandler> io_handler_;
    std::shared_ptr<LogService> log_service_;
};

}