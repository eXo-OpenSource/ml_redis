#include "Module.h"
#include "CFunctions.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::redis_create_client(lua_State* lua_vm)
{
	// redis_client redis_create_client()
	try {
		const auto client = new RedisClient();
		g_Module->AddRedisClient(client);

		lua_pushlightuserdata(lua_vm, static_cast<void*>(client));
		return 1;	
	} catch(std::exception& e)
	{
		stringstream ss;
		ss << "Failed to create to redis client. [" << e.what() << "]\n";
		pModuleManager->ErrorPrintf(ss.str().c_str());
		lua_pushboolean(lua_vm, false);
		return 1;
	}
}

int CFunctions::redis_connect(lua_State* lua_vm)
{
	// bool redis_connect(redis_client client, string host, number port)
	if (lua_type(lua_vm, 1) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 2) != LUA_TSTRING || lua_type(lua_vm, 3) != LUA_TNUMBER)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_connect.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	try
	{
		// Read arguments
		const auto client = static_cast<RedisClient*>(lua_touserdata(lua_vm, 1));
		const auto host = lua_tostring(lua_vm, 2);
		const auto port = lua_tonumber(lua_vm, 3);

		// verify client
		if (!g_Module->HasRedisClient(client))
		{
			pModuleManager->ErrorPrintf("Bad argument @ redis_connect, invalid client has been passed.");
			lua_pushboolean(lua_vm, false);
			return 1;
		}

		client->connect(host, port);
		lua_pushboolean(lua_vm, true);
		return 1;
	} catch(std::exception& e)
	{
		stringstream ss;
		ss << "Failed to connect to redis server. [" << e.what() << "]\n";
		pModuleManager->ErrorPrintf(ss.str().c_str());
		lua_pushboolean(lua_vm, false);
		return 1;
	}
}

int CFunctions::redis_disconnect(lua_State* lua_vm)
{
	// void redis_disconnect(redis_client client)
	if (lua_type(lua_vm, 1) != LUA_TLIGHTUSERDATA)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_disconnect.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	try {
		// Read arguments
		const auto client = static_cast<RedisClient*>(lua_touserdata(lua_vm, 1));

		// verify client
		if (!g_Module->HasRedisClient(client))
		{
			pModuleManager->ErrorPrintf("Bad argument @ redis_disconnect, invalid client has been passed.");
			lua_pushboolean(lua_vm, false);
			return 1;
		}

		client->disconnect();
		lua_pushboolean(lua_vm, true);
		return 1;
	} catch(exception& e)
	{
		std::cout << e.what() << std::endl;
		lua_pushboolean(lua_vm, false);
		return 1;
	}
}

int CFunctions::redis_test(lua_State* lua_vm)
{
	// bool redis_test(redis_client client)
	if (lua_type(lua_vm, 1) != LUA_TLIGHTUSERDATA)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_test.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	try
	{
		// Read arguments
		const auto client = static_cast<RedisClient*>(lua_touserdata(lua_vm, 1));

		// verify client
		if (!g_Module->HasRedisClient(client))
		{
			lua_pushboolean(lua_vm, false);
			return 1;
		}

		std::cout << (client->is_connected() ? "true" : "false") << std::endl;
		lua_pushboolean(lua_vm, true);
		return 1;
	} catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		lua_pushboolean(lua_vm, false);
		return 1;
	}
}
