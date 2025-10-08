#pragma once

#include <memory>
#include <string>
#include "src/dao/data_export_import_dao.hpp"
#include "src/cli/io_handler.hpp"

namespace services {

class DataExportImportService {
private:
    std::shared_ptr<dao::DataExportImportDAO> export_import_dao_;
    std::shared_ptr<IOHandler> io_handler_;

public:
    DataExportImportService(std::shared_ptr<dao::DataExportImportDAO> export_import_dao,
                          std::shared_ptr<IOHandler> io_handler);
    
    // Основные операции
    bool export_data(const std::string& file_path);
    bool import_data(const std::string& file_path);
    bool create_backup(const std::string& backup_path);
    bool restore_backup(const std::string& backup_path);
    
    // Специализированный экспорт
    bool export_logs_csv(const std::string& file_path);
    bool export_users_csv(const std::string& file_path);
    bool export_roles_csv(const std::string& file_path);
    
    // Статистика
    void show_statistics();
};

}