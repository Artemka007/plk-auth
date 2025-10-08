#include "database.hpp"
#include <iostream>
#include <pqxx/pqxx>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>
#include <filesystem>
#include "../dao/log_dao.hpp"
#include "../dao/user_dao.hpp"
#include "../dao/access_permission_dao.hpp"

namespace db {

// PostgreSQL реализация
Database::Database(
    const std::string& host,
    unsigned int port,
    const std::string& database,
    const std::string& user,
    const std::string& password
) {
    try {
        connection_string_ = "postgresql://" + user + ":" + password + "@" + host + 
                           ":" + std::to_string(port) + "/" + database;
        
        // Создаем соединение с базой данных
        connection_ = std::make_shared<pqxx::connection>(connection_string_);
        
        if (!connection_->is_open()) {
            throw std::runtime_error("Failed to open database connection");
        }
        
        std::cout << "Connected to PostgreSQL database: " << database 
                  << " on " << host << ":" << port << std::endl;
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create PostgreSQL database: " + std::string(e.what()));
    }
}

std::shared_ptr<Database> Database::create(
    const std::string& host,
    unsigned int port,
    const std::string& database,
    const std::string& user,
    const std::string& password) {
    
    return std::make_shared<Database>(host, port, database, user, password);
}

bool Database::test_connection() {
    try {
        if (!connection_ || !connection_->is_open()) {
            return false;
        }
        
        pqxx::work txn(*connection_);
        auto result = txn.exec("SELECT 1 as test_value");
        txn.commit();
        
        if (!result.empty() && result[0]["test_value"].as<int>() == 1) {
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Connection test failed: " << e.what() << std::endl;
        return false;
    }
}

void Database::close() {
    if (connection_ && connection_->is_open()) {
        connection_->close();
        std::cout << "Database connection closed" << std::endl;
    }
    connection_.reset();
}

bool Database::create_schema() {
    try {
        pqxx::work txn(*connection_);
        
        // Создаем таблицу пользователей
        txn.exec(
            "CREATE TABLE IF NOT EXISTS app_user ("
            "id VARCHAR(36) PRIMARY KEY,"
            "first_name VARCHAR(100) NOT NULL,"
            "last_name VARCHAR(100) NOT NULL,"
            "patronymic VARCHAR(100),"
            "email VARCHAR(255) UNIQUE NOT NULL,"
            "phone VARCHAR(20),"
            "password_hash VARCHAR(255) NOT NULL,"
            "is_active BOOLEAN NOT NULL DEFAULT true,"
            "password_change_required BOOLEAN NOT NULL DEFAULT true,"
            "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "last_login_at TIMESTAMP"
            ")"
        );
        
        // Создаем таблицу ролей
        txn.exec(
            "CREATE TABLE IF NOT EXISTS user_role ("
            "id VARCHAR(36) PRIMARY KEY,"
            "name VARCHAR(50) UNIQUE NOT NULL,"
            "description TEXT,"
            "is_system BOOLEAN NOT NULL DEFAULT false,"
            "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ")"
        );
        
        // Создаем таблицу назначения ролей
        txn.exec(
            "CREATE TABLE IF NOT EXISTS user_role_assignment ("
            "id SERIAL PRIMARY KEY,"
            "user_id VARCHAR(36) NOT NULL REFERENCES app_user(id) ON DELETE CASCADE,"
            "role_id VARCHAR(36) NOT NULL REFERENCES user_role(id) ON DELETE CASCADE,"
            "assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "UNIQUE(user_id, role_id)"
            ")"
        );
        
        // Создаем таблицу разрешений
        txn.exec(
            "CREATE TABLE IF NOT EXISTS access_permission ("
            "id VARCHAR(36) PRIMARY KEY,"
            "name VARCHAR(50) UNIQUE NOT NULL,"
            "description TEXT"
            ")"
        );
        
        // Создаем таблицу связи ролей и разрешений
        txn.exec(
            "CREATE TABLE IF NOT EXISTS role_permission ("
            "id SERIAL PRIMARY KEY,"
            "role_id VARCHAR(36) NOT NULL REFERENCES user_role(id) ON DELETE CASCADE,"
            "permission_id VARCHAR(36) NOT NULL REFERENCES access_permission(id) ON DELETE CASCADE,"
            "granted_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "UNIQUE(role_id, permission_id)"
            ")"
        );
        
        // Создаем таблицу логов
        txn.exec(
            "CREATE TABLE IF NOT EXISTS system_log ("
            "id VARCHAR(36) PRIMARY KEY,"
            "level VARCHAR(10) NOT NULL,"
            "action_type VARCHAR(50) NOT NULL,"
            "message TEXT NOT NULL,"
            "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "actor_id VARCHAR(36) REFERENCES app_user(id) ON DELETE SET NULL,"
            "subject_id VARCHAR(36) REFERENCES app_user(id) ON DELETE SET NULL,"
            "ip_address VARCHAR(45),"
            "user_agent TEXT"
            ")"
        );
        
        // Создаем индексы для улучшения производительности
        txn.exec("CREATE INDEX IF NOT EXISTS idx_app_user_email ON app_user(email)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_app_user_active ON app_user(is_active)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_user_role_name ON user_role(name)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_user_role_assignment_user ON user_role_assignment(user_id)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_user_role_assignment_role ON user_role_assignment(role_id)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_system_log_timestamp ON system_log(timestamp)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_system_log_level ON system_log(level)");
        
        txn.commit();
        std::cout << "Database schema created successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create schema: " << e.what() << std::endl;
        return false;
    }
}

bool Database::drop_schema() {
    try {
        pqxx::work txn(*connection_);
        
        // Удаляем таблицы в правильном порядке (с учетом внешних ключей)
        txn.exec("DROP TABLE IF EXISTS role_permission CASCADE");
        txn.exec("DROP TABLE IF EXISTS user_role_assignment CASCADE");
        txn.exec("DROP TABLE IF EXISTS system_log CASCADE");
        txn.exec("DROP TABLE IF EXISTS access_permission CASCADE");
        txn.exec("DROP TABLE IF EXISTS user_role CASCADE");
        txn.exec("DROP TABLE IF EXISTS app_user CASCADE");
        
        txn.commit();
        std::cout << "Database schema dropped successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to drop schema: " << e.what() << std::endl;
        return false;
    }
}

std::unique_ptr<pqxx::work> Database::begin_transaction() {
    try {
        return std::make_unique<pqxx::work>(*connection_);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to begin transaction: " + std::string(e.what()));
    }
}

bool Database::backup(const std::string& backup_path) {
    try {
        // Получаем параметры подключения из connection string
        std::string host = connection_->hostname();
        std::string port = connection_->port();
        std::string dbname = connection_->dbname();
        std::string user = connection_->username();
        
        // Пароль нужно хранить отдельно, так как pqxx не предоставляет метод для его получения
        // В реальном приложении пароль должен передаваться как параметр
        std::string password = "password"; // Это нужно исправить в реальном приложении
        
        // Формируем команду pg_dump
        std::string command = "pg_dump";
        command += " -h " + host;
        command += " -p " + port;
        command += " -U " + user;
        command += " -d " + dbname;
        command += " -f " + backup_path;
        command += " -F c"; // custom format
        
        // Устанавливаем переменную окружения с паролем
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", password.c_str());
        #else
            setenv("PGPASSWORD", password.c_str(), 1);
        #endif
        
        std::cout << "Creating backup: " << backup_path << std::endl;
        
        // Выполняем команду
        int result = std::system(command.c_str());
        
        // Очищаем переменную окружения
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", "");
        #else
            unsetenv("PGPASSWORD");
        #endif
        
        if (result == 0) {
            std::cout << "Backup created successfully" << std::endl;
        } else {
            std::cerr << "Backup failed with exit code: " << result << std::endl;
        }
        
        return result == 0;
    } catch (const std::exception& e) {
        std::cerr << "Backup failed: " << e.what() << std::endl;
        return false;
    }
}

bool Database::restore(const std::string& backup_path) {
    try {
        if (!std::filesystem::exists(backup_path)) {
            std::cerr << "Backup file not found: " << backup_path << std::endl;
            return false;
        }
        
        // Получаем параметры подключения
        std::string host = connection_->hostname();
        std::string port = connection_->port();
        std::string dbname = connection_->dbname();
        std::string user = connection_->username();
        std::string password = "password"; // Это нужно исправить в реальном приложении
        
        // Формируем команду pg_restore
        std::string command = "pg_restore";
        command += " -h " + host;
        command += " -p " + port;
        command += " -U " + user;
        command += " -d " + dbname;
        command += " -c"; // clean (drop) database objects before recreating
        command += " " + backup_path;
        
        // Устанавливаем переменную окружения с паролем
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", password.c_str());
        #else
            setenv("PGPASSWORD", password.c_str(), 1);
        #endif
        
        std::cout << "Restoring from backup: " << backup_path << std::endl;
        
        // Выполняем команду
        int result = std::system(command.c_str());
        
        // Очищаем переменную окружения
        #ifdef _WIN32
            _putenv_s("PGPASSWORD", "");
        #else
            unsetenv("PGPASSWORD");
        #endif
        
        if (result == 0) {
            std::cout << "Restore completed successfully" << std::endl;
        } else {
            std::cerr << "Restore failed with exit code: " << result << std::endl;
        }
        
        return result == 0;
    } catch (const std::exception& e) {
        std::cerr << "Restore failed: " << e.what() << std::endl;
        return false;
    }
}

std::string Database::get_connection_info() const {
    if (!connection_) {
        return "No connection";
    }
    
    std::stringstream info;
    info << "Host: " << connection_->hostname()
         << ", Port: " << connection_->port()
         << ", Database: " << connection_->dbname()
         << ", User: " << connection_->username()
         << ", Status: " << (connection_->is_open() ? "Connected" : "Disconnected");
    
    return info.str();
}

bool Database::export_to_file(const std::string& file_path) {
    try {
        pqxx::work txn(*connection_);
        
        // Экспорт структуры и данных
        std::string export_sql = 
            "COPY (" 
            "SELECT '-- Users table data' AS comment UNION ALL "
            "SELECT 'INSERT INTO app_user (id, first_name, last_name, email, password_hash, is_active, password_change_required, created_at) VALUES (' || "
            "'''' || id || ''', ' || "
            "'''' || REPLACE(first_name, '''', '''''') || ''', ' || "
            "'''' || REPLACE(last_name, '''', '''''') || ''', ' || "
            "'''' || email || ''', ' || "
            "'''' || password_hash || ''', ' || "
            "is_active || ', ' || "
            "password_change_required || ', ' || "
            "'''' || created_at || ''');' "
            "FROM app_user"
            ") TO STDOUT";
        
        // Здесь можно добавить экспорт других таблиц
        
        std::ofstream file(file_path);
        auto result = txn.exec(export_sql);
        for (const auto& row : result) {
            file << row[0].as<std::string>() << "\n";
        }
        
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Export failed: " << e.what() << std::endl;
        return false;
    }
}

bool Database::import_from_file(const std::string& file_path) {
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

bool Database::export_logs_to_csv(const std::string& file_path, const dao::LogFilter& filter) {
    try {
        pqxx::work txn(*connection_);
        
        std::string where_clause = "";
        // Здесь можно добавить фильтрацию по уровню, дате и т.д.
        
        std::string export_sql = 
            "COPY (" 
            "SELECT level, action_type, message, timestamp, actor_id, subject_id, ip_address, user_agent "
            "FROM system_log" + where_clause + " "
            "ORDER BY timestamp DESC"
            ") TO STDOUT WITH CSV HEADER";
        
        std::ofstream file(file_path);
        auto result = txn.exec(export_sql);
        
        // Заголовок CSV
        file << "level,action_type,message,timestamp,actor_id,subject_id,ip_address,user_agent\n";
        
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

// Реализация DAOFactory
DAOFactory::DAOFactory(std::shared_ptr<Database> db) : database_(std::move(db)) {}

std::shared_ptr<dao::UserDAO> DAOFactory::create_user_dao() {
    if (!database_ || !database_->get_connection()) {
        throw std::runtime_error("Database connection is not available");
    }
    // Включаем заголовочный файл UserDAO перед использованием
    return std::shared_ptr<dao::UserDAO>(new dao::UserDAO(database_->get_connection()));
}

std::shared_ptr<dao::LogDAO> DAOFactory::create_log_dao() {
    if (!database_ || !database_->get_connection()) {
        throw std::runtime_error("Database connection is not available");
    }
    // Включаем заголовочный файл UserDAO перед использованием
    return std::shared_ptr<dao::LogDAO>(new dao::LogDAO(database_->get_connection()));
}

std::shared_ptr<dao::AccessPermissionDAO> DAOFactory::create_permission_dao() {
    if (!database_ || !database_->get_connection()) {
        throw std::runtime_error("Database connection is not available");
    }
    return std::shared_ptr<dao::AccessPermissionDAO>(new dao::AccessPermissionDAO(database_->get_connection()));
}
} // namespace db