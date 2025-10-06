#include "postgres_migration.hpp"
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/connection.hxx>

using namespace odb::core;

postgres_migration::postgres_migration(odb::database& db) : db_(db){}

void postgres_migration::create_tables(){
    transaction t (db_.begin());
    create_migration_table();
    schema_catalog::create_schema(db_);
    set_schema_version(1);

    t.commit();
}

void postgres_migration::drop_tables(){
    transaction t(db_.begin());
    schema_catalog::drop_schema(db_);
    db_.execute("DROP TABLE IF EXISTS shema_migrations");
    t.commit();

}

void postgres_migration::migrate(){
    if(!needs_migration) return;
    transaction t (db_.begin());
    int current_version = get_current_schema_version();
    switch (current_version){
        case 0:
            create_tables();
            break;
        case 1:
            break;
        
    }

    t.commit();
}

bool postgres_migration::needs_migration(){
    try{
        int current = get_current_schema_version();
        return current < 1;
    }   catch(const std::exception&){
        return true;
    }
}

void postgres_migration::create_migration_table(){
    db_.execute(
        "CREATE TABLE IF NOT EXISTS shema_migrations("
        "version INTEGER PRIMARY KEY,"
        "applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")"
    );
}

int postgres_migration::get_current_schema_version(){
    try{
        odb::result <int> r(db_.query<int>(
            "SELECT version FROM schema_migrations ORDER BY version DESC LIMIT 1"
        ));
        if (r.empty()) return 0;
        return *r.begin();
    }   catch (const std::exception&){ return 0;}
}

void postgres_migration::set_schema_version(int version){
    db_.execute("DELETE FROM schema_migrations");
    db_.execute("INSERT INTO schema_migrations (version) VALUES (" + std::to_string(version) + ")");
}

std::unique_ptr<migration_interface> migration_interface::create(odb::database& db){
    return std::make_unique<postgres_migration>(db);
}