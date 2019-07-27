#pragma once
#include <cpp_redis/core/client.hpp>
#include <cpp_redis/core/subscriber.hpp>

using redis_client = cpp_redis::client;
using redis_subscriber = cpp_redis::subscriber;

class RedisClient
{
public:
	RedisClient();
	~RedisClient();

	void connect(const std::string& host, const int& port) const;
	void disconnect() const;
	bool is_connected() const;
private:
	redis_client* _client;
	redis_subscriber* _subscriber;
};

