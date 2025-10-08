#pragma once

#include <memory>
#include <string>
#include <pqxx/pqxx>
#include "log_dao.hpp"

namespace dao {

class DataExportImportDAO {
private:
    std::shared_ptr<pqxx::connection> connection_;

public:
    explicit DataExportImportDAO(std::shared_ptr<pqxx::connection> connection);
    
    bool export_to_file(const std::string& file_path);
    bool export_logs_to_csv(const std::string& file_path, const LogFilter& filter = {});
    bool export_users_to_csv(const std::string& file_path);
    bool export_roles_to_csv(const std::string& file_path);
    
    bool import_from_file(const std::string& file_path);
    bool import_users_from_csv(const std::string& file_path);
    
    bool create_backup(const std::string& backup_path);
    bool restore_backup(const std::string& backup_path);
    
    size_t get_user_count();
    size_t get_log_count();
    size_t get_role_count();
};

} 