#include "ChatServer.h"
#include "Database.h"
#include "RedisClient.h"
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <port> <server_ip>" << std::endl;
        return 1;
    }
    
    int port = atoi(argv[1]);
    std::string server_ip = argv[2];
    
    muduo::Logger::setLogLevel(muduo::Logger::INFO);
    
    if (!Database::getInstance().connect("localhost", 3306, "root", "password", "chat_db")) {
        LOG_ERROR << "Failed to connect to database";
        return 1;
    }
    LOG_INFO << "Connected to database";
    
    if (!RedisClient::getInstance().connect("localhost", 6379)) {
        LOG_ERROR << "Failed to connect to Redis";
        return 1;
    }
    LOG_INFO << "Connected to Redis";
    
    muduo::net::EventLoop loop;
    muduo::net::InetAddress listenAddr(port);
    ChatServer server(&loop, listenAddr, "ChatServer", server_ip);
    
    server.start();
    loop.loop();
    
    return 0;
}
