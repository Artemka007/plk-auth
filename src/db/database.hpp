#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <pqxx/pqxx>
#include "../dao/user_dao.hpp"
#include "../dao/log_dao.hpp"

namespace db {

class Database {
private:
    std::shared_ptr<pqxx::connection> connection_;
    std::string connection_string_;

public:
    Database(
        const std::string& host,
        unsigned int port,
        const std::string& database,
        const std::string& user,
        const std::string& password
    );
    
    static std::shared_ptr<Database> create(
        const std::string& host,
        unsigned int port,
        const std::string& database,
        const std::string& user,
        const std::string& password);
    
    // Основные операции
    bool test_connection();
    void close();
    bool create_schema();
    bool drop_schema();
    std::unique_ptr<pqxx::work> begin_transaction();
    bool backup(const std::string& backup_path);
    bool restore(const std::string& backup_path);
    
    // Геттеры
    std::shared_ptr<pqxx::connection> get_connection() const { return connection_; }
    const std::string& get_connection_string() const { return connection_string_; }
    std::string get_connection_info() const;
    
    // Проверка состояния
    bool is_connected() const { 
        return connection_ && connection_->is_open(); 
    }
};

class DAOFactory {
private:
    std::shared_ptr<Database> database_;

public:
    explicit DAOFactory(std::shared_ptr<Database> db);
    
    std::shared_ptr<dao::UserDAO> create_user_dao();
    std::shared_ptr<dao::LogDAO> create_log_dao();
};

} // namespace db