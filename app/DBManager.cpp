//
//  DBManager.cpp
//  sqlite-app
//
//  Created by toyota on 2017/12/10.
//

#include "DBManager.hpp"

std::mutex DBManager::sMutex;

DBManager::DBManager(): mDBPath(""){}

DBManager *DBManager::getInstance(){
    static DBManager *pInstance = nullptr;
    if(nullptr == pInstance){
        DBManager::sMutex.lock();
        if(nullptr == pInstance){
            pInstance = new DBManager();
        }
        DBManager::sMutex.unlock();
    }
    return pInstance;
}

void DBManager::initialDB(const std::string &path){
    DBManager::sMutex.lock();
    mDBPath = path;
    getDB();
    DBManager::sMutex.unlock();
}

DBStorage DBManager::getDB(){
    static DBStorage db = ORM::initStorage(mDBPath);
    static bool isInitial = false;
    if(!isInitial){
        db.sync_schema();
        isInitial = true;
    }
    return db;
}


