#pragma once
#include <iostream>
#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>

#include "extra/CLuaArguments.h"

class ILuaModuleManager10;
struct lua_State;

extern ILuaModuleManager10* pModuleManager;

class CFunctions
{
public:
	static inline int Test(lua_State* lua_vm)
	{
		try {
			cpp_redis::client client;
			client.connect();

			client.set("lol", "redis", [](cpp_redis::reply& reply)
			{
				std::cout << "Reply: " << reply << std::endl;
			});
			client.get("lol", [](cpp_redis::reply& reply) {
				std::cout << "Got from redis: " << reply << std::endl;
			});
			client.sync_commit();
			lua_pushboolean(lua_vm, true);
		} catch(std::exception& e)
		{
			std::cout << e.what() << std::endl;
			lua_pushboolean(lua_vm, false);
		}
		return 1;
	}

	static inline void register_lua_global(lua_State* lua_vm, const char* name, void* value)
	{
		lua_pushlightuserdata(lua_vm, value);
		lua_setglobal(lua_vm, name);
	}

	static int redis_create_client(lua_State* lua_vm);
	static int redis_connect(lua_State* lua_vm);
	static int redis_disconnect(lua_State* lua_vm);
	static int redis_test(lua_State* lua_vm);
};
