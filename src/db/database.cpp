#include "./database.hpp"
#include "../dao/user_dao.hpp"
#include <odb/schema-catalog.hxx>
#include <odb/connection.hxx>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <functional>

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
        connection_string_ = "host=" + host + 
                           " port=" + std::to_string(port) + 
                           " dbname=" + database + 
                           " user=" + user + 
                           " password=" + password;
        
        database_ = std::make_shared<odb::pgsql::database>(user, password, database, host, port);
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
        odb::transaction t(database_->begin());
        database_->execute("SELECT 1");
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Database::close() {
    database_.reset();
}

bool Database::create_schema() {
    try {
        odb::transaction t(database_->begin());
        
        // Создаем схему через ODB
        odb::schema_catalog::create_schema(*database_);
        
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Database::drop_schema() {
    try {
        odb::transaction t(database_->begin());
        
        // Удаляем схему через ODB
        odb::schema_catalog::drop_schema(*database_);
        
        t.commit();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

odb::transaction Database::begin_transaction() {
    return odb::transaction(database_->begin());
}

bool Database::backup(const std::string& backup_path) {
    // Для PostgreSQL можно интегрировать с pg_dump
    // В реальном приложении здесь был бы вызов system("pg_dump ...")
    // Но для простоты просто возвращаем true
    return true;
}

// Реализация DAOFactory
DAOFactory::DAOFactory(std::shared_ptr<Database> db) : database_(std::move(db)) {}

std::unique_ptr<dao::UserDAO> DAOFactory::create_user_dao() {
    return std::make_unique<dao::UserDAO>(database_->get_odb_database());
}

} // namespace db