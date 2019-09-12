#pragma once
#include "LunaRedisClient.h"
#include "Utils.h"

const char LunaRedisClient::className[] = "RedisClient";
Luna<LunaRedisClient>::RegType LunaRedisClient::methods[] = {
	// methods
	method(LunaRedisClient, connect),
	method(LunaRedisClient, disconnect),
	method(LunaRedisClient, get),
	method(LunaRedisClient, set),
	method(LunaRedisClient, subscribe),
	method(LunaRedisClient, unsubscribe),
	method(LunaRedisClient, publish),

	// Do not remove, required
	{ nullptr,		nullptr}
};

class LunaRedisClientHelper
{
public:
	static inline void Register(lua_State* lua_vm) { Luna<LunaRedisClient>::Register(lua_vm); }
};

