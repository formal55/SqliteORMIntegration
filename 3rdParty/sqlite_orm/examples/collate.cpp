//
//  collate.cpp
//  CPPTest
//
//  Created by John Zakharov on 02.12.2017.
//  Copyright © 2017 John Zakharov. All rights reserved.
//

#include "sqlite_orm.h"

#include <string>
#include <sys/time.h>
#include <iostream>

using std::cout;
using std::endl;

struct User {
    int id;
    std::string name;
    time_t createdAt;
};

int main(int argc, char** argv) {
    
    using namespace sqlite_orm;
    auto storage = make_storage("collate.sqlite",
                                make_table("users",
                                           make_column("id",
                                                       &User::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &User::name),
                                           make_column("created_at",
                                                       &User::createdAt)));
    storage.sync_schema();
    storage.remove_all<User>();
    
    storage.insert(User{
        0,
        "Lil Kim",
        time(nullptr),
    });
    storage.insert(User{
        0,
        "lil kim",
        time(nullptr),
    });
    storage.insert(User{
        0,
        "Nicki Minaj",
        time(nullptr),
    });
    
    //  SELECT COUNT(*) FROM users WHERE name = 'lil kim'
    auto preciseLilKimsCount = storage.count<User>(where(is_equal(&User::name, "lil kim")));
    cout << "preciseLilKimsCount = " << preciseLilKimsCount << endl;
    
    //  SELECT COUNT(*) FROM users WHERE name = 'lil kim' COLLATE NOCASE
    auto nocaseCount = storage.count<User>(where(is_equal(&User::name, "lil kim").collate_nocase()));
    cout << "nocaseCount = " << nocaseCount << endl;
    
    //  SELECT COUNT(*) FROM users
    cout << "total users count = " << storage.count<User>() << endl;
    
    return 0;
}
