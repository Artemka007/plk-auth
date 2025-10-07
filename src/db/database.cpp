#include "database.hpp"
#include <iostream>
#include <pqxx/pqxx>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <functional>
#include <filesystem>

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
        int port = connection_->port();
        std::string dbname = connection_->dbname();
        std::string user = connection_->username();
        
        // Пароль нужно хранить отдельно, так как pqxx не предоставляет метод для его получения
        // В реальном приложении пароль должен передаваться как параметр
        std::string password = "password"; // Это нужно исправить в реальном приложении
        
        // Формируем команду pg_dump
        std::string command = "pg_dump";
        command += " -h " + host;
        command += " -p " + std::to_string(port);
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
        int port = connection_->port();
        std::string dbname = connection_->dbname();
        std::string user = connection_->username();
        std::string password = "password"; // Это нужно исправить в реальном приложении
        
        // Формируем команду pg_restore
        std::string command = "pg_restore";
        command += " -h " + host;
        command += " -p " + std::to_string(port);
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

// Реализация DAOFactory
DAOFactory::DAOFactory(std::shared_ptr<Database> db) : database_(std::move(db)) {}

std::unique_ptr<dao::UserDAO> DAOFactory::create_user_dao() {
    if (!database_ || !database_->get_connection()) {
        throw std::runtime_error("Database connection is not available");
    }
    // Включаем заголовочный файл UserDAO перед использованием
    return std::unique_ptr<dao::UserDAO>(new dao::UserDAO(database_->get_connection()));
}

std::unique_ptr<dao::UserRoleDAO> DAOFactory::create_user_role_dao() {
    if (!database_ || !database_->get_connection()) {
        throw std::runtime_error("Database connection is not available");
    }
    // Заглушка - нужно реализовать UserRoleDAO
    throw std::runtime_error("UserRoleDAO not implemented yet");
}

std::unique_ptr<dao::AccessPermissionDAO> DAOFactory::create_access_permission_dao() {
    if (!database_ || !database_->get_connection()) {
        throw std::runtime_error("Database connection is not available");
    }
    // Заглушка - нужно реализовать AccessPermissionDAO
    throw std::runtime_error("AccessPermissionDAO not implemented yet");
}

} // namespace db