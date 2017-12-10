//
//  tests.cpp
//  CPPTest
//
//  Created by John Zakharov on 05.01.17.
//  Copyright © 2017 John Zakharov. All rights reserved.
//

//#include "tests.hpp"

#include "sqlite_orm.h"

#include <cassert>
#include <vector>
#include <string>
#include <iostream>

using namespace sqlite_orm;

using std::cout;
using std::endl;

void testForeignKey() {
    cout << __func__ << endl;
    
    struct Location {
        int id;
        std::string place;
        std::string country;
        std::string city;
        int distance;
        
    };
    
    struct Visit {
        int id;
        std::shared_ptr<int> location;
        std::shared_ptr<int> user;
        int visited_at;
        uint8_t mark;
    };
    
    //  this case didn't compile on linux until `typedef constraints_type` was added to `foreign_key_t`
    auto storage = make_storage(":memory:",
                                make_table(
                                           "location",
                                           make_column("id", &Location::id, primary_key()),
                                           make_column("place", &Location::place),
                                           make_column("country", &Location::country),
                                           make_column("city", &Location::city),
                                           make_column("distance", &Location::distance)
                                           ),
                                make_table(
                                           "visit",
                                           make_column("id", &Visit::id, primary_key()),
                                           make_column("location", &Visit::location),
                                           make_column("user", &Visit::user),
                                           make_column("visited_at", &Visit::visited_at),
                                           make_column("mark", &Visit::mark),
                                           foreign_key(&Visit::location).references(&Location::id)
                                           )
                                );
    storage.sync_schema();
    
    int fromDate = int(time(nullptr));
    int toDate = int(time(nullptr));
    int toDistance = 100;
    auto id = 10;
    storage.select(columns(&Visit::mark, &Visit::visited_at, &Location::place),
                   inner_join<Location>(on(is_equal(&Visit::location, &Location::id))),
                   where(is_equal(&Visit::user, id) and
                         greater_than(&Visit::visited_at, fromDate) and
                         lesser_than(&Visit::visited_at, toDate) and
                         lesser_than(&Location::distance, toDistance)
                         ),
                   order_by(&Visit::visited_at));
}

//  appeared after #57
void testForeignKey2() {
    cout << __func__ << endl;
    
//    namespace testnamespace {
    
        class test1 {
        public:
            // Constructors
            test1() {};
            
            // Variables
            int id;
            std::string val1;
            std::string val2;
        };
        
        class test2 {
        public:
            // Constructors
            test2() {};
            
            // Variables
            int id;
            int fk_id;
            std::string val1;
            std::string val2;
        };
//    }
    
    auto table1 = make_table("test_1",
                             make_column("id",
                                         &test1::id,
                                         primary_key()),
                             make_column("val1",
                                         &test1::val1),
                             make_column("val2",
                                         &test1::val2));
    
    auto table2 = make_table("test_2",
                             make_column("id",
                                         &test2::id,
                                         primary_key()),
                             make_column("fk_id",
                                         &test2::fk_id),
                             make_column("val1",
                                         &test2::val1),
                             make_column("val2",
                                         &test2::val2),
                             foreign_key(&test2::fk_id).references(&test1::id));
    
    auto storage = make_storage("test.sqlite",
                                table1,
                                table2);
    
    storage.sync_schema();
    
    
    test1 t1;
    t1.val1 = "test";
    t1.val2 = "test";
    storage.insert(t1);
    
    test1 t1_copy;
    t1_copy.val1 = "test";
    t1_copy.val2 = "test";
    storage.insert(t1_copy);
    
    test2 t2;
    t2.fk_id = 1;
    t2.val1 = "test";
    t2.val2 = "test";
    storage.insert(t2);
    
    t2.fk_id = 2;
    
    storage.update(t2);
}

void testTypeParsing() {
    cout << __func__ << endl;
    
    //  int
    assert(*to_sqlite_type("INT") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("integeer") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("INTEGER") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("TINYINT") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("SMALLINT") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("MEDIUMINT") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("BIGINT") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("UNSIGNED BIG INT") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("INT2") == sqlite_type::INTEGER);
    assert(*to_sqlite_type("INT8") == sqlite_type::INTEGER);
    
    //  text
    assert(*to_sqlite_type("TEXT") == sqlite_type::TEXT);
    assert(*to_sqlite_type("CLOB") == sqlite_type::TEXT);
    //    assert(*to_sqlite_type("CHARACTER()") == sqlite_type::TEXT);
    for(auto i = 0; i< 255; ++i) {
        //        auto sqliteTypeString = "CHARACTER(" + std::to_string(i) + ")";
        assert(*to_sqlite_type("CHARACTER(" + std::to_string(i) + ")") == sqlite_type::TEXT);
        assert(*to_sqlite_type("VARCHAR(" + std::to_string(i) + ")") == sqlite_type::TEXT);
        assert(*to_sqlite_type("VARYING CHARACTER(" + std::to_string(i) + ")") == sqlite_type::TEXT);
        assert(*to_sqlite_type("NCHAR(" + std::to_string(i) + ")") == sqlite_type::TEXT);
        assert(*to_sqlite_type("NATIVE CHARACTER(" + std::to_string(i) + ")") == sqlite_type::TEXT);
        assert(*to_sqlite_type("NVARCHAR(" + std::to_string(i) + ")") == sqlite_type::TEXT);
    }
    
    //  blob..
    assert(*to_sqlite_type("BLOB") == sqlite_type::BLOB);
    
    //  real
    assert(*to_sqlite_type("REAL") == sqlite_type::REAL);
    assert(*to_sqlite_type("DOUBLE") == sqlite_type::REAL);
    assert(*to_sqlite_type("DOUBLE PRECISION") == sqlite_type::REAL);
    assert(*to_sqlite_type("FLOAT") == sqlite_type::REAL);
    
    assert(*to_sqlite_type("NUMERIC") == sqlite_type::REAL);
    for(auto i = 0; i < 255; ++i) {
        for(auto j = 0; j < 10; ++j) {
            assert(*to_sqlite_type("DECIMAL(" + std::to_string(i) + "," + std::to_string(j) + ")") == sqlite_type::REAL);
        }
    }
    assert(*to_sqlite_type("BOOLEAN") == sqlite_type::REAL);
    assert(*to_sqlite_type("DATE") == sqlite_type::REAL);
    assert(*to_sqlite_type("DATETIME") == sqlite_type::REAL);
    
    
    
    assert(type_is_nullable<bool>::value == false);
    assert(type_is_nullable<char>::value == false);
    assert(type_is_nullable<unsigned char>::value == false);
    assert(type_is_nullable<signed char>::value == false);
    assert(type_is_nullable<short>::value == false);
    assert(type_is_nullable<unsigned short>::value == false);
    assert(type_is_nullable<int>::value == false);
    assert(type_is_nullable<unsigned int>::value == false);
    assert(type_is_nullable<long>::value == false);
    assert(type_is_nullable<unsigned long>::value == false);
    assert(type_is_nullable<long long>::value == false);
    assert(type_is_nullable<unsigned long long>::value == false);
    assert(type_is_nullable<float>::value == false);
    assert(type_is_nullable<double>::value == false);
    assert(type_is_nullable<long double>::value == false);
    assert(type_is_nullable<long double>::value == false);
    assert(type_is_nullable<std::string>::value == false);
    assert(type_is_nullable<std::shared_ptr<int>>::value == true);
    assert(type_is_nullable<std::shared_ptr<std::string>>::value == true);
    assert(type_is_nullable<std::unique_ptr<int>>::value == true);
    assert(type_is_nullable<std::unique_ptr<std::string>>::value == true);

}

/**
 *  this is the deal: assume we have a `users` table with schema
 *  `CREATE TABLE users (id INTEGER NOT NULL PRIMARY KEY, name TEXT NOT NULL, category_id INTEGER, surname TEXT)`.
 *  We create a storage and insert several objects. Next we simulate schema changing (app update for example): create
 *  another storage with a new schema partial of previous one: `CREATE TABLE users (id INTEGER NOT NULL PRIMARY KEY, name TEXT NOT NULL)`.
 *  Next we call `sync_schema(true)` and assert that all users are saved. This test tests whether REMOVE COLUMN imitation works well.
 */
void testSyncSchema() {
    cout << __func__ << endl;
    
    //  this is an old version of user..
    struct UserBefore {
        int id;
        std::string name;
        std::shared_ptr<int> categoryId;
        std::shared_ptr<std::string> surname;
    };
    
    //  this is an new version of user..
    struct UserAfter {
        int id;
        std::string name;
    };
    
    //  create an old storage..
    auto filename = "sync_schema_text.sqlite";
    auto storage = make_storage(filename,
                                make_table("users",
                                           make_column("id",
                                                       &UserBefore::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &UserBefore::name),
                                           make_column("category_id",
                                                       &UserBefore::categoryId),
                                           make_column("surname",
                                                       &UserBefore::surname)));
    
    //  sync in case if it is first launch..
    auto syncSchemaSimulationRes = storage.sync_schema_simulate();
    auto syncSchemaRes = storage.sync_schema();
    
    assert(syncSchemaRes == syncSchemaSimulationRes);
    
    //  remove old users in case the test was launched before..
    storage.remove_all<UserBefore>();
    
    //  create c++ objects to insert into table..
    std::vector<UserBefore> usersToInsert {
        { -1, "Michael", nullptr, std::make_shared<std::string>("Scofield") },
        { -1, "Lincoln", std::make_shared<int>(4), std::make_shared<std::string>("Burrows") },
        { -1, "Sucre", nullptr, nullptr },
        { -1, "Sara", std::make_shared<int>(996), std::make_shared<std::string>("Tancredi") },
        { -1, "John", std::make_shared<int>(100500), std::make_shared<std::string>("Abruzzi") },
        { -1, "Brad", std::make_shared<int>(65), nullptr },
        { -1, "Paul", std::make_shared<int>(65), nullptr },
    };
    
    for(auto &user : usersToInsert) {
        auto insertedId = storage.insert(user);
        user.id = insertedId;
    }
    
    //  assert count first cause we will be asserting row by row next..
    assert(storage.count<UserBefore>() == usersToInsert.size());
    
    //  now we create new storage with partial schema..
    auto newStorage = make_storage(filename,
                                   make_table("users",
                                              make_column("id",
                                                          &UserAfter::id,
                                                          primary_key()),
                                              make_column("name",
                                                          &UserAfter::name)));
    
    syncSchemaSimulationRes = newStorage.sync_schema_simulate(true);
    
    //  now call `sync_schema` with argument `preserve` as `true`. It will retain data in case `sqlite_orm` needs to remove a column..
    syncSchemaRes = newStorage.sync_schema(true);
    assert(syncSchemaRes.size() == 1);
    assert(syncSchemaRes.begin()->second == sync_schema_result::old_columns_removed);
    assert(syncSchemaSimulationRes == syncSchemaRes);
    
    //  get all users after syncing schema..
    auto usersFromDb = newStorage.get_all<UserAfter>(order_by(&UserAfter::id));
    
    assert(usersFromDb.size() == usersToInsert.size());
    
    for(auto i = 0; i < usersFromDb.size(); ++i) {
        auto &userFromDb = usersFromDb[i];
        auto &oldUser = usersToInsert[i];
        assert(userFromDb.id == oldUser.id);
        assert(userFromDb.name == oldUser.name);
    }
    
    auto usersCountBefore = newStorage.count<UserAfter>();
    
    syncSchemaSimulationRes = newStorage.sync_schema_simulate();
    syncSchemaRes = newStorage.sync_schema();
    assert(syncSchemaRes == syncSchemaSimulationRes);
    
    auto usersCountAfter = newStorage.count<UserAfter>();
    assert(usersCountBefore == usersCountAfter);
    
    //  test select..
    auto ids = newStorage.select(&UserAfter::id);
    auto users = newStorage.get_all<UserAfter>();
    decltype(ids) idsFromGetAll;
    idsFromGetAll.reserve(users.size());
    std::transform(users.begin(),
                   users.end(),
                   std::back_inserter(idsFromGetAll),
                   [=](auto &user) {
                       return user.id;
                   });
    assert(std::equal(ids.begin(),
                      ids.end(),
                      idsFromGetAll.begin(),
                      idsFromGetAll.end()));
    
}

void testSelect() {
    cout << __func__ << endl;
    
    sqlite3 *db;
    auto dbFileName = "test.db";
    auto rc = sqlite3_open(dbFileName, &db);
    assert(rc == SQLITE_OK);
    auto sql = "CREATE TABLE IF NOT EXISTS WORDS("
    "ID INTEGER PRIMARY        KEY AUTOINCREMENT      NOT NULL,"
    "CURRENT_WORD          TEXT     NOT NULL,"
    "BEFORE_WORD           TEXT     NOT NULL,"
    "AFTER_WORD            TEXT     NOT NULL,"
    "OCCURANCES            INT      NOT NULL);";
    
    char *errMsg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    assert(rc == SQLITE_OK);
    
    sqlite3_stmt *stmt;
    
    //  delete previous words. This command is excess in travis or other docker based CI tools
    //  but it is required on local machine
    sql = "DELETE FROM WORDS";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    assert(rc == SQLITE_OK);
    
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        cout << sqlite3_errmsg(db) << endl;
        assert(0);
    }
    sqlite3_finalize(stmt);
    
    sql = "INSERT INTO WORDS (CURRENT_WORD, BEFORE_WORD, AFTER_WORD, OCCURANCES) VALUES(?, ?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    assert(rc == SQLITE_OK);
    
    //  INSERT [ ID, 'best', 'behaviour', 'hey', 5 ]
    
    sqlite3_bind_text(stmt, 1, "best", -1, nullptr);
    sqlite3_bind_text(stmt, 2, "behaviour", -1, nullptr);
    sqlite3_bind_text(stmt, 3, "hey", -1, nullptr);
    sqlite3_bind_int(stmt, 4, 5);
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        cout << sqlite3_errmsg(db) << endl;
        assert(0);
    }
    sqlite3_finalize(stmt);
    
    auto firstId = sqlite3_last_insert_rowid(db);
    
    //  INSERT [ ID, 'corruption', 'blood', 'brothers', 15 ]
    
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    assert(rc == SQLITE_OK);
    sqlite3_bind_text(stmt, 1, "corruption", -1, nullptr);
    sqlite3_bind_text(stmt, 2, "blood", -1, nullptr);
    sqlite3_bind_text(stmt, 3, "brothers", -1, nullptr);
    sqlite3_bind_int(stmt, 4, 15);
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        cout << sqlite3_errmsg(db) << endl;
        assert(0);
    }
    sqlite3_finalize(stmt);
    
    auto secondId = sqlite3_last_insert_rowid(db);
    
    sqlite3_close(db);
    
    struct Word {
        int id;
        std::string currentWord;
        std::string beforeWord;
        std::string afterWord;
        int occurances;
    };
    
    auto storage = make_storage(dbFileName,
                                make_table("WORDS",
                                           make_column("ID",
                                                       &Word::id,
                                                       primary_key(),
                                                       autoincrement()),
                                           make_column("CURRENT_WORD",
                                                       &Word::currentWord),
                                           make_column("BEFORE_WORD",
                                                       &Word::beforeWord),
                                           make_column("AFTER_WORD",
                                                       &Word::afterWord),
                                           make_column("OCCURANCES",
                                                       &Word::occurances)));
    
    storage.sync_schema();  //  sync schema must not alter any data cause schemas are the same
    
    assert(storage.count<Word>() == 2);
    
    auto firstRow = storage.get_no_throw<Word>(firstId);
    assert(firstRow);
    assert(firstRow->currentWord == "best");
    assert(firstRow->beforeWord == "behaviour");
    assert(firstRow->afterWord == "hey");
    assert(firstRow->occurances == 5);
    
    auto secondRow = storage.get_no_throw<Word>(secondId);
    assert(secondRow);
    assert(secondRow->currentWord == "corruption");
    assert(secondRow->beforeWord == "blood");
    assert(secondRow->afterWord == "brothers");
    assert(secondRow->occurances == 15);
    
    auto cols = columns(&Word::id,
                        &Word::currentWord,
                        &Word::beforeWord,
                        &Word::afterWord,
                        &Word::occurances);
    auto rawTuples = storage.select(cols, where(eq(&Word::id, firstId)));
    assert(rawTuples.size() == 1);
    
    {
        auto &firstTuple = rawTuples.front();
        assert(std::get<0>(firstTuple) == firstId);
        assert(std::get<1>(firstTuple) == "best");
        assert(std::get<2>(firstTuple) == "behaviour");
        assert(std::get<3>(firstTuple) == "hey");
        assert(std::get<4>(firstTuple) == 5);
    }
    
    rawTuples = storage.select(cols, where(eq(&Word::id, secondId)));
    assert(rawTuples.size() == 1);
    
    {
        auto &secondTuple = rawTuples.front();
        assert(std::get<0>(secondTuple) == secondId);
        assert(std::get<1>(secondTuple) == "corruption");
        assert(std::get<2>(secondTuple) == "blood");
        assert(std::get<3>(secondTuple) == "brothers");
        assert(std::get<4>(secondTuple) == 15);
    }
    
    auto ordr = order_by(&Word::id);
    
    auto idsOnly = storage.select(&Word::id, ordr);
    assert(idsOnly.size() == 2);
    
    assert(idsOnly[0] == firstId);
    assert(idsOnly[1] == secondId);
    
    auto currentWordsOnly = storage.select(&Word::currentWord, ordr);
    assert(currentWordsOnly.size() == 2);
    
    assert(currentWordsOnly[0] == "best");
    assert(currentWordsOnly[1] == "corruption");
    
    auto beforeWordsOnly = storage.select(&Word::beforeWord, ordr);
    assert(beforeWordsOnly.size() == 2);
    
    assert(beforeWordsOnly[0] == "behaviour");
    assert(beforeWordsOnly[1] == "blood");
    
    auto afterWordsOnly = storage.select(&Word::afterWord, ordr);
    assert(afterWordsOnly.size() == 2);
    
    assert(afterWordsOnly[0] == "hey");
    assert(afterWordsOnly[1] == "brothers");
    
    auto occurencesOnly = storage.select(&Word::occurances, ordr);
    assert(occurencesOnly.size() == 2);
    
    assert(occurencesOnly[0] == 5);
    assert(occurencesOnly[1] == 15);
    
    //  test update_all with the same storage
    
    storage.update_all(set(&Word::currentWord, "ototo"),
                       where(is_equal(&Word::id, firstId)));
    
    assert(storage.get<Word>(firstId).currentWord == "ototo");
    
}

void testRemove() {
    cout << __func__ << endl;
    
    struct Object {
        int id;
        std::string name;
    };
    
    auto storage = make_storage("test_remove.sqlite",
                                make_table("objects",
                                           make_column("id",
                                                       &Object::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &Object::name)));
    storage.sync_schema();
    storage.remove_all<Object>();
    
    auto id1 = storage.insert(Object{ 0, "Skillet"});
    assert(storage.count<Object>() == 1);
    storage.remove<Object>(id1);
    assert(storage.count<Object>() == 0);
    
}

void testInsert() {
    cout << __func__ << endl;
    
    struct Object {
        int id;
        std::string name;
    };
    
    struct ObjectWithoutRowid {
        int id;
        std::string name;
    };
    
    auto storage = make_storage("test_insert.sqlite",
                                make_table("objects",
                                           make_column("id",
                                                       &Object::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &Object::name)),
                                make_table("objects_without_rowid",
                                           make_column("id",
                                                       &ObjectWithoutRowid::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &ObjectWithoutRowid::name)).without_rowid());
    
    storage.sync_schema();
    storage.remove_all<Object>();
    storage.remove_all<ObjectWithoutRowid>();
    
    for(auto i = 0; i < 100; ++i) {
        storage.insert(Object{
            0,
            "Skillet",
        });
        assert(storage.count<Object>() == i + 1);
    }
    
    auto initList = {
        Object{
            0,
            "Insane",
        },
        Object{
            0,
            "Super",
        },
        Object{
            0,
            "Sun",
        },
    };
    
    cout << "inserting range" << endl;
    auto countBefore = storage.count<Object>();
    storage.insert_range(initList.begin(),
                         initList.end());
    assert(storage.count<Object>() == countBefore + initList.size());
    
    
    //  test empty container
    std::vector<Object> emptyVector;
    storage.insert_range(emptyVector.begin(),
                         emptyVector.end());
    
    //  test insert without rowid
    storage.insert(ObjectWithoutRowid{ 10, "Life" });
    assert(storage.get<ObjectWithoutRowid>(10).name == "Life");
    storage.insert(ObjectWithoutRowid{ 20, "Death" });
    assert(storage.get<ObjectWithoutRowid>(20).name == "Death");
}

void testReplace() {
    cout << __func__ << endl;
    
    struct Object {
        int id;
        std::string name;
    };
    
    auto storage = make_storage("test_replace.sqlite",
                                make_table("objects",
                                           make_column("id",
                                                       &Object::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &Object::name)));
    
    storage.sync_schema();
    storage.remove_all<Object>();
    
    storage.replace(Object{
        100,
        "Baby",
    });
    assert(storage.count<Object>() == 1);
    auto baby = storage.get<Object>(100);
    assert(baby.id == 100);
    assert(baby.name == "Baby");
    
    storage.replace(Object{
        200,
        "Time",
    });
    assert(storage.count<Object>() == 2);
    auto time = storage.get<Object>(200);
    assert(time.id == 200);
    assert(time.name == "Time");
    storage.replace(Object{
        100,
        "Ototo",
    });
    assert(storage.count<Object>() == 2);
    auto ototo = storage.get<Object>(100);
    assert(ototo.id == 100);
    assert(ototo.name == "Ototo");
    
    auto initList = {
        Object{
            300,
            "Iggy",
        },
        Object{
            400,
            "Azalea",
        },
    };
    storage.replace_range(initList.begin(), initList.end());
    assert(storage.count<Object>() == 4);
    
    //  test empty container
    std::vector<Object> emptyVector;
    storage.replace_range(emptyVector.begin(),
                          emptyVector.end());
}

void testEmptyStorage() {
    cout << __func__ << endl;
    
    auto storage = make_storage("empty.sqlite");
    storage.table_exists("table");
}

void testTransactionGuard() {
    cout << __func__ << endl;
    
    struct Object {
        int id;
        std::string name;
    };
    
    auto storage = make_storage("test_transaction_guard.sqlite",
                                make_table("objects",
                                           make_column("id",
                                                       &Object::id,
                                                       primary_key()),
                                           make_column("name",
                                                       &Object::name)));
    
    storage.sync_schema();
    storage.remove_all<Object>();
    
    storage.insert(Object{0, "Jack"});
    
    //  insert, call make a storage to cakk an exception and check that rollback was fired
    auto countBefore = storage.count<Object>();
    try{
        auto guard = storage.transaction_guard();
        
        storage.insert(Object{0, "John"});
        
        storage.get<Object>(-1);
        
        assert(false);
    }catch(...){
        auto countNow = storage.count<Object>();
        
        assert(countBefore == countNow);
    }
    
    //  check that one can call other transaction functions without exceptions
    storage.transaction([&]{return false;});
    
    //  commit explicitly and check that after exception data was saved
    countBefore = storage.count<Object>();
    try{
        auto guard = storage.transaction_guard();
        storage.insert(Object{0, "John"});
        guard.commit();
        storage.get<Object>(-1);
        assert(false);
    }catch(...){
        auto countNow = storage.count<Object>();
        
        assert(countNow == countBefore + 1);
    }
    
    //  rollback explicitly
    countBefore = storage.count<Object>();
    try{
        auto guard = storage.transaction_guard();
        storage.insert(Object{0, "Michael"});
        guard.rollback();
        storage.get<Object>(-1);
        assert(false);
    }catch(...){
        auto countNow = storage.count<Object>();
        assert(countNow == countBefore);
    }
    
    //  commit on exception
    countBefore = storage.count<Object>();
    try{
        auto guard = storage.transaction_guard();
        guard.commit_on_destroy = true;
        storage.insert(Object{0, "Michael"});
        storage.get<Object>(-1);
        assert(false);
    }catch(...){
        auto countNow = storage.count<Object>();
        assert(countNow == countBefore + 1);
    }
    
    //  work witout exception
    countBefore = storage.count<Object>();
    try{
        auto guard = storage.transaction_guard();
        guard.commit_on_destroy = true;
        storage.insert(Object{0, "Lincoln"});
        
    }catch(...){
        assert(0);
    }
    auto countNow = storage.count<Object>();
    assert(countNow == countBefore + 1);
}

/**
 *  Created by fixing https://github.com/fnc12/sqlite_orm/issues/42
 */
void testEscapeChars() {
    cout << __func__ << endl;
    
    struct Employee {
        int id;
        std::string name;
        int age;
        std::string address;
        double salary;
    };
    auto storage =  make_storage("test_escape_chars.sqlite",
                                 make_table("COMPANY",
                                            make_column("INDEX",
                                                        &Employee::id,
                                                        primary_key()),
                                            make_column("NAME",
                                                        &Employee::name),
                                            make_column("AGE",
                                                        &Employee::age),
                                            make_column("ADDRESS",
                                                        &Employee::address),
                                            make_column("SALARY",
                                                        &Employee::salary)));
    storage.sync_schema();
    storage.remove_all<Employee>();
    
    storage.insert(Employee{
        0,
        "Paul'l",
        20,
        "Sacramento 20",
        40000,
    });
    
    auto paulL = storage.get_all<Employee>(where(is_equal(&Employee::name, "Paul'l")));
    
    storage.replace(Employee{
        10,
        "Selena",
        24,
        "Florida",
        500000,
    });
    
    auto selena = storage.get<Employee>(10);
    auto selenaMaybe = storage.get_no_throw<Employee>(10);
    selena.name = "Gomez";
    storage.update(selena);
    storage.remove<Employee>(10);
}

//  appeared after #54
void testBlob() {
    cout << __func__ << endl;
    
    struct BlobData {
        std::vector<char> data;
    };
    typedef char byte;
    
    auto generateData = [](int size) -> byte* {
        auto data = (byte*)::malloc(size * sizeof(byte));
        for (int i = 0; i < size; ++i) {
            if ((i+1) % 10 == 0) {
                data[i] = 0;
            } else {
                data[i] = (rand() % 100) + 1;
            }
        }
        return data;
    };
    
    auto storage = make_storage("blob.db",
                                make_table("blob",
                                           make_column("data", &BlobData::data)));
    storage.sync_schema();
    storage.remove_all<BlobData>();
    
    auto size = 100;
    auto data = generateData(size);
    
    //  write data
    BlobData d;
    std::vector<char> v(data, data + size);
    d.data = v;
//    cout << "Write Size: " << d.data.size() << endl;
    storage.insert(d);
    
    //  read data with get_all
    {
        auto vd = storage.get_all<BlobData>();
        assert(vd.size() == 1);
        auto &blob = vd.front();
        assert(blob.data.size() == size);
        assert(std::equal(data,
                          data + size,
                          blob.data.begin()));
    }
    
    //  read data with select (single column)
    {
        auto blobData = storage.select(&BlobData::data);
        assert(blobData.size() == 1);
        auto &blob = blobData.front();
        assert(blob.size() == size);
        assert(std::equal(data,
                          data + size,
                          blob.begin()));
    }
    
    //  read data with select (multi column)
    {
        auto blobData = storage.select(columns(&BlobData::data));
        assert(blobData.size() == 1);
        auto &blob = std::get<0>(blobData.front());
        assert(blob.size() == size);
        assert(std::equal(data,
                          data + size,
                          blob.begin()));
    }
    
    free(data);
}

//  appeared after #55
void testDefaultValue() {
    cout << __func__ << endl;
    
    struct User {
        int userId;
        std::string name;
        int age;
        std::string email;
    };
    
    auto storage1 = make_storage("test_db.sqlite",
                                 make_table("User",
                                            make_column("Id",
                                                        &User::userId,
                                                        primary_key()),
                                            make_column("Name",
                                                        &User::name),
                                            make_column("Age",
                                                        &User::age)));
    storage1.sync_schema();
    storage1.remove_all<User>();
    
    auto storage2 = make_storage("test_db.sqlite",
                                 make_table("User",
                                            make_column("Id",
                                                        &User::userId,
                                                        primary_key()),
                                            make_column("Name",
                                                        &User::name),
                                            make_column("Age",
                                                        &User::age),
                                            make_column("Email",
                                                        &User::email,
                                                        default_value("example@email.com"))));
    storage2.sync_schema();
}

//  after #18
void testCompositeKey() {
    cout << __func__ << endl;
    
    struct Record
    {
        int year;
        int month;
        int amount;
    };
    
    auto recordsTableName = "records";
    auto storage = make_storage("compisite_key.db",
                                make_table(recordsTableName,
                                           make_column("year", &Record::year),
                                           make_column("month", &Record::month),
                                           make_column("amount", &Record::amount),
                                           primary_key(&Record::year, &Record::month)));
    
    storage.sync_schema();
    assert(storage.sync_schema()[recordsTableName] == sqlite_orm::sync_schema_result::already_in_sync);
    
    auto storage2 = make_storage("compisite_key2.db",
                                 make_table(recordsTableName,
                                            make_column("year", &Record::year),
                                            make_column("month", &Record::month),
                                            make_column("amount", &Record::amount),
                                            primary_key(&Record::month, &Record::year)));
    storage2.sync_schema();
    assert(storage2.sync_schema()[recordsTableName] == sqlite_orm::sync_schema_result::already_in_sync);
    
    auto storage3 = make_storage("compisite_key3.db",
                                 make_table(recordsTableName,
                                            make_column("year", &Record::year),
                                            make_column("month", &Record::month),
                                            make_column("amount", &Record::amount),
                                            primary_key(&Record::amount, &Record::month, &Record::year)));
    storage3.sync_schema();
    assert(storage3.sync_schema()[recordsTableName] == sqlite_orm::sync_schema_result::already_in_sync);
    
}

int main() {
    
    cout << "version = " << make_storage("").libversion() << endl;
    
    testTypeParsing();
    
    testSyncSchema();
    
    testInsert();
    
    testReplace();
    
    testSelect();
    
    testRemove();
    
    testEmptyStorage();
    
    testTransactionGuard();
    
    testEscapeChars();
    
    testForeignKey();
    
    testForeignKey2();
    
    testBlob();
    
    testDefaultValue();
    
    testCompositeKey();
}
