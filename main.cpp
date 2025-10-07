#include <iostream>
#include <memory>
#include <string>
#include "database.hpp"

int main() {
    std::cout << "Starting C++ PostgreSQL Application...\n";
    
    try {
        // Создаем подключение к базе данных
        auto db = db::Database::create(
            "postgres",  // host
            5432,        // port
            "myapp",     // database
            "postgres",  // user
            "password"   // password
        );
        
        std::cout << "Database connection created successfully!\n";
        
        // Тестируем подключение
        if (db->test_connection()) {
            std::cout << "Database connection test: SUCCESS\n";
        } else {
            std::cout << "Database connection test: FAILED\n";
            return 1;
        }
        
        // Выводим информацию о подключении
        std::cout << "Connection info: " << db->get_connection_info() << "\n";
        
        // Создаем таблицы (если они еще не созданы через init.sql)
        std::cout << "Creating database schema...\n";
        if (db->create_schema()) {
            std::cout << "Database schema created successfully!\n";
        } else {
            std::cout << "Failed to create database schema\n";
            // Не выходим с ошибкой, т.к. таблицы могли быть созданы через init.sql
        }
        
        // Демонстрация транзакции
        std::cout << "Testing transaction...\n";
        try {
            auto txn = db->begin_transaction();
            std::cout << "Transaction started successfully\n";
            txn->commit();
            std::cout << "Transaction committed successfully\n";
        } catch (const std::exception& e) {
            std::cout << "Transaction test failed: " << e.what() << "\n";
        }
        
        std::cout << "\nApplication started successfully!\n";
        std::cout << "Press Ctrl+C to exit...\n";
        
        // Простой цикл чтобы приложение не завершалось сразу
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}