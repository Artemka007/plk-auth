#pragma once
#include <memory>
#include <string>
#include <odb\database.hxx>
using namespace std;

class migration_interface {
public:
    virtual ~migration_interface() = default;
    
    virtual void create_tables() = 0;
    virtual void drop_tables() = 0;
    virtual void migrate() = 0;
    virtual bool needs_migration() = 0;
    
    static unique_ptr<migration_interface> create(odb::database& db);
};




