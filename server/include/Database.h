#pragma once

#include <mysql/mysql.h>
#include <string>
#include <memory>
#include <vector>
#include "Common.h"

class Database {
public:
    static Database& getInstance();
    
    bool connect(const std::string& host, int port, const std::string& user, 
                 const std::string& password, const std::string& database);
    
    void disconnect();
    
    bool registerUser(const std::string& username, const std::string& password, 
                      const std::string& nickname, const std::string& avatar);
    
    UserInfo loginUser(const std::string& username, const std::string& password);
    
    bool isUsernameExists(const std::string& username);
    
    UserInfo getUserInfo(int user_id);
    
    bool addFriendRequest(int from_id, int to_id, const std::string& message);
    
    bool acceptFriendRequest(int from_id, int to_id);
    
    bool rejectFriendRequest(int from_id, int to_id);
    
    std::vector<UserInfo> getFriends(int user_id);
    
    std::vector<UserInfo> getFriendRequests(int user_id);
    
    int createGroup(const std::string& name, const std::string& description, int owner_id);
    
    bool joinGroup(int user_id, int group_id);
    
    bool leaveGroup(int user_id, int group_id);
    
    std::vector<GroupInfo> getUserGroups(int user_id);
    
    std::vector<UserInfo> getGroupMembers(int group_id);
    
    bool saveMessage(const ChatMessage& msg);
    
    std::vector<ChatMessage> getOfflineMessages(int user_id);
    
    bool clearOfflineMessages(int user_id);
    
    bool updateUserOnlineStatus(int user_id, bool online);
    
private:
    Database();
    ~Database();
    
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    MYSQL* connection_;
    
    bool executeQuery(const std::string& query);
    
    MYSQL_RES* executeSelect(const std::string& query);
    
    std::string escapeString(const std::string& str);
};
