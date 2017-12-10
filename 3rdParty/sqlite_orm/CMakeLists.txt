project(sqliteORM )
cmake_minimum_required(VERSION 3.0.2)

include(${CMAKE_SOURCE_DIR}/3rdParty/libsqlite3/cmake/dist.cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${SQLITE3_MY_INCLUDE_DIR}
)

set(lib_srcs 
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/insert.cpp
)

set(header_srcs 
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite_orm.h 
)

add_definitions(-std=c++14)
add_executable(${PROJECT_NAME}-insert ${lib_srcs} ${header_srcs})
target_link_libraries(${PROJECT_NAME}-insert ${SQLITE3_MY_LIBRARIES})


install_header ( ${header_src} )
install_executable ( ${PROJECT_NAME}-insert )
