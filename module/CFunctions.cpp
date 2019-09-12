#include "Module.h"
#include "CFunctions.h"
#include "Utils.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::redis_create_client(lua_State* lua_vm)
{
	const auto client = new ml_redis::redis_client();
	utils::lua_createobject(lua_vm, client, "RedisClient");

	return 1;
}

int CFunctions::redis_client_destruct(lua_State* lua_vm)
{
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");
	delete client;
	
	return 0;
}

int CFunctions::redis_connect(lua_State* lua_vm)
{
	// Read arguments
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");
	const auto host   = luaL_checkstring(lua_vm, 2);
	const auto port   = luaL_checknumber(lua_vm, 3);
	
	// verify client
	if (!g_Module->HasRedisClient(client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");

	try
	{
		client->connect(host, port);
		lua_pushboolean(lua_vm, true);
	} catch (std::exception& e)
	{
		pModuleManager->ErrorPrintf("Failed to connect to redis server. [%s]\n", e.what());
		lua_pushboolean(lua_vm, false);
	}
		
	return 1;
}

int CFunctions::redis_disconnect(lua_State* lua_vm)
{
	// Read arguments
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");

	// verify client
	if (!g_Module->HasRedisClient(client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");

	try {
		client->disconnect();
		lua_pushboolean(lua_vm, true);
	} catch(exception& e)
	{
		pModuleManager->ErrorPrintf("%s\n", e.what());
		lua_pushboolean(lua_vm, false);
	}

	return 1;
}

int CFunctions::redis_set(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 2);
	
	// Read arguments
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");
	const auto key    = luaL_checkstring(lua_vm, 3);
	const auto value  = luaL_checkstring(lua_vm, 4);

	// verify client
	if (!g_Module->HasRedisClient(client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");

	g_Module->GetJobManager().PushTask([lua_vm, client, key, value]() -> const std::optional<std::any>
	{
		try {
			return client->set(key, value);
		} catch(exception& e)
		{
			pModuleManager->ErrorPrintf("%s\n", e.what());
			return {};
		}
	}, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::optional<std::any>& result)
	{
		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		if (!g_Module->HasLuaVM(lua_vm))
			return;

		// Push stored reference to callback function to the stack
		lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

		// Push the result
		try
		{
			if (result.has_value())
			{
				const auto reply = std::any_cast<cpp_redis::reply>(result.value());
				if (!reply.is_error())
				{
					lua_pushboolean(lua_vm, true);
				} else
				{
					std::cout << reply.error() << std::endl;
					lua_pushboolean(lua_vm, false);
				}
			} else {
				lua_pushboolean(lua_vm, false);
			}
		} catch(std::bad_any_cast&)
		{
			lua_pushboolean(lua_vm, false);
		}

		// Finally, call the function
		const auto err = lua_pcall(lua_vm, 1, 0, 0);
		if (err != 0)
			pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
	});

	lua_pushboolean(lua_vm, true);
	return 1;
}

int CFunctions::redis_get(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 2);

	// Read arguments
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");
	const auto key    = luaL_checkstring(lua_vm, 3);

	// verify client
	if (!g_Module->HasRedisClient(client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");

	g_Module->GetJobManager().PushTask([lua_vm, client, key]() -> const std::optional<std::any>
	{
		try {
			return client->get(key);
		} catch(exception& e)
		{
			pModuleManager->ErrorPrintf("%s\n", e.what());
			return {};
		}
	}, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::optional<std::any>& result)
	{
		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		if (!g_Module->HasLuaVM(lua_vm))
			return;

		// Push stored reference to callback function to the stack
		lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

		// Push the result
		try
		{
			if (result.has_value())
			{
				const auto reply = std::any_cast<cpp_redis::reply>(result.value());
				if (!reply.is_error())
				{
					lua_pushstring(lua_vm, reply.as_string().c_str());
				} else
				{
					std::cout << reply.error() << std::endl;
					lua_pushboolean(lua_vm, false);
				}
			} else {
				lua_pushboolean(lua_vm, false);
			}
		} catch(std::bad_any_cast&)
		{
			lua_pushboolean(lua_vm, false);
		}

		// Finally, call the function
		const auto err = lua_pcall(lua_vm, 1, 0, 0);
		if (err != 0)
			pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
	});

	lua_pushboolean(lua_vm, true);
	return 1;
}

int CFunctions::redis_subscribe(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 2);

	// Read arguments
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");
	const auto channel = lua_tostring(lua_vm, 3);

	// verify client
	if (!g_Module->HasRedisClient(client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");
	
	// subscribe to the channel
	try {
		client->subscribe(channel, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::string& channel, const std::string& message)
		{
			// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
			if (!g_Module->HasLuaVM(lua_vm))
				return;

			// Push stored reference to callback function to the stack
			lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

			// Push results from redis to the stack
			lua_pushstring(lua_vm, channel.c_str());
			lua_pushstring(lua_vm, message.c_str());
			
			// Finally, call the function
			const auto err = lua_pcall(lua_vm, 2, 0, 0);
			if (err != 0)
				pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
		});

		lua_pushboolean(lua_vm, true);
	} catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		lua_pushboolean(lua_vm, false);
	}
	
	return 1;
}

int CFunctions::redis_publish(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 2);

	// Read arguments
	const auto client = utils::lua_checkobject<ml_redis::redis_client>(lua_vm, "RedisClient");
	const auto channel = lua_tostring(lua_vm, 3);
	const auto message = lua_tostring(lua_vm, 4);
	
	// verify client
	if (!g_Module->HasRedisClient(client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");

	g_Module->GetJobManager().PushTask([client, channel, message]() -> const std::optional<std::any>
	{
		try {
			return client->publish(channel, message);
		} catch(exception& e)
		{
			pModuleManager->ErrorPrintf("%s\n", e.what());
			return {};
		}
	}, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::optional<std::any>& result)
	{
		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		if (!g_Module->HasLuaVM(lua_vm))
			return;

		// Push stored reference to callback function to the stack
		lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

		// Push the result
		try
		{
			if (result.has_value())
			{
				const auto reply = std::any_cast<cpp_redis::reply>(result.value());
				if (!reply.is_error())
				{
					lua_pushboolean(lua_vm, true);
				} else
				{
					std::cout << reply.error() << std::endl;
					lua_pushboolean(lua_vm, false);
				}	
			} else {
				lua_pushboolean(lua_vm, false);
			}
		} catch(std::bad_any_cast&)
		{
			lua_pushboolean(lua_vm, false);
		}

		// Finally, call the function
		const auto err = lua_pcall(lua_vm, 1, 0, 0);
		if (err != 0)
			pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
	});

	lua_pushboolean(lua_vm, true);
	return 1;
}
