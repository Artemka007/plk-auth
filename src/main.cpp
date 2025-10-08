#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "./db/database.hpp"
#include "./services/user_service.hpp"
#include "./services/auth_service.hpp"
#include "./services/log_service.hpp"
#include "./services/data_export_import_service.hpp"
#include "./cli/cli_app.hpp"
#include "./cli/standard_io_handler.hpp"

// Функция для создания и настройки всех зависимостей
std::shared_ptr<CliApp> create_cli_app() {
    try {
        auto io_handler = std::make_shared<StandardIOHandler>();

        auto db = db::Database::create(
            "postgres",
            5432,
            "myapp",
            "postgres",
            "password"
        );
        
        io_handler->println("Database connection created successfully!");
        
        if (!db->test_connection()) {
            io_handler->error("Database connection test: FAILED");
            return nullptr;
        }
        
        io_handler->println("Creating database schema...");
        db->create_schema();
        
        auto dao_factory = db::DAOFactory(db);
        auto user_dao = dao_factory.create_user_dao();
        auto log_dao = dao_factory.create_log_dao();
        auto permission_dao = dao_factory.create_permission_dao();
        auto data_export_import_dao = dao_factory.create_export_import_dao();
        
        auto user_service = std::make_shared<services::UserService>(io_handler, user_dao, permission_dao);
        auto auth_service = std::make_shared<services::AuthService>(user_dao);
        auto log_service = std::make_shared<services::LogService>(log_dao);
        auto data_export_import_service = std::make_shared<services::DataExportImportService>(data_export_import_dao, io_handler);
        
        user_service->initialize_system();

        // 6. Создаем CLI компоненты
        
        // 7. Создаем и возвращаем CliApp
        return std::make_shared<CliApp>(user_service, auth_service, log_service, data_export_import_service, io_handler);
        
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