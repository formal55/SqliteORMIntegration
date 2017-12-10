project(sqliteORMExample)
cmake_minimum_required(VERSION 3.0.2)

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/libsqlite3/cmake ${CMAKE_MODULE_PATH})

if ( APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

add_subdirectory(3rdParty)
add_subdirectory(app)