cmake_minimum_required(VERSION 3.10)
project(cluster_chat)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Boost REQUIRED COMPONENTS system thread)

find_library(MYSQL_LIBRARY mysqlclient PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
find_path(MYSQL_INCLUDE_DIR mysql/mysql.h PATHS /usr/include /usr/include/mysql)

find_library(HIREDIS_LIBRARY hiredis PATHS /usr/lib /usr/lib/x86_64-linux-gnu)
find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h PATHS /usr/include)

find_path(NLOHMANN_INCLUDE_DIR NAMES json.hpp PATHS /usr/include/nlohmann /usr/local/include/nlohmann)

include_directories(
    ${PROJECT_SOURCE_DIR}/server/include
    ${MYSQL_INCLUDE_DIR}
    ${HIREDIS_INCLUDE_DIR}
    ${NLOHMANN_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)

file(GLOB_RECURSE SERVER_SOURCES "${PROJECT_SOURCE_DIR}/server/src/*.cpp")

add_executable(chat_server ${SERVER_SOURCES})

target_link_libraries(chat_server
    ${MYSQL_LIBRARY}
    ${HIREDIS_LIBRARY}
    ${Boost_LIBRARIES}
    muduo_net
    muduo_base
    pthread
)

install(TARGETS chat_server DESTINATION bin)

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/client/include
)

file(GLOB_RECURSE CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/client/src/*.cpp")

add_executable(chat_client ${CLIENT_SOURCES})

target_link_libraries(chat_client
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)