#pragma once
#include <cpp_redis/core/client.hpp>
#include <cpp_redis/core/subscriber.hpp>

class RedisClient
{
public:
	RedisClient();
	~RedisClient();

	void connect(const std::string& host, const int& port) const;
	void disconnect() const;
	bool is_connected() const;
private:
	cpp_redis::client* _client;
	cpp_redis::subscriber* _subscriber;
};

