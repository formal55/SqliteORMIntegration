# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Kapec
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

# Project
project ( sqlite3 C )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )

# SQLite configuration
find_package ( Threads )
if ( Threads_FOUND )
  if ( CMAKE_USE_PTHREADS_INIT )
    add_definitions ( -D_REENTRANT=1 -DTHREADSAFE=1 )
    set ( EXTRA_LIBS ${CMAKE_THREAD_LIBS_INIT} )
  endif ( )
endif ( )
find_package ( Readline )
if ( READLINE_FOUND )
  include_directories ( ${READLINE_INCLUDE_DIRS} )
endif ( )

# Add dl on unix systems
if ( UNIX )
  set ( EXTRA_LIBS ${EXTRA_LIBS} dl )
endif ( )

set(sqlite_header  
      ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
      ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3ext.h
)

# SQLite Library
add_library ( sqlite3 STATIC sqlite3.c ${sqlite_header})
if ( CMAKE_USE_PTHREADS_INIT )
  target_link_libraries ( sqlite3 ${EXTRA_LIBS} )
endif ( )

# SQLite client
add_executable ( sqlite-app shell.c ${sqlite_header})
set_target_properties ( sqlite-app PROPERTIES OUTPUT_NAME sqlite CLEAN_DIRECT_OUTPUT 
  1 )
target_link_libraries ( sqlite-app sqlite3 )
if ( READLINE_LIBRARIES )
  target_link_libraries ( sqlite-app ${READLINE_LIBRARIES} )
endif ( )



set (SQLITE3_MY_LIBRARIES ${PROJECT_NAME} CACHE PATH "sqlite 3 libraries" FORCE )
set (SQLITE3_MY_INCLUDE_DIR ${PROJECT_SOURCE_DIR} CACHE PATH "sqlite 3 header" FORCE)

install_library ( ${PROJECT_NAME} )
install_header ( ${sqlite_header} )
install_executable ( sqlite-app )

