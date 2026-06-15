#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class MessageType {
    LOGIN_REQUEST = 1001,
    LOGIN_RESPONSE = 1002,
    REGISTER_REQUEST = 1003,
    REGISTER_RESPONSE = 1004,
    ADD_FRIEND_REQUEST = 2001,
    ADD_FRIEND_RESPONSE = 2002,
    FRIEND_REQUEST_NOTIFY = 2003,
    CREATE_GROUP = 2004,
    JOIN_GROUP = 2005,
    GROUP_NOTIFY = 2006,
    CHAT_MESSAGE = 3001,
    CHAT_RESPONSE = 3002,
    OFFLINE_MESSAGE = 3003,
    HEARTBEAT = 4001,
    ERROR_RESPONSE = 9999
};

struct MessageHeader {
    MessageType type;
    uint32_t length;
};

struct UserInfo {
    int id;
    std::string username;
    std::string password;
    std::string nickname;
    std::string avatar;
    bool online;
};

struct ChatMessage {
    int id;
    int from_id;
    int to_id;
    int group_id;
    std::string content;
    std::string timestamp;
    bool is_group;
    bool is_read;
};

struct GroupInfo {
    int id;
    std::string name;
    std::string description;
    int owner_id;
};

const std::string REDIS_CHANNEL_PREFIX = "chat_channel_";
const std::string REDIS_ONLINE_USERS = "online_users";
const std::string REDIS_OFFLINE_MESSAGE_PREFIX = "offline_msg_";
