﻿cmake_minimum_required(VERSION 3.16)
project(cluster_chat)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_SERVER "Build server" OFF)
option(BUILD_CLIENT "Build client" ON)

if(BUILD_CLIENT)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64" ${CMAKE_PREFIX_PATH})
    find_package(Qt6 COMPONENTS Core Network Widgets REQUIRED)
    
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    
    include_directories(
        ${PROJECT_SOURCE_DIR}/client/include
    )
    
    file(GLOB_RECURSE CLIENT_HEADERS "client/include/*.h")
    file(GLOB_RECURSE CLIENT_SOURCES "client/src/*.cpp")
    
    add_executable(chat_client
        ${CLIENT_SOURCES}
        ${CLIENT_HEADERS}
    )
    
    target_link_libraries(chat_client
        Qt6::Core
        Qt6::Network
        Qt6::Widgets
    )
    
    message(STATUS "Build client: ON")
endif()

if(BUILD_SERVER)
    if(WIN32)
        set(Boost_INCLUDE_DIR "C:/boost_1_85_0")
        set(Boost_LIBRARY_DIR "C:/boost_1_85_0/stage/lib")
        set(MYSQL_INCLUDE_DIR "C:/mysql-connector-c-8.3.0-winx64/include")
        set(MYSQL_LIBRARY_DIR "C:/mysql-connector-c-8.3.0-winx64/lib64")
        set(HIREDIS_INCLUDE_DIR "C:/hiredis/include")
        set(HIREDIS_LIBRARY_DIR "C:/hiredis/lib")
        set(MUDUO_INCLUDE_DIR "C:/muduo/include")
        set(MUDUO_LIBRARY_DIR "C:/muduo/lib")
        set(NLOHMANN_INCLUDE_DIR "C:/nlohmann")
    else()
        find_package(Boost REQUIRED COMPONENTS system thread)
        find_path(MYSQL_INCLUDE_DIR mysql/mysql.h PATHS /usr/include /usr/include/mysql /usr/local/include)
        find_library(MYSQL_LIBRARY mysqlclient PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
        find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h PATHS /usr/include /usr/local/include)
        find_library(HIREDIS_LIBRARY hiredis PATHS /usr/lib /usr/lib/x86_64-linux-gnu /usr/local/lib)
        find_path(MUDUO_INCLUDE_DIR muduo/net/TcpServer.h PATHS /usr/include /usr/local/include)
        find_library(MUDUO_NET_LIBRARY muduo_net PATHS /usr/lib /usr/local/lib)
        find_library(MUDUO_BASE_LIBRARY muduo_base PATHS /usr/lib /usr/local/lib)
        find_path(NLOHMANN_INCLUDE_DIR NAMES json.hpp PATHS /usr/include/nlohmann /usr/local/include/nlohmann /usr/include)
    endif()

    include_directories(
        ${PROJECT_SOURCE_DIR}/server/include
        ${Boost_INCLUDE_DIR}
        ${MYSQL_INCLUDE_DIR}
        ${HIREDIS_INCLUDE_DIR}
        ${MUDUO_INCLUDE_DIR}
        ${NLOHMANN_INCLUDE_DIR}
    )

    link_directories(
        ${Boost_LIBRARY_DIR}
        ${MYSQL_LIBRARY_DIR}
        ${HIREDIS_LIBRARY_DIR}
        ${MUDUO_LIBRARY_DIR}
    )

    file(GLOB_RECURSE SERVER_SOURCES "server/src/*.cpp")

    add_executable(chat_server ${SERVER_SOURCES})

    if(WIN32)
        target_link_libraries(chat_server
            libboost_system-mgw11-mt-x64-1_85
            libboost_thread-mgw11-mt-x64-1_85
            libmysql
            hiredis
            muduo_net
            muduo_base
            ws2_32
            pthread
        )
    else()
        target_link_libraries(chat_server
            ${Boost_SYSTEM_LIBRARY}
            ${Boost_THREAD_LIBRARY}
            ${MYSQL_LIBRARY}
            ${HIREDIS_LIBRARY}
            ${MUDUO_NET_LIBRARY}
            ${MUDUO_BASE_LIBRARY}
            pthread
        )
    endif()

    message(STATUS "Build server: ON")
endif()