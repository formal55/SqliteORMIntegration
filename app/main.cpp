#include <sqlite_orm.h>
#include <iostream>
#include <typeinfo>
#include "DBManager.hpp"


int main(){
    printf("Hello world\n");
    
    DBManager::getInstance()->initialDB("myggg.sqlite");
    DBStorage storage = DBManager::getInstance()->getDB();
    
    Employee james{
        1,
        "James",
        24,
        "Houston",
        10000.00,
    };

    storage.replace(james);

    Employee james1{
        1,
        "James",
        30,
        "Houston",
        99999,
    };

    storage.replace(james1);

    for(auto &hero : storage.iterate<Employee>()) {
        std::cout << "hero = " << storage.dump(hero) << std::endl;
    }

    return 0;
}
