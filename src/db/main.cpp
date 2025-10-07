#include "database.hpp"
#include <iostream>

int main() {
    try {
        // Создаем подключение к PostgreSQL
        auto db = db::Database::create(
            "localhost",     // хост
            5432,           // порт
            "bdprosoft",        // база данных
            "postgres",     // пользователь
            "password"      // пароль
        );
        
        // Тестируем подключение
        if (!db->test_connection()) {
            std::cerr << "Failed to connect to database" << std::endl;
            return 1;
        }
        
        // Создаем схему
        if (!db->create_schema()) {
            std::cerr << "Failed to create schema" << std::endl;
            return 1;
        }
        
        // Создаем фабрику DAO
        db::DAOFactory factory(db);
        
        // Создаем UserDAO
        auto user_dao = factory.create_user_dao();
        
        // Используем DAO...
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}