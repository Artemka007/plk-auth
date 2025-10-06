#pragma once
#include "migration_interface.hpp"
#include <odb/database.hxx>

class postgres_migration : public migration_interface {
public:
    postgres_migration(odb::database& db);
    
    void create_tables() override;
    void drop_tables() override;
    void migrate() override;
    bool needs_migration() override;

private:
    odb::database& db_;
    void create_user_table();
    void create_migration_table();
    int get_current_schema_version();
    void set_schema_version(int version);
};