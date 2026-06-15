#include "RedisClient.h"
#include <iostream>
#include <thread>
#include <functional>

RedisClient::RedisClient() : context_(nullptr), pubsub_context_(nullptr), running_(false) {}

RedisClient::~RedisClient() {
    disconnect();
}

RedisClient& RedisClient::getInstance() {
    static RedisClient instance;
    return instance;
}

bool RedisClient::connect(const std::string& host, int port, const std::string& password) {
    context_ = redisConnect(host.c_str(), port);
    if (!context_ || context_->err) {
        if (context_) {
            redisFree(context_);
            context_ = nullptr;
        }
        return false;
    }
    
    if (!password.empty()) {
        redisReply* reply = (redisReply*)redisCommand(context_, "AUTH %s", password.c_str());
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            redisFree(context_);
            context_ = nullptr;
            return false;
        }
        freeReplyObject(reply);
    }
    
    pubsub_context_ = redisConnect(host.c_str(), port);
    if (!pubsub_context_ || pubsub_context_->err) {
        if (pubsub_context_) {
            redisFree(pubsub_context_);
            pubsub_context_ = nullptr;
        }
        redisFree(context_);
        context_ = nullptr;
        return false;
    }
    
    if (!password.empty()) {
        redisReply* reply = (redisReply*)redisCommand(pubsub_context_, "AUTH %s", password.c_str());
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            redisFree(pubsub_context_);
            pubsub_context_ = nullptr;
            redisFree(context_);
            context_ = nullptr;
            return false;
        }
        freeReplyObject(reply);
    }
    
    running_ = true;
    pubsub_thread_ = std::thread(&RedisClient::pubsubLoop, this);
    
    return true;
}

void RedisClient::disconnect() {
    running_ = false;
    
    if (pubsub_thread_.joinable()) {
        pubsub_thread_.join();
    }
    
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
    
    if (pubsub_context_) {
        redisFree(pubsub_context_);
        pubsub_context_ = nullptr;
    }
}

bool RedisClient::publish(const std::string& channel, const std::string& message) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "PUBLISH %s %s", channel.c_str(), message.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

void RedisClient::subscribe(const std::string& channel,
                            std::function<void(const std::string&, const std::string&)> callback) {
    if (!pubsub_context_) return;
    
    redisCommand(pubsub_context_, "SUBSCRIBE %s", channel.c_str());
}

void RedisClient::unsubscribe(const std::string& channel) {
    if (!pubsub_context_) return;
    redisCommand(pubsub_context_, "UNSUBSCRIBE %s", channel.c_str());
}

bool RedisClient::set(const std::string& key, const std::string& value, int expire_seconds) {
    if (!context_) return false;
    
    redisReply* reply;
    if (expire_seconds > 0) {
        reply = (redisReply*)redisCommand(context_, "SET %s %s EX %d", key.c_str(), value.c_str(), expire_seconds);
    } else {
        reply = (redisReply*)redisCommand(context_, "SET %s %s", key.c_str(), value.c_str());
    }
    
    if (!reply) return false;
    
    bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0);
    freeReplyObject(reply);
    return success;
}

std::string RedisClient::get(const std::string& key) {
    if (!context_) return "";
    
    redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
    if (!reply) return "";
    
    std::string result;
    if (reply->type == REDIS_REPLY_STRING) {
        result = reply->str;
    }
    
    freeReplyObject(reply);
    return result;
}

bool RedisClient::del(const std::string& key) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "DEL %s", key.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

bool RedisClient::exists(const std::string& key) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "EXISTS %s", key.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

bool RedisClient::hset(const std::string& key, const std::string& field, const std::string& value) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

std::string RedisClient::hget(const std::string& key, const std::string& field) {
    if (!context_) return "";
    
    redisReply* reply = (redisReply*)redisCommand(context_, "HGET %s %s", key.c_str(), field.c_str());
    if (!reply) return "";
    
    std::string result;
    if (reply->type == REDIS_REPLY_STRING) {
        result = reply->str;
    }
    
    freeReplyObject(reply);
    return result;
}

bool RedisClient::sadd(const std::string& key, const std::string& member) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "SADD %s %s", key.c_str(), member.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

bool RedisClient::srem(const std::string& key, const std::string& member) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "SREM %s %s", key.c_str(), member.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

bool RedisClient::sismember(const std::string& key, const std::string& member) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "SISMEMBER %s %s", key.c_str(), member.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

std::vector<std::string> RedisClient::smembers(const std::string& key) {
    std::vector<std::string> result;
    if (!context_) return result;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "SMEMBERS %s", key.c_str());
    if (!reply) return result;
    
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            result.push_back(reply->element[i]->str);
        }
    }
    
    freeReplyObject(reply);
    return result;
}

bool RedisClient::lpush(const std::string& key, const std::string& value) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "LPUSH %s %s", key.c_str(), value.c_str());
    if (!reply) return false;
    
    bool success = reply->integer > 0;
    freeReplyObject(reply);
    return success;
}

std::vector<std::string> RedisClient::lrange(const std::string& key, int start, int end) {
    std::vector<std::string> result;
    if (!context_) return result;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "LRANGE %s %d %d", key.c_str(), start, end);
    if (!reply) return result;
    
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            result.push_back(reply->element[i]->str);
        }
    }
    
    freeReplyObject(reply);
    return result;
}

bool RedisClient::ltrim(const std::string& key, int start, int end) {
    if (!context_) return false;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "LTRIM %s %d %d", key.c_str(), start, end);
    if (!reply) return false;
    
    bool success = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0);
    freeReplyObject(reply);
    return success;
}

long long RedisClient::llen(const std::string& key) {
    if (!context_) return 0;
    
    redisReply* reply = (redisReply*)redisCommand(context_, "LLEN %s", key.c_str());
    if (!reply) return 0;
    
    long long result = reply->integer;
    freeReplyObject(reply);
    return result;
}

bool RedisClient::setUserOnline(int user_id, const std::string& server_ip, int server_port) {
    std::string key = "user_server:" + std::to_string(user_id);
    std::string value = server_ip + ":" + std::to_string(server_port);
    return hset(REDIS_ONLINE_USERS, std::to_string(user_id), value) && sadd(REDIS_ONLINE_USERS, std::to_string(user_id));
}

bool RedisClient::setUserOffline(int user_id) {
    std::string user_str = std::to_string(user_id);
    return del("user_server:" + user_str) && srem(REDIS_ONLINE_USERS, user_str);
}

std::string RedisClient::getUserServer(int user_id) {
    return hget(REDIS_ONLINE_USERS, std::to_string(user_id));
}

bool RedisClient::isUserOnline(int user_id) {
    return sismember(REDIS_ONLINE_USERS, std::to_string(user_id));
}

void RedisClient::pubsubLoop() {
    while (running_) {
        redisReply* reply = nullptr;
        if (redisGetReply(pubsub_context_, (void**)&reply) == REDIS_OK && reply) {
            if (reply->type == REDIS_REPLY_ARRAY && reply->elements >= 3) {
                std::string type = reply->element[0]->str;
                std::string channel = reply->element[1]->str;
                std::string message = reply->element[2]->str;
            }
            freeReplyObject(reply);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
