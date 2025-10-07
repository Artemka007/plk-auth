#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "./db/database.hpp"
#include "./services/user_service.hpp"
#include "./services/auth_service.hpp"
#include "./services/log_service.hpp"
#include "./cli/cli_app.hpp"
#include "./cli/io_handler.hpp"

// Функция для создания и настройки всех зависимостей
std::shared_ptr<CliApp> create_cli_app() {
    try {
        // 1. Создаем подключение к базе данных
        auto db = db::Database::create(
            "postgres",  // host
            5432,        // port
            "myapp",     // database
            "postgres",  // user
            "password"   // password
        );
        
        std::cout << "✅ Database connection created successfully!\n";
        
        // 2. Тестируем подключение
        if (!db->test_connection()) {
            std::cout << "❌ Database connection test: FAILED\n";
            return nullptr;
        }
        std::cout << "✅ Database connection test: SUCCESS\n";
        
        // 3. Создаем схему базы данных (если нужно)
        std::cout << "🗃️ Creating database schema...\n";
        db->create_schema();
        
        // 4. Создаем DAO фабрику и DAO объекты
        auto dao_factory = db::DAOFactory(db);
        auto user_dao = dao_factory.create_user_dao();
        auto log_dao = dao_factory.create_log_dao();
        
        // 5. Создаем сервисы
        auto user_service = std::make_shared<services::UserService>(user_dao);
        auto auth_service = std::make_shared<services::AuthService>(user_dao);
        auto log_service = std::make_shared<services::LogService>(log_dao);
        
        // 6. Создаем CLI компоненты
        auto io_handler = std::make_shared<IOHandler>();
        
        // 7. Создаем и возвращаем CliApp
        return std::make_shared<CliApp>(user_service, auth_service, log_service, io_handler);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error creating CLI app: " << e.what() << "\n";
        return nullptr;
    }
}

int main() {
    std::cout << "🚀 Starting C++ PostgreSQL CLI Application...\n";
    
    try {
        // Создаем CLI приложение
        auto cli_app = create_cli_app();
        
        if (!cli_app) {
            std::cerr << "❌ Failed to initialize CLI application\n";
            return 1;
        }
        
        std::cout << "✅ CLI application initialized successfully!\n";
        std::cout << "💻 Type 'help' to see available commands\n";
        
        // Запускаем главный цикл приложения
        cli_app->Run();
        
        std::cout << "👋 Application finished successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Critical error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}