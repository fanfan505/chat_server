#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Logging.h>
#include <map>
#include <mutex>
#include "Common.h"

class ChatServer {
public:
    ChatServer(muduo::net::EventLoop* loop, 
               const muduo::net::InetAddress& listenAddr,
               const std::string& nameArg);
    
    ~ChatServer();
    
    void start();
    
    void sendMessageToUser(int user_id, const std::string& message);
    
    void sendMessageToGroup(int group_id, const std::string& message, int exclude_user_id = -1);
    
    void removeConnection(int user_id);
    
    bool isUserOnline(int user_id);
    
private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    
    void onMessage(const muduo::net::TcpConnectionPtr& conn,
                   muduo::net::Buffer* buf,
                   muduo::Timestamp time);
    
    void handleMessage(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleLogin(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleRegister(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleAddFriend(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleChatMessage(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleCreateGroup(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleJoinGroup(const muduo::net::TcpConnectionPtr& conn, const json& data);
    
    void handleHeartbeat(const muduo::net::TcpConnectionPtr& conn);
    
    void sendResponse(const muduo::net::TcpConnectionPtr& conn, 
                      MessageType type, 
                      const json& data);
    
    muduo::net::TcpServer server_;
    muduo::net::EventLoop* loop_;
    
    std::map<int, muduo::net::TcpConnectionPtr> userConnections_;
    std::map<std::string, int> connectionUsers_;
    std::mutex mutex_;
};
