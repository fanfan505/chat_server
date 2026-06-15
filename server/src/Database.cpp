#include "Database.h"
#include <iostream>

Database::Database() : connection_(nullptr) {}

Database::~Database() {
    disconnect();
}

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

bool Database::connect(const std::string& host, int port, const std::string& user,
                       const std::string& password, const std::string& database) {
    connection_ = mysql_init(nullptr);
    if (!connection_) {
        return false;
    }
    
    bool reconnect = 1;
    mysql_options(connection_, MYSQL_OPT_RECONNECT, &reconnect);
    
    if (!mysql_real_connect(connection_, host.c_str(), user.c_str(), 
                            password.c_str(), database.c_str(), port, nullptr, 0)) {
        return false;
    }
    
    return true;
}

void Database::disconnect() {
    if (connection_) {
        mysql_close(connection_);
        connection_ = nullptr;
    }
}

bool Database::executeQuery(const std::string& query) {
    return mysql_query(connection_, query.c_str()) == 0;
}

MYSQL_RES* Database::executeSelect(const std::string& query) {
    if (mysql_query(connection_, query.c_str()) != 0) {
        return nullptr;
    }
    return mysql_store_result(connection_);
}

std::string Database::escapeString(const std::string& str) {
    char* escaped = new char[str.length() * 2 + 1];
    mysql_real_escape_string(connection_, escaped, str.c_str(), str.length());
    std::string result(escaped);
    delete[] escaped;
    return result;
}

bool Database::registerUser(const std::string& username, const std::string& password,
                            const std::string& nickname, const std::string& avatar) {
    std::string query = "INSERT INTO users (username, password, nickname, avatar, online) VALUES ('" +
                        escapeString(username) + "', '" + escapeString(password) + "', '" +
                        escapeString(nickname) + "', '" + escapeString(avatar) + "', 0)";
    return executeQuery(query);
}

bool Database::isUsernameExists(const std::string& username) {
    std::string query = "SELECT id FROM users WHERE username = '" + escapeString(username) + "'";
    MYSQL_RES* result = executeSelect(query);
    if (!result) return false;
    
    bool exists = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    return exists;
}

UserInfo Database::loginUser(const std::string& username, const std::string& password) {
    UserInfo info;
    info.id = -1;
    
    std::string query = "SELECT id, username, password, nickname, avatar FROM users WHERE "
                        "username = '" + escapeString(username) + "' AND password = '" + escapeString(password) + "'";
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return info;
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        info.id = atoi(row[0]);
        info.username = row[1];
        info.password = row[2];
        info.nickname = row[3];
        info.avatar = row[4];
        info.online = true;
    }
    
    mysql_free_result(result);
    return info;
}

UserInfo Database::getUserInfo(int user_id) {
    UserInfo info;
    info.id = -1;
    
    char query[256];
    sprintf(query, "SELECT id, username, nickname, avatar, online FROM users WHERE id = %d", user_id);
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return info;
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row) {
        info.id = atoi(row[0]);
        info.username = row[1];
        info.nickname = row[2];
        info.avatar = row[3];
        info.online = atoi(row[4]) == 1;
    }
    
    mysql_free_result(result);
    return info;
}

bool Database::addFriendRequest(int from_id, int to_id, const std::string& message) {
    char query[512];
    sprintf(query, "INSERT INTO friend_requests (from_id, to_id, message, status) VALUES (%d, %d, '%s', 0)",
            from_id, to_id, escapeString(message).c_str());
    return executeQuery(query);
}

bool Database::acceptFriendRequest(int from_id, int to_id) {
    char query[256];
    sprintf(query, "UPDATE friend_requests SET status = 1 WHERE from_id = %d AND to_id = %d", from_id, to_id);
    if (!executeQuery(query)) return false;
    
    sprintf(query, "INSERT INTO friends (user_id, friend_id) VALUES (%d, %d), (%d, %d)", 
            from_id, to_id, to_id, from_id);
    return executeQuery(query);
}

bool Database::rejectFriendRequest(int from_id, int to_id) {
    char query[256];
    sprintf(query, "UPDATE friend_requests SET status = 2 WHERE from_id = %d AND to_id = %d", from_id, to_id);
    return executeQuery(query);
}

std::vector<UserInfo> Database::getFriends(int user_id) {
    std::vector<UserInfo> friends;
    
    char query[256];
    sprintf(query, "SELECT u.id, u.username, u.nickname, u.avatar, u.online FROM users u "
                   "JOIN friends f ON u.id = f.friend_id WHERE f.user_id = %d", user_id);
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return friends;
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        UserInfo info;
        info.id = atoi(row[0]);
        info.username = row[1];
        info.nickname = row[2];
        info.avatar = row[3];
        info.online = atoi(row[4]) == 1;
        friends.push_back(info);
    }
    
    mysql_free_result(result);
    return friends;
}

std::vector<UserInfo> Database::getFriendRequests(int user_id) {
    std::vector<UserInfo> requests;
    
    char query[256];
    sprintf(query, "SELECT u.id, u.username, u.nickname, u.avatar FROM users u "
                   "JOIN friend_requests r ON u.id = r.from_id WHERE r.to_id = %d AND r.status = 0", user_id);
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return requests;
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        UserInfo info;
        info.id = atoi(row[0]);
        info.username = row[1];
        info.nickname = row[2];
        info.avatar = row[3];
        info.online = false;
        requests.push_back(info);
    }
    
    mysql_free_result(result);
    return requests;
}

int Database::createGroup(const std::string& name, const std::string& description, int owner_id) {
    std::string query = "INSERT INTO groups (name, description, owner_id) VALUES ('" +
                        escapeString(name) + "', '" + escapeString(description) + "', " + 
                        std::to_string(owner_id) + ")";
    
    if (!executeQuery(query)) return -1;
    
    char query2[256];
    sprintf(query2, "INSERT INTO group_members (group_id, user_id, role) VALUES (%d, %d, 1)",
            static_cast<int>(mysql_insert_id(connection_)), owner_id);
    
    if (!executeQuery(query2)) return -1;
    
    return static_cast<int>(mysql_insert_id(connection_));
}

bool Database::joinGroup(int user_id, int group_id) {
    char query[256];
    sprintf(query, "INSERT INTO group_members (group_id, user_id, role) VALUES (%d, %d, 0)", group_id, user_id);
    return executeQuery(query);
}

bool Database::leaveGroup(int user_id, int group_id) {
    char query[256];
    sprintf(query, "DELETE FROM group_members WHERE group_id = %d AND user_id = %d", group_id, user_id);
    return executeQuery(query);
}

std::vector<GroupInfo> Database::getUserGroups(int user_id) {
    std::vector<GroupInfo> groups;
    
    char query[256];
    sprintf(query, "SELECT g.id, g.name, g.description, g.owner_id FROM groups g "
                   "JOIN group_members gm ON g.id = gm.group_id WHERE gm.user_id = %d", user_id);
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return groups;
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        GroupInfo info;
        info.id = atoi(row[0]);
        info.name = row[1];
        info.description = row[2];
        info.owner_id = atoi(row[3]);
        groups.push_back(info);
    }
    
    mysql_free_result(result);
    return groups;
}

std::vector<UserInfo> Database::getGroupMembers(int group_id) {
    std::vector<UserInfo> members;
    
    char query[256];
    sprintf(query, "SELECT u.id, u.username, u.nickname, u.avatar, u.online FROM users u "
                   "JOIN group_members gm ON u.id = gm.user_id WHERE gm.group_id = %d", group_id);
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return members;
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        UserInfo info;
        info.id = atoi(row[0]);
        info.username = row[1];
        info.nickname = row[2];
        info.avatar = row[3];
        info.online = atoi(row[4]) == 1;
        members.push_back(info);
    }
    
    mysql_free_result(result);
    return members;
}

bool Database::saveMessage(const ChatMessage& msg) {
    char query[1024];
    sprintf(query, "INSERT INTO messages (from_id, to_id, group_id, content, timestamp, is_group, is_read) "
                   "VALUES (%d, %d, %d, '%s', '%s', %d, %d)",
            msg.from_id, msg.to_id, msg.group_id, escapeString(msg.content).c_str(),
            msg.timestamp.c_str(), msg.is_group ? 1 : 0, msg.is_read ? 1 : 0);
    return executeQuery(query);
}

std::vector<ChatMessage> Database::getOfflineMessages(int user_id) {
    std::vector<ChatMessage> messages;
    
    char query[256];
    sprintf(query, "SELECT id, from_id, to_id, group_id, content, timestamp, is_group, is_read "
                   "FROM messages WHERE to_id = %d AND is_group = 0 AND is_read = 0", user_id);
    
    MYSQL_RES* result = executeSelect(query);
    if (!result) return messages;
    
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr) {
        ChatMessage msg;
        msg.id = atoi(row[0]);
        msg.from_id = atoi(row[1]);
        msg.to_id = atoi(row[2]);
        msg.group_id = atoi(row[3]);
        msg.content = row[4];
        msg.timestamp = row[5];
        msg.is_group = atoi(row[6]) == 1;
        msg.is_read = atoi(row[7]) == 1;
        messages.push_back(msg);
    }
    
    mysql_free_result(result);
    return messages;
}

bool Database::clearOfflineMessages(int user_id) {
    char query[256];
    sprintf(query, "UPDATE messages SET is_read = 1 WHERE to_id = %d AND is_group = 0 AND is_read = 0", user_id);
    return executeQuery(query);
}

bool Database::updateUserOnlineStatus(int user_id, bool online) {
    char query[256];
    sprintf(query, "UPDATE users SET online = %d WHERE id = %d", online ? 1 : 0, user_id);
    return executeQuery(query);
}
