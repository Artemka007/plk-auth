#pragma once
#include <memory>
#include <string>
#include <odb\database.hxx>
using namespace std;

class MigrationInterface {
public:
    virtual ~MigrationInterface() = default;
    
    virtual void createTables() = 0;
    virtual void dropTables() = 0;
    virtual void migrate() = 0;
    virtual bool needsMigration() = 0;
    
    static unique_ptr<MigrationInterface> create(odb::database& db);
};




