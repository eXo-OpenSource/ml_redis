﻿#include "RedisClient.h"
#include "Module.h"

redis_client::redis_client()
{
	_client     = std::make_unique<cpp_redis::client>();
	_subscriber = std::make_unique<cpp_redis::subscriber>();
}

void redis_client::authenticate(const std::string& password, const cpp_redis::client::reply_callback_t& client_callback,
	const cpp_redis::client::reply_callback_t& subscriber_callback) const
{
	_client->auth(password, client_callback);
	_subscriber->auth(password, subscriber_callback);

	_client->commit();
	_subscriber->commit();
}

cpp_redis::reply redis_client::set(const std::string& key, const std::string& value) const
{
	auto result = _client->set(key, value);
	_client->commit();

	result.wait();
	return result.get();
}

void redis_client::set(const std::map<std::string, std::string>& pairs, const std::function<void(const std::string&, cpp_redis::reply&)>& callback) const
{
	for(auto& [key, value] : pairs)
	{
		_client->set(key, value, [key, callback](cpp_redis::reply& reply)
		{
			callback(key, reply);
		});
	}
	_client->commit();
}

cpp_redis::reply redis_client::get(const std::string& key) const
{
	auto result = _client->get(key);
	_client->commit();

	result.wait();
	return result.get();
}

void redis_client::get(const std::list<std::string>& keys, const std::function<void(const std::string&, cpp_redis::reply &)>& callback) const
{
	for (auto& key : keys)
	{
		_client->get(key, [key, callback](cpp_redis::reply& reply)
		{
			callback(key, reply);
		});
	}
	_client->commit();
}

void redis_client::subscribe(const std::list<std::string>& channels,
	const cpp_redis::subscriber::subscribe_callback_t& callback) const
{
	for (auto& channel : channels)
	{
		_subscriber->subscribe(channel, callback);
	}

	_subscriber->commit();
}

void redis_client::unsubscribe(const std::list<std::string>& channels) const
{
	for (auto& channel : channels)
	{
		_subscriber->unsubscribe(channel);
	}
	_subscriber->commit();
}

cpp_redis::reply redis_client::publish(const std::string& channel, const std::string& message) const
{
	auto result = _client->publish(channel, message);
	_client->commit();

	result.wait();
	return result.get();
}

void redis_client::publish(const std::map<std::string, std::string>& pairs,
	const std::function<void(const std::string&, cpp_redis::reply&)>& callback) const
{
	for (auto& [channel, message] : pairs)
	{
		_client->publish(channel, message, [channel, callback](cpp_redis::reply& reply)
		{
			callback(channel, reply);
		});
	}
	_client->commit();
}
