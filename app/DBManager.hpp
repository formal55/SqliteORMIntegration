//
//  DBManager.hpp
//  sqlite-app
//
//  Created by toyota on 2017/12/10.
//

#ifndef DBManager_hpp
#define DBManager_hpp

#include <sqlite_orm.h>
#include <mutex>

struct Employee {
    int id;
    std::string name;
    int age;
    std::string address;
    double salary;
};

struct Employee2 {
    int id;
    std::string name;
    int age;
    std::string address;
};
using namespace sqlite_orm;

namespace ORM {
    inline auto initStorage(const std::string &path) {
        return make_storage(path,
                            make_table("COMPANY",
                                       make_column("ID",
                                                   &Employee::id,
                                                   primary_key()),
                                       make_column("NAME",
                                                   &Employee::name),
                                       make_column("AGE",
                                                   &Employee::age),
                                       make_column("ADDRESS",
                                                   &Employee::address),
                                       make_column("SALARY",
                                                   &Employee::salary)),
                            make_table("COMPANY2",
                                       make_column("ID",
                                                   &Employee2::id,
                                                   primary_key()),
                                       make_column("NAME",
                                                   &Employee2::name),
                                       make_column("AGE",
                                                   &Employee2::age),
                                       make_column("ADDRESS",
                                                   &Employee2::address)));
    }
}

typedef decltype(ORM::initStorage("")) DBStorage;

class DBManager{
public:
    static DBManager *getInstance();
    void initialDB(const std::string &path);
    DBStorage getDB();
    
private:
    explicit DBManager();

private:
    static std::mutex sMutex;
    std::string mDBPath;
   
};



#endif /* DBManager_hpp */
