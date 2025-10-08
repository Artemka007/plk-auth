#include "data_export_import_dao.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <pqxx/pqxx>
#include "log_dao.hpp"
#include "models/enums.hpp"

namespace dao {

DataExportImportDAO::DataExportImportDAO(std::shared_ptr<pqxx::connection> connection)
    : connection_(std::move(connection)) {}

bool DataExportImportDAO::export_to_file(const std::string& file_path) {
    try {
        pqxx::work txn(*connection_);
        
        std::ofstream file(file_path);
        if (!file.is_open()) {
            return false;
        }

        // Экспорт пользователей
        file << "-- Users table data\n";
        auto users_result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "password_hash, is_active, password_change_required, created_at, updated_at, last_login_at "
            "FROM app_user"
        );
        
        for (const auto& row : users_result) {
            file << "INSERT INTO app_user (id, first_name, last_name, patronymic, email, phone, "
                 << "password_hash, is_active, password_change_required, created_at, updated_at, last_login_at) VALUES ("
                 << "'" << row["id"].as<std::string>() << "', "
                 << "'" << row["first_name"].as<std::string>() << "', "
                 << "'" << row["last_name"].as<std::string>() << "', "
                 << (row["patronymic"].is_null() ? "NULL" : "'" + row["patronymic"].as<std::string>() + "'") << ", "
                 << "'" << row["email"].as<std::string>() << "', "
                 << (row["phone"].is_null() ? "NULL" : "'" + row["phone"].as<std::string>() + "'") << ", "
                 << "'" << row["password_hash"].as<std::string>() << "', "
                 << row["is_active"].as<bool>() << ", "
                 << row["password_change_required"].as<bool>() << ", "
                 << "'" << row["created_at"].as<std::string>() << "', "
                 << "'" << row["updated_at"].as<std::string>() << "', "
                 << (row["last_login_at"].is_null() ? "NULL" : "'" + row["last_login_at"].as<std::string>() + "'")
                 << ");\n";
        }

        // Экспорт ролей
        file << "\n-- Roles table data\n";
        auto roles_result = txn.exec("SELECT id, name, description, is_system, created_at, updated_at FROM user_role");
        for (const auto& row : roles_result) {
            file << "INSERT INTO user_role (id, name, description, is_system, created_at, updated_at) VALUES ("
                 << "'" << row["id"].as<std::string>() << "', "
                 << "'" << row["name"].as<std::string>() << "', "
                 << (row["description"].is_null() ? "NULL" : "'" + row["description"].as<std::string>() + "'") << ", "
                 << row["is_system"].as<bool>() << ", "
                 << "'" << row["created_at"].as<std::string>() << "', "
                 << "'" << row["updated_at"].as<std::string>() << "'"
                 << ");\n";
        }

        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Export failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataExportImportDAO::export_logs_to_csv(const std::string& file_path, const LogFilter& filter) {
    try {
        pqxx::work txn(*connection_);
        
        std::string where_clause = "";
        if (filter.level != models::LogLevel{}) {
            where_clause += " WHERE level = '" + models::to_string(filter.level) + "'";
        }
        
        std::string export_sql = 
            "COPY (" 
            "SELECT level, action_type, message, timestamp, actor_id, subject_id, ip_address, user_agent "
            "FROM system_log" + where_clause + " "
            "ORDER BY timestamp DESC"
            ") TO STDOUT WITH CSV HEADER";
        
        std::ofstream file(file_path);
        auto result = txn.exec(export_sql);
        
        for (const auto& row : result) {
            file << row["level"].as<std::string>() << ","
                 << row["action_type"].as<std::string>() << ","
                 << "\"" << row["message"].as<std::string>() << "\","
                 << row["timestamp"].as<std::string>() << ","
                 << (row["actor_id"].is_null() ? "" : row["actor_id"].as<std::string>()) << ","
                 << (row["subject_id"].is_null() ? "" : row["subject_id"].as<std::string>()) << ","
                 << (row["ip_address"].is_null() ? "" : row["ip_address"].as<std::string>()) << ","
                 << (row["user_agent"].is_null() ? "" : row["user_agent"].as<std::string>()) << "\n";
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Logs export failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataExportImportDAO::export_users_to_csv(const std::string& file_path) {
    try {
        pqxx::work txn(*connection_);
        
        std::string export_sql = 
            "COPY (" 
            "SELECT id, first_name, last_name, patronymic, email, phone, is_active, "
            "password_change_required, created_at, last_login_at "
            "FROM app_user ORDER BY created_at DESC"
            ") TO STDOUT WITH CSV HEADER";
        
        std::ofstream file(file_path);
        
        file << "id,first_name,last_name,patronymic,email,phone,is_active,"
             << "password_change_required,created_at,last_login_at\n";
        
        auto result = txn.exec(
            "SELECT id, first_name, last_name, patronymic, email, phone, "
            "is_active, password_change_required, created_at, last_login_at "
            "FROM app_user ORDER BY created_at DESC"
        );
        
        for (const auto& row : result) {
            file << row["id"].as<std::string>() << ","
                 << row["first_name"].as<std::string>() << ","
                 << row["last_name"].as<std::string>() << ","
                 << (row["patronymic"].is_null() ? "" : row["patronymic"].as<std::string>()) << ","
                 << row["email"].as<std::string>() << ","
                 << (row["phone"].is_null() ? "" : row["phone"].as<std::string>()) << ","
                 << row["is_active"].as<bool>() << ","
                 << row["password_change_required"].as<bool>() << ","
                 << row["created_at"].as<std::string>() << ","
                 << (row["last_login_at"].is_null() ? "" : row["last_login_at"].as<std::string>()) << "\n";
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Users export failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataExportImportDAO::export_roles_to_csv(const std::string& file_path) {
    try {
        pqxx::work txn(*connection_);
        
        std::string export_sql = 
            "COPY (" 
            "SELECT id, name, description, is_system, created_at, updated_at "
            "FROM user_role ORDER BY name"
            ") TO STDOUT WITH CSV HEADER";
        
        std::ofstream file(file_path);
        auto result = txn.exec(export_sql);
        
        for (const auto& row : result) {
            file << row["id"].as<std::string>() << ","
                 << row["name"].as<std::string>() << ","
                 << (row["description"].is_null() ? "" : row["description"].as<std::string>()) << ","
                 << row["is_system"].as<bool>() << ","
                 << row["created_at"].as<std::string>() << ","
                 << row["updated_at"].as<std::string>() << "\n";
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Roles export failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataExportImportDAO::import_from_file(const std::string& file_path) {
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return false;
        }
        
        pqxx::work txn(*connection_);
        std::string line;
        
        while (std::getline(file, line)) {
            if (!line.empty() && line.find("INSERT INTO") != std::string::npos) {
                txn.exec(line);
            }
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Import failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataExportImportDAO::import_users_from_csv(const std::string& file_path) {
    // Реализация импорта пользователей из CSV
    // Можно добавить позже при необходимости
    return false;
}

bool DataExportImportDAO::create_backup(const std::string& backup_path) {
    try {
        std::string host = connection_->hostname();
        std::string port = connection_->port();
        std::string dbname = connection_->dbname();
        std::string user = connection_->username();
        std::string password = "password"; // В реальном приложении получать из конфигурации
        
        std::string command = "pg_dump";
        command += " -h " + host;
        command += " -p " + port;
        command += " -U " + user;
        command += " -d " + dbname;
        command += " -f " + backup_path;
        command += " -F c";
        
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", password.c_str());
        #else
            setenv("PGPASSWORD", password.c_str(), 1);
        #endif
        
        int result = std::system(command.c_str());
        
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", "");
        #else
            unsetenv("PGPASSWORD");
        #endif
        
        return result == 0;
    } catch (const std::exception& e) {
        std::cerr << "Backup failed: " << e.what() << std::endl;
        return false;
    }
}

bool DataExportImportDAO::restore_backup(const std::string& backup_path) {
    try {
        if (!std::filesystem::exists(backup_path)) {
            return false;
        }
        
        std::string host = connection_->hostname();
        std::string port = connection_->port();
        std::string dbname = connection_->dbname();
        std::string user = connection_->username();
        std::string password = "password";
        
        std::string command = "pg_restore";
        command += " -h " + host;
        command += " -p " + port;
        command += " -U " + user;
        command += " -d " + dbname;
        command += " -c";
        command += " " + backup_path;
        
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", password.c_str());
        #else
            setenv("PGPASSWORD", password.c_str(), 1);
        #endif
        
        int result = std::system(command.c_str());
        
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", "");
        #else
            unsetenv("PGPASSWORD");
        #endif
        
        return result == 0;
    } catch (const std::exception& e) {
        std::cerr << "Restore failed: " << e.what() << std::endl;
        return false;
    }
}

size_t DataExportImportDAO::get_user_count() {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec("SELECT COUNT(*) FROM app_user");
        txn.commit();
        return result[0][0].as<size_t>();
    } catch (const std::exception& e) {
        return 0;
    }
}

size_t DataExportImportDAO::get_log_count() {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec("SELECT COUNT(*) FROM system_log");
        txn.commit();
        return result[0][0].as<size_t>();
    } catch (const std::exception& e) {
        return 0;
    }
}

size_t DataExportImportDAO::get_role_count() {
    try {
        pqxx::work txn(*connection_);
        auto result = txn.exec("SELECT COUNT(*) FROM user_role");
        txn.commit();
        return result[0][0].as<size_t>();
    } catch (const std::exception& e) {
        return 0;
    }
}

}