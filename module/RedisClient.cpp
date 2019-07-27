#include "RedisClient.h"
#include "Module.h"

RedisClient::RedisClient()
{
	_client = new cpp_redis::client();
	_subscriber = new cpp_redis::subscriber();

	g_Module->AddRedisClient(this);
}

RedisClient::~RedisClient()
{
	g_Module->RemoveRedisClient(this);

	delete _client;
	delete _subscriber;
}

void RedisClient::connect(const std::string& host, const int& port) const
{
	_client->connect(host, port);
	_subscriber->connect(host, port);
}

void RedisClient::disconnect() const
{
	_client->disconnect(true);
	_subscriber->disconnect(true);
}

bool RedisClient::is_connected() const
{
	return _client->is_connected() && _subscriber->is_connected();
}
