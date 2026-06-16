#pragma once

#include <hiredis/hiredis.h>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <map>
#include <mutex>
#include "Common.h"

class RedisClient {
public:
    static RedisClient& getInstance();
    
    bool connect(const std::string& host, int port, const std::string& password = "");
    
    void disconnect();
    
    bool publish(const std::string& channel, const std::string& message);
    
    void subscribe(const std::string& channel, 
                   std::function<void(const std::string&, const std::string&)> callback);
    
    void unsubscribe(const std::string& channel);
    
    bool set(const std::string& key, const std::string& value, int expire_seconds = 0);
    
    std::string get(const std::string& key);
    
    bool del(const std::string& key);
    
    bool exists(const std::string& key);
    
    bool hset(const std::string& key, const std::string& field, const std::string& value);
    
    std::string hget(const std::string& key, const std::string& field);
    
    bool sadd(const std::string& key, const std::string& member);
    
    bool srem(const std::string& key, const std::string& member);
    
    bool sismember(const std::string& key, const std::string& member);
    
    std::vector<std::string> smembers(const std::string& key);
    
    bool lpush(const std::string& key, const std::string& value);
    
    std::vector<std::string> lrange(const std::string& key, int start, int end);
    
    bool ltrim(const std::string& key, int start, int end);
    
    long long llen(const std::string& key);
    
    bool setUserOnline(int user_id, const std::string& server_ip, int server_port);
    
    bool setUserOffline(int user_id);
    
    std::string getUserServer(int user_id);
    
    bool isUserOnline(int user_id);
    
private:
    RedisClient();
    ~RedisClient();
    
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(const RedisClient&) = delete;
    
    redisContext* context_;
    redisContext* pubsub_context_;
    
    std::thread pubsub_thread_;
    bool running_;
    
    std::map<std::string, std::function<void(const std::string&, const std::string&)>> callbacks_;
    std::mutex callbacks_mutex_;
    
    void pubsubLoop();
};