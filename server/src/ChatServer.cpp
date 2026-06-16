#include "ChatServer.h"
#include "Database.h"
#include "RedisClient.h"
#include <muduo/base/Logging.h>
#include <muduo/net/Endian.h>
#include <sstream>

ChatServer::ChatServer(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& listenAddr,
                       const std::string& nameArg,
                       const std::string& serverIp)
    : server_(loop, listenAddr, nameArg), loop_(loop), serverIp_(serverIp), serverPort_(listenAddr.toPort()) {
    server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    RedisClient::getInstance().subscribe("chat_cluster", 
        std::bind(&ChatServer::onRedisMessage, this, std::placeholders::_1, std::placeholders::_2));
}

ChatServer::~ChatServer() {}

void ChatServer::start() {
    LOG_INFO << "ChatServer start listening on " << server_.ipPort();
    server_.start();
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (conn->connected()) {
        LOG_INFO << "New connection from " << conn->peerAddress().toIpPort();
    } else {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = connectionUsers_.find(conn->name());
        if (it != connectionUsers_.end()) {
            int user_id = it->second;
            userConnections_.erase(user_id);
            connectionUsers_.erase(it);
            
            Database::getInstance().updateUserOnlineStatus(user_id, false);
            RedisClient::getInstance().setUserOffline(user_id);
            LOG_INFO << "User " << user_id << " disconnected";
        }
        LOG_INFO << "Connection " << conn->peerAddress().toIpPort() << " closed";
    }
}

void ChatServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time) {
    std::string msg = buf->retrieveAllAsString();
    try {
        json data = json::parse(msg);
        handleMessage(conn, data);
    } catch (const std::exception& e) {
        LOG_ERROR << "Parse message error: " << e.what();
    }
}

void ChatServer::handleMessage(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    int type = data["type"];
    switch (type) {
        case 1001: handleLogin(conn, data); break;
        case 1003: handleRegister(conn, data); break;
        case 2001: handleAddFriend(conn, data); break;
        case 2003: handleAcceptFriend(conn, data); break;
        case 3001: handleChatMessage(conn, data); break;
        case 2004: handleCreateGroup(conn, data); break;
        case 2005: handleJoinGroup(conn, data); break;
        case 4001: handleHeartbeat(conn); break;
        default: LOG_ERROR << "Unknown message type: " << type;
    }
}

void ChatServer::handleLogin(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    std::string username = data["username"];
    std::string password = data["password"];
    
    UserInfo user = Database::getInstance().loginUser(username, password);
    json response;
    
    if (user.id != -1) {
        std::lock_guard<std::mutex> lock(mutex_);
        userConnections_[user.id] = conn;
        connectionUsers_[conn->name()] = user.id;
        
        Database::getInstance().updateUserOnlineStatus(user.id, true);
        RedisClient::getInstance().setUserOnline(user.id, serverIp_, serverPort_);
        
        response["type"] = 1002;
        response["success"] = true;
        response["user"] = {
            {"id", user.id},
            {"username", user.username},
            {"nickname", user.nickname},
            {"avatar", user.avatar}
        };
        
        auto offlineMsgs = Database::getInstance().getOfflineMessages(user.id);
        if (!offlineMsgs.empty()) {
            json msgsJson = json::array();
            for (const auto& msg : offlineMsgs) {
                msgsJson.push_back({
                    {"id", msg.id},
                    {"from_id", msg.from_id},
                    {"content", msg.content},
                    {"timestamp", msg.timestamp}
                });
            }
            response["offline_messages"] = msgsJson;
            Database::getInstance().clearOfflineMessages(user.id);
        }
        
        auto friends = Database::getInstance().getFriends(user.id);
        if (!friends.empty()) {
            json friendsJson = json::array();
            for (const auto& f : friends) {
                friendsJson.push_back({
                    {"id", f.id},
                    {"username", f.username},
                    {"nickname", f.nickname},
                    {"avatar", f.avatar},
                    {"online", f.online}
                });
            }
            response["friends"] = friendsJson;
        }
        
        auto groups = Database::getInstance().getUserGroups(user.id);
        if (!groups.empty()) {
            json groupsJson = json::array();
            for (const auto& g : groups) {
                groupsJson.push_back({
                    {"id", g.id},
                    {"name", g.name},
                    {"description", g.description}
                });
            }
            response["groups"] = groupsJson;
        }
        
        LOG_INFO << "User " << user.username << " logged in on server " << serverIp_;
    } else {
        response["type"] = 1002;
        response["success"] = false;
        response["message"] = "Invalid username or password";
    }
    
    sendResponse(conn, MessageType::LOGIN_RESPONSE, response);
}

void ChatServer::handleRegister(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    std::string username = data["username"];
    std::string password = data["password"];
    std::string nickname = data["nickname"];
    std::string avatar = data.contains("avatar") ? data["avatar"] : "";
    
    json response;
    
    if (Database::getInstance().isUsernameExists(username)) {
        response["type"] = 1004;
        response["success"] = false;
        response["message"] = "Username already exists";
    } else if (Database::getInstance().registerUser(username, password, nickname, avatar)) {
        response["type"] = 1004;
        response["success"] = true;
        response["message"] = "Register success";
        LOG_INFO << "New user registered: " << username;
    } else {
        response["type"] = 1004;
        response["success"] = false;
        response["message"] = "Register failed";
    }
    
    sendResponse(conn, MessageType::REGISTER_RESPONSE, response);
}

void ChatServer::handleAddFriend(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    auto it = connectionUsers_.find(conn->name());
    if (it == connectionUsers_.end()) return;
    
    int from_id = it->second;
    int to_id = data["to_id"];
    std::string message = data.contains("message") ? data["message"] : "";
    
    json response;
    if (Database::getInstance().addFriendRequest(from_id, to_id, message)) {
        response["type"] = 2002;
        response["success"] = true;
        
        if (RedisClient::getInstance().isUserOnline(to_id)) {
            json notify = {
                {"type", 2003},
                {"from_id", from_id},
                {"message", message}
            };
            forwardToServer(to_id, notify.dump());
        }
    } else {
        response["type"] = 2002;
        response["success"] = false;
    }
    
    sendResponse(conn, MessageType::ADD_FRIEND_RESPONSE, response);
}

void ChatServer::handleAcceptFriend(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    auto it = connectionUsers_.find(conn->name());
    if (it == connectionUsers_.end()) return;
    
    int to_id = it->second;
    int from_id = data["from_id"];
    
    json response;
    if (Database::getInstance().acceptFriendRequest(from_id, to_id)) {
        response["type"] = 2003;
        response["success"] = true;
        
        if (RedisClient::getInstance().isUserOnline(from_id)) {
            json notify = {
                {"type", 2003},
                {"success", true},
                {"from_id", to_id}
            };
            forwardToServer(from_id, notify.dump());
        }
    } else {
        response["type"] = 2003;
        response["success"] = false;
    }
    
    sendResponse(conn, MessageType::ADD_FRIEND_RESPONSE, response);
}

void ChatServer::handleChatMessage(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    auto it = connectionUsers_.find(conn->name());
    if (it == connectionUsers_.end()) return;
    
    int from_id = it->second;
    int to_id = data["to_id"];
    int group_id = data.contains("group_id") ? data["group_id"].get<int>() : 0;
    std::string content = data["content"];
    bool is_group = data.contains("is_group") ? data["is_group"].get<bool>() : false;
    
    time_t now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    ChatMessage msg;
    msg.from_id = from_id;
    msg.to_id = to_id;
    msg.group_id = group_id;
    msg.content = content;
    msg.timestamp = timestamp;
    msg.is_group = is_group;
    msg.is_read = false;
    
    Database::getInstance().saveMessage(msg);
    
    if (is_group) {
        sendMessageToGroup(group_id, data.dump(), from_id);
    } else {
        forwardToServer(to_id, data.dump());
    }
    
    json response;
    response["type"] = 3002;
    response["success"] = true;
    sendResponse(conn, MessageType::CHAT_RESPONSE, response);
}

void ChatServer::handleCreateGroup(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    auto it = connectionUsers_.find(conn->name());
    if (it == connectionUsers_.end()) return;
    
    int owner_id = it->second;
    std::string name = data["name"];
    std::string description = data.contains("description") ? data["description"] : "";
    
    int group_id = Database::getInstance().createGroup(name, description, owner_id);
    
    json response;
    if (group_id != -1) {
        response["type"] = 2004;
        response["success"] = true;
        response["group_id"] = group_id;
        LOG_INFO << "Group created: " << name << " by user " << owner_id;
    } else {
        response["type"] = 2004;
        response["success"] = false;
    }
    
    sendResponse(conn, static_cast<MessageType>(2004), response);
}

void ChatServer::handleJoinGroup(const muduo::net::TcpConnectionPtr& conn, const json& data) {
    auto it = connectionUsers_.find(conn->name());
    if (it == connectionUsers_.end()) return;
    
    int user_id = it->second;
    int group_id = data["group_id"];
    
    json response;
    if (Database::getInstance().joinGroup(user_id, group_id)) {
        response["type"] = 2005;
        response["success"] = true;
    } else {
        response["type"] = 2005;
        response["success"] = false;
    }
    
    sendResponse(conn, static_cast<MessageType>(2005), response);
}

void ChatServer::handleHeartbeat(const muduo::net::TcpConnectionPtr& conn) {
    json response;
    response["type"] = 4001;
    response["success"] = true;
    sendResponse(conn, MessageType::HEARTBEAT, response);
}

void ChatServer::sendResponse(const muduo::net::TcpConnectionPtr& conn,
                              MessageType type,
                              const json& data) {
    std::string msg = data.dump();
    conn->send(msg);
}

void ChatServer::sendMessageToUser(int user_id, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userConnections_.find(user_id);
    if (it != userConnections_.end()) {
        it->second->send(message);
    }
}

void ChatServer::sendMessageToGroup(int group_id, const std::string& message, int exclude_user_id) {
    auto members = Database::getInstance().getGroupMembers(group_id);
    for (const auto& member : members) {
        if (member.id != exclude_user_id) {
            forwardToServer(member.id, message);
        }
    }
}

void ChatServer::forwardToServer(int target_user_id, const std::string& message) {
    if (isUserOnline(target_user_id)) {
        sendMessageToUser(target_user_id, message);
    } else {
        std::string target_server = RedisClient::getInstance().getUserServer(target_user_id);
        if (!target_server.empty()) {
            json forward_msg = {
                {"type", 5001},
                {"target_user_id", target_user_id},
                {"message", message}
            };
            RedisClient::getInstance().publish("chat_cluster", forward_msg.dump());
        }
    }
}

void ChatServer::onRedisMessage(const std::string& channel, const std::string& message) {
    try {
        json data = json::parse(message);
        if (data["type"] == 5001) {
            int target_user_id = data["target_user_id"];
            std::string inner_msg = data["message"];
            
            if (isUserOnline(target_user_id)) {
                sendMessageToUser(target_user_id, inner_msg);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR << "Parse redis message error: " << e.what();
    }
}

void ChatServer::removeConnection(int user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userConnections_.find(user_id);
    if (it != userConnections_.end()) {
        connectionUsers_.erase(it->second->name());
        userConnections_.erase(it);
    }
}

bool ChatServer::isUserOnline(int user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return userConnections_.find(user_id) != userConnections_.end();
}