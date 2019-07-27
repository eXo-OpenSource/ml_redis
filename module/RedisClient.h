#pragma once
#include <cpp_redis/core/client.hpp>
#include <cpp_redis/core/subscriber.hpp>

class redis_client
{
public:
	redis_client();
	~redis_client();

	void connect(const std::string& host, const int& port) const { _client->connect(host, port); _subscriber->connect(host, port); }
	void disconnect() const { _client->disconnect(); _subscriber->disconnect(); }
	[[nodiscard]] bool is_connected() const { return _client->is_connected() && _subscriber->is_connected(); }

	[[nodiscard]] cpp_redis::reply set(const std::string& key, const std::string& value) const;
	void set(const std::string& key, const std::string& value, const cpp_redis::client::reply_callback_t& callback) const { _client->set(key, value, callback); _client->commit(); }
	void set(const std::map<std::string, std::string>& pairs, const std::function<void(const std::string&, cpp_redis::reply &)>& callback) const;

	[[nodiscard]] cpp_redis::reply get(const std::string& key) const;
	void get(const std::string& key, const cpp_redis::client::reply_callback_t& callback) const { _client->get(key, callback); _client->commit(); };
	void get(const std::list<std::string>& keys, const std::function<void(const std::string&, cpp_redis::reply &)>& callback) const;
private:
	cpp_redis::client* _client;
	cpp_redis::subscriber* _subscriber;
};

