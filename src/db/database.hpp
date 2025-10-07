#pragma once
#include <memory>
#include <string>
#include <odb/database.hxx>
#include <odb/pgsql/database.hxx>
#include <odb/transaction.hxx>
#include "../dao/user_dao.hpp"

namespace db {

class Database {
private:
    std::shared_ptr<odb::core::database> database_;
    std::string connection_string_;

public:
    // Конструктор PostgreSQL
    explicit Database(
        const std::string& host = "localhost",
        unsigned int port = 5432,
        const std::string& database = "pl",
        const std::string& user = "postgres",
        const std::string& password = "q1w2e3r4"
    );
    
    // Фабричный метод
    static std::shared_ptr<Database> create(
        const std::string& host = "localhost",
        unsigned int port = 5432,
        const std::string& database = "pl",
        const std::string& user = "postgres",
        const std::string& password = "q1w2e3r4"
    );
    
    // Получить underlying ODB database
    std::shared_ptr<odb::core::database> get_odb_database() const { return database_; }
    const std::string& get_connection_string() const { return connection_string_; }
    
    // Управление соединением
    bool test_connection();
    void close();
    
    // Миграции и схема
    bool create_schema();
    bool drop_schema();
    
    // Транзакции
    odb::transaction begin_transaction();
    
    // Резервное копирование (для PostgreSQL можно добавить pg_dump)
    bool backup(const std::string& backup_path);
};

// Фабрика для создания DAO
class DAOFactory {
private:
    std::shared_ptr<Database> database_;

public:
    explicit DAOFactory(std::shared_ptr<Database> db);
    
    // Создание DAO объектов
    std::unique_ptr<dao::UserDAO> create_user_dao();
    
    // Получить базу данных
    std::shared_ptr<Database> get_database() const { return database_; }
};

} // namespace db