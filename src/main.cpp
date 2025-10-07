#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include "./db/database.hpp"
#include "./dao/user_dao.hpp"
#include "./models/user.hpp"
#include "./models/user_role.hpp"

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

class CommandProcessor {
private:
    std::shared_ptr<db::Database> db_;
    std::shared_ptr<dao::UserDAO> user_dao_;
    
public:
    CommandProcessor(std::shared_ptr<db::Database> db) : db_(db) {
        auto dao_factory = db::DAOFactory(db);
        user_dao_ = dao_factory.create_user_dao();
    }
    
    void process_command(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "help" || cmd == "?") {
            show_help();
        } else if (cmd == "exit" || cmd == "quit") {
            std::cout << "Goodbye! 👋\n";
            exit(0);
        } else if (cmd == "users") {
            list_users();
        } else if (cmd == "create") {
            create_user();
        } else if (cmd == "find") {
            std::string email;
            iss >> email;
            find_user(email);
        } else if (cmd == "delete") {
            std::string email;
            iss >> email;
            delete_user(email);
        } else if (cmd == "demo") {
            run_demo();
        } else if (cmd == "schema") {
            create_schema();
        } else if (cmd == "test") {
            test_connection();
        } else if (cmd == "clear" || cmd == "cls") {
            clear_screen();
        } else if (cmd == "status") {
            show_status();
        } else if (cmd == "roles") {
            list_roles();
        } else if (cmd == "assign-role") {
            std::string email, role;
            iss >> email >> role;
            assign_role(email, role);
        } else if (cmd.empty()) {
            // Пустая команда - ничего не делаем
        } else {
            std::cout << "❌ Unknown command: " << cmd << "\n";
            std::cout << "Type 'help' for available commands\n";
        }
    }

private:
    void show_help() {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "🎮 C++ PostgreSQL App - Available Commands\n";
        std::cout << std::string(50, '=') << "\n";
        std::cout << "📊 USERS:\n";
        std::cout << "  users                    - List all users\n";
        std::cout << "  create                   - Create new user\n";
        std::cout << "  find <email>             - Find user by email\n";
        std::cout << "  delete <email>           - Delete user by email\n";
        std::cout << "\n⚙️  SYSTEM:\n";
        std::cout << "  schema                   - Create database schema\n";
        std::cout << "  test                     - Test database connection\n";
        std::cout << "  status                   - Show system status\n";
        std::cout << "  demo                     - Run demo operations\n";
        std::cout << "\n🎯 ROLES:\n";
        std::cout << "  roles                    - List all roles\n";
        std::cout << "  assign-role <email> <role> - Assign role to user\n";
        std::cout << "\n🔧 UTILS:\n";
        std::cout << "  clear / cls              - Clear screen\n";
        std::cout << "  help / ?                 - Show this help\n";
        std::cout << "  exit / quit              - Exit application\n";
        std::cout << std::string(50, '=') << "\n";
    }
    
    void list_users() {
        try {
            auto users = user_dao_->find_all();
            
            std::cout << "\n👥 Users (" << users.size() << "):\n";
            std::cout << std::string(80, '-') << "\n";
            std::cout << std::left 
                      << std::setw(36) << "ID"
                      << std::setw(20) << "Name" 
                      << std::setw(25) << "Email"
                      << std::setw(8) << "Status"
                      << "\n";
            std::cout << std::string(80, '-') << "\n";
            
            for (const auto& user : users) {
                std::cout << std::left 
                          << std::setw(36) << (user->id().length() > 35 ? user->id().substr(0, 35) + "..." : user->id())
                          << std::setw(20) << (user->full_name().length() > 19 ? user->full_name().substr(0, 19) + "..." : user->full_name())
                          << std::setw(25) << (user->email().length() > 24 ? user->email().substr(0, 24) + "..." : user->email())
                          << std::setw(8) << (user->is_active() ? "✅" : "❌")
                          << "\n";
            }
            std::cout << std::string(80, '-') << "\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ Error listing users: " << e.what() << "\n";
        }
    }
    
    void create_user() {
        try {
            std::string first_name, last_name, email, phone;
            
            std::cout << "\n👤 Create New User:\n";
            std::cout << "First Name: ";
            std::getline(std::cin, first_name);
            std::cout << "Last Name: ";
            std::getline(std::cin, last_name);
            std::cout << "Email: ";
            std::getline(std::cin, email);
            std::cout << "Phone (optional): ";
            std::getline(std::cin, phone);
            
            if (first_name.empty() || last_name.empty() || email.empty()) {
                std::cout << "❌ First name, last name and email are required!\n";
                return;
            }
            
            auto user = std::make_shared<models::User>(first_name, last_name, email);
            user->set_password_hash("temp_password_hash_123");
            if (!phone.empty()) {
                user->set_phone(phone);
            }
            
            if (user_dao_->save(user)) {
                std::cout << "✅ User created successfully! ID: " << user->id() << "\n";
            } else {
                std::cout << "❌ Failed to create user (possibly duplicate email)\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Error creating user: " << e.what() << "\n";
        }
    }
    
    void find_user(const std::string& email) {
        if (email.empty()) {
            std::cout << "❌ Usage: find <email>\n";
            return;
        }
        
        try {
            auto user = user_dao_->find_by_email(email);
            
            if (user) {
                std::cout << "\n🔍 User Found:\n";
                print_user_info(user);
                
                // Показываем роли пользователя
                auto roles = user_dao_->user_roles(user);
                if (!roles.empty()) {
                    std::cout << "🎯 Roles: ";
                    for (const auto& role : roles) {
                        std::cout << role->name() << " ";
                    }
                    std::cout << "\n";
                }
            } else {
                std::cout << "❌ User not found with email: " << email << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Error finding user: " << e.what() << "\n";
        }
    }
    
    void delete_user(const std::string& email) {
        if (email.empty()) {
            std::cout << "❌ Usage: delete <email>\n";
            return;
        }
        
        try {
            auto user = user_dao_->find_by_email(email);
            if (!user) {
                std::cout << "❌ User not found with email: " << email << "\n";
                return;
            }
            
            std::cout << "⚠️  Are you sure you want to delete user: " << user->full_name() << " (" << email << ")? (y/N): ";
            std::string confirm;
            std::getline(std::cin, confirm);
            
            if (confirm == "y" || confirm == "Y") {
                if (user_dao_->delete_by_id(user->id())) {
                    std::cout << "✅ User deleted successfully!\n";
                } else {
                    std::cout << "❌ Failed to delete user\n";
                }
            } else {
                std::cout << "Deletion cancelled.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Error deleting user: " << e.what() << "\n";
        }
    }
    
    void create_schema() {
        try {
            std::cout << "🗃️  Creating database schema...\n";
            if (db_->create_schema()) {
                std::cout << "✅ Schema created successfully!\n";
            } else {
                std::cout << "⚠️  Schema creation failed or tables already exist\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Error creating schema: " << e.what() << "\n";
        }
    }
    
    void test_connection() {
        try {
            if (db_->test_connection()) {
                std::cout << "✅ Database connection: OK\n";
                std::cout << "📊 Connection info: " << db_->get_connection_info() << "\n";
            } else {
                std::cout << "❌ Database connection: FAILED\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "❌ Error testing connection: " << e.what() << "\n";
        }
    }
    
    void show_status() {
        try {
            auto users = user_dao_->find_all();
            auto active_users = user_dao_->find_active_users();
            
            std::cout << "\n📊 System Status:\n";
            std::cout << std::string(30, '-') << "\n";
            std::cout << "Database: " << (db_->test_connection() ? "✅ Connected" : "❌ Disconnected") << "\n";
            std::cout << "Total Users: " << users.size() << "\n";
            std::cout << "Active Users: " << active_users.size() << "\n";
            std::cout << "Connection: " << db_->get_connection_info() << "\n";
            std::cout << std::string(30, '-') << "\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ Error getting status: " << e.what() << "\n";
        }
    }
    
    void list_roles() {
        // Заглушка - нужно реализовать RoleDAO
        std::cout << "\n🎯 Available Roles:\n";
        std::cout << " - ADMIN (System Administrator)\n";
        std::cout << " - USER (Regular User)\n";
        std::cout << "💡 Note: Role management will be implemented soon!\n";
    }
    
    void assign_role(const std::string& email, const std::string& role) {
        if (email.empty() || role.empty()) {
            std::cout << "❌ Usage: assign-role <email> <role>\n";
            return;
        }
        
        std::cout << "💡 Role assignment feature coming soon!\n";
        std::cout << "Would assign role '" << role << "' to user with email '" << email << "'\n";
    }
    
    void run_demo() {
        std::cout << "\n🎬 Starting demo operations...\n";
        demo_user_operations(db_);
    }
    
    void clear_screen() {
        std::cout << "\033[2J\033[1;1H"; // ANSI escape codes для очистки экрана
        std::cout << "🔄 Screen cleared!\n";
    }
};

void interactive_mode(std::shared_ptr<db::Database> db) {
    CommandProcessor processor(db);
    
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "🚀 C++ PostgreSQL Application Started!\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Type 'help' for available commands\n";
    
    std::string command;
    while (true) {
        std::cout << "\n💻 > ";
        std::getline(std::cin, command);
        processor.process_command(command);
    }
}

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
        
        std::cout << "✅ Database connection created successfully!\n";
        
        // Тестируем подключение
        if (db->test_connection()) {
            std::cout << "✅ Database connection test: SUCCESS\n";
        } else {
            std::cout << "❌ Database connection test: FAILED\n";
            return 1;
        }
        
        // Создаем таблицы
        std::cout << "🗃️  Creating database schema...\n";
        db->create_schema();
        
        // Запускаем интерактивный режим
        interactive_mode(db);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}