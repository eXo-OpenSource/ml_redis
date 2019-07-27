#include "RedisClient.h"
#include "Module.h"

redis_client::redis_client()
{
	_client     = new cpp_redis::client();
	_subscriber = new cpp_redis::subscriber();

	g_Module->AddRedisClient(this);
}

redis_client::~redis_client()
{
	g_Module->RemoveRedisClient(this);

	delete _client;
	delete _subscriber;
}

void redis_client::connect(const std::string& host, const int& port) const
{
	_client->connect(host, port);
	_subscriber->connect(host, port);
}

void redis_client::disconnect() const
{
	_client->disconnect(true);
	_subscriber->disconnect(true);
}

bool redis_client::is_connected() const
{
	return _client->is_connected() && _subscriber->is_connected();
}

cpp_redis::reply redis_client::set(const std::string& key, const std::string& value) const
{
	auto result = _client->set(key, value);
	_client->commit();

	result.wait();
	return result.get();
}

void redis_client::set(const std::string& key, const std::string& value,
	const cpp_redis::client::reply_callback_t& callback) const
{
	_client->set(key, value, callback);
	_client->commit();
}

cpp_redis::reply redis_client::get(const std::string& key) const
{
	auto result = _client->get(key);
	_client->commit();

	result.wait();
	return result.get();
}

void redis_client::get(const std::string& key, const cpp_redis::client::reply_callback_t& callback) const
{
	_client->get(key, callback);
	_client->commit();
}
