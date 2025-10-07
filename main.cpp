#include <iostream>
#include <pqxx/pqxx>

int main() {
    try {
        // Connect to the database
        // Replace with your PostgreSQL connection string
        pqxx::connection c("dbname=myapp user=postgres password=q1w2e3r4 host=postgres_db port=5432");
        std::cout << "Connected to " << c.dbname() << std::endl;

        // Start a transaction
        pqxx::work txn(c);

        // Execute a query
        pqxx::result r = txn.exec("SELECT version()");

        // Print the result
        for (const auto& row : r) {
            for (const auto& field : row) {
                std::cout << field.as<std::string>() << std::endl;
            }
        }

        // Commit the transaction
        txn.commit();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << "Password: " << "aaaa" << std::endl;
        return 1;
    }
    return 0;
}