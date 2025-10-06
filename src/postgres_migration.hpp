#pragma once
#include "migration_interface.hpp"
#include <odb/database.hxx>

class PostgresMigration : public MigrationInterface {
public:
    PostgresMigration(odb::database& db);
    
    void createTables() override;
    void dropTables() override;
    void migrate() override;
    bool needsMigration() override;

private:
    odb::database& db_;
    void createUserTable();
    void createMigrationTable();
    int getCurrentSchemaVersion();
    void setSchemaVersion(int version);
};