#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "./db/database.hpp"
#include "./dao/user_dao.hpp"

void print_user_info(const std::shared_ptr<models::User>& user) {
    if (!user) {
        std::cout << "User not found\n";
        return;
    }
    
    std::cout << "=== User Information ===\n";
    std::cout << "ID: " << user->id() << "\n";
    std::cout << "Full Name: " << user->full_name() << "\n";
    std::cout << "Email: " << user->email() << "\n";
    std::cout << "Phone: " << (user->phone().has_value() ? user->phone().value() : "Not set") << "\n";
    std::cout << "Active: " << (user->is_active() ? "Yes" : "No") << "\n";
    std::cout << "Password Change Required: " << (user->is_password_change_required() ? "Yes" : "No") << "\n";
    std::cout << "Created: " << user->created_at() << "\n";
    std::cout << "Last Login: " << (user->last_login_at().has_value() ? user->last_login_at().value() : "Never") << "\n";
    std::cout << "========================\n";
}

void demo_user_operations(std::shared_ptr<db::Database> db) {
    std::cout << "\n=== User Operations Demo ===\n";
    
    try {
        // Создаем DAO фабрику
        db::DAOFactory dao_factory(db);
        auto user_dao = dao_factory.create_user_dao();
        
        // 1. Создаем нового пользователя
        std::cout << "1. Creating new user...\n";
        auto new_user = std::make_shared<models::User>(
            "Ivan", 
            "Petrov", 
            "ivan.petrov@example.com"
        );
        new_user->set_password_hash("hashed_password_123");
        new_user->set_phone("+79161234567");
        
        if (user_dao->save(new_user)) {
            std::cout << "✅ User created successfully! ID: " << new_user->id() << "\n";
        } else {
            std::cout << "❌ Failed to create user\n";
        }
        
        // 2. Ищем пользователя по email
        std::cout << "\n2. Searching user by email...\n";
        auto found_user = user_dao->find_by_email("ivan.petrov@example.com");
        if (found_user) {
            std::cout << "✅ User found by email:\n";
            print_user_info(found_user);
        } else {
            std::cout << "❌ User not found by email\n";
        }
        
        // 3. Ищем пользователя по ID
        std::cout << "\n3. Searching user by ID...\n";
        auto user_by_id = user_dao->find_by_id(new_user->id());
        if (user_by_id) {
            std::cout << "✅ User found by ID:\n";
            print_user_info(user_by_id);
        } else {
            std::cout << "❌ User not found by ID\n";
        }
        
        // 4. Обновляем пользователя
        std::cout << "\n4. Updating user...\n";
        new_user->set_first_name("Ivan Updated");
        new_user->set_patronymic("Sergeevich");
        if (user_dao->update(new_user)) {
            std::cout << "✅ User updated successfully!\n";
            
            // Проверяем обновление
            auto updated_user = user_dao->find_by_id(new_user->id());
            if (updated_user) {
                std::cout << "Updated user info:\n";
                print_user_info(updated_user);
            }
        } else {
            std::cout << "❌ Failed to update user\n";
        }
        
        // 5. Создаем еще одного пользователя для демонстрации
        std::cout << "\n5. Creating additional user...\n";
        auto second_user = std::make_shared<models::User>(
            "Maria",
            "Ivanova", 
            "maria.ivanova@example.com"
        );
        second_user->set_password_hash("another_hashed_password");
        second_user->set_patronymic("Petrovna");
        
        if (user_dao->save(second_user)) {
            std::cout << "✅ Second user created! ID: " << second_user->id() << "\n";
        }
        
        // 6. Получаем всех пользователей
        std::cout << "\n6. Listing all users...\n";
        auto all_users = user_dao->find_all();
        std::cout << "Total users in database: " << all_users.size() << "\n";
        for (const auto& user : all_users) {
            std::cout << " - " << user->full_name() << " (" << user->email() << ")\n";
        }
        
        // 7. Поиск по имени
        std::cout << "\n7. Searching users by name...\n";
        auto users_by_name = user_dao->find_by_name("Ivan", "Petrov");
        std::cout << "Found " << users_by_name.size() << " users with name 'Ivan Petrov'\n";
        
        // 8. Демонстрация поиска несуществующего пользователя
        std::cout << "\n8. Searching for non-existent user...\n";
        auto non_existent = user_dao->find_by_email("nonexistent@example.com");
        if (!non_existent) {
            std::cout << "✅ Correctly returned null for non-existent user\n";
        }
        
        // 9. Тестируем смену пароля
        std::cout << "\n9. Testing password change...\n";
        if (user_dao->change_password(new_user, "new_secure_password_hash")) {
            std::cout << "✅ Password changed successfully\n";
        }
        
        // 10. Тестируем обновление времени входа
        std::cout << "\n10. Testing last login update...\n";
        if (user_dao->update_last_login(new_user)) {
            std::cout << "✅ Last login time updated\n";
            
            // Проверяем обновление
            auto user_with_login = user_dao->find_by_id(new_user->id());
            if (user_with_login && user_with_login->last_login_at().has_value()) {
                std::cout << "Last login at: " << user_with_login->last_login_at().value() << "\n";
            }
        }
        
        std::cout << "\n🎉 All user operations completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error in user operations: " << e.what() << "\n";
    }
}

int main() {
    std::cout << "Starting C++ PostgreSQL Application...\n";
    
    try {
        // Создаем подключение к базе данных
        auto db = db::Database::create(
            "postgres_db",  // host (используйте "postgres" если в Docker)
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
            std::cout << "✅ Database schema created successfully!\n";
        } else {
            std::cout << "⚠️  Schema creation failed or tables already exist\n";
        }
        
        // Демонстрация транзакции
        std::cout << "Testing transaction...\n";
        try {
            auto txn = db->begin_transaction();
            std::cout << "✅ Transaction started successfully\n";
            txn->commit();
            std::cout << "✅ Transaction committed successfully\n";
        } catch (const std::exception& e) {
            std::cout << "❌ Transaction test failed: " << e.what() << "\n";
        }
        
        // Запускаем демонстрацию операций с пользователями
        demo_user_operations(db);
        
        std::cout << "\n🎊 Application started successfully!\n";
        std::cout << "Press Ctrl+C to exit...\n";
        
        // Простой цикл чтобы приложение не завершалось сразу
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::cout << "Application is still running...\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}