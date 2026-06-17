cmake_minimum_required(VERSION 3.16)
project(cluster_chat)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    option(BUILD_SERVER "Build server" OFF)
    option(BUILD_CLIENT "Build client" ON)
else()
    option(BUILD_SERVER "Build server" ON)
    option(BUILD_CLIENT "Build client" OFF)
endif()

if(BUILD_CLIENT)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.11.1/mingw_64" ${CMAKE_PREFIX_PATH})
    find_package(Qt6 COMPONENTS Core Network Widgets REQUIRED)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    include_directories(${PROJECT_SOURCE_DIR}/client/include)
    file(GLOB_RECURSE CLIENT_HEADERS "client/include/*.h")
    file(GLOB_RECURSE CLIENT_SOURCES "client/src/*.cpp")
    add_executable(chat_client ${CLIENT_SOURCES} ${CLIENT_HEADERS})
    target_link_libraries(chat_client Qt6::Core Qt6::Network Qt6::Widgets)
    message(STATUS "Build client: ON")
endif()

if(BUILD_SERVER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    
    find_package(Boost REQUIRED)
    
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MYSQL REQUIRED mysqlclient)
    pkg_check_modules(HIREDIS REQUIRED hiredis)
    
    set(MUDUO_INCLUDE_DIR "/home/fanfan/muduo/include")
    set(MUDUO_LIB_DIR "/home/fanfan/muduo/build/release/lib")
    
    include_directories(${PROJECT_SOURCE_DIR}/server/include)
    include_directories(${Boost_INCLUDE_DIRS})
    include_directories(${MYSQL_INCLUDE_DIRS})
    include_directories(${HIREDIS_INCLUDE_DIRS})
    include_directories(${MUDUO_INCLUDE_DIR})
    link_directories(${MUDUO_LIB_DIR})
    
    file(GLOB_RECURSE SERVER_HEADERS "server/include/*.h")
    file(GLOB_RECURSE SERVER_SOURCES "server/src/*.cpp")
    add_executable(chat_server ${SERVER_SOURCES} ${SERVER_HEADERS})
    target_link_libraries(chat_server muduo_net muduo_base
                          ${MYSQL_LIBRARIES} ${HIREDIS_LIBRARIES} -lpthread)
    message(STATUS "Build server: ON")
endif()