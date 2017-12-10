project(sqliteORM-app)
cmake_minimum_required(VERSION 3.0.2)

include(${CMAKE_SOURCE_DIR}/3rdParty/libsqlite3/cmake/dist.cmake)
include(${CMAKE_SOURCE_DIR}/3rdParty/sqlite_orm/sqlite_orm.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(lib_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/DBManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set(header_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/DBManager.hpp
)

add_executable ( ${PROJECT_NAME} ${lib_srcs} ${header_srcs})
target_link_libraries(${PROJECT_NAME} ${SQLITE3_MY_LIBRARIES})
install_executable (${PROJECT_NAME})
