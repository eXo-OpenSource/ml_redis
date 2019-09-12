#include "LunaRedisClient.h"
#include "Utils.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

// ReSharper disable CppMemberFunctionMayBeConst
LunaRedisClient::LunaRedisClient(lua_State* lua_vm)
{
	_client = std::make_shared<redis_client>();
	g_Module->AddRedisClient(_client);
}

LunaRedisClient::~LunaRedisClient()
{
	g_Module->RemoveRedisClient(_client);
}

void LunaRedisClient::verify_self(lua_State* lua_vm) const
{
	if (!g_Module->HasRedisClient(_client))
		luaL_argerror(lua_vm, 1, "invalid client has been passed");
}

int LunaRedisClient::connect(lua_State* lua_vm)
{
	// Read arguments
	const auto host   = luaL_checkstring(lua_vm, 1);
	const auto port   = luaL_checknumber(lua_vm, 2);
	
	// verify client
	verify_self(lua_vm);

	try
	{
		_client->connect(host, port);
		lua_pushboolean(lua_vm, true);
	} catch (std::exception& e)
	{
		luaL_error(lua_vm, "Failed to connect to redis server. [%s]\n", e.what());
	}
		
	return 1;
}

int LunaRedisClient::disconnect(lua_State* lua_vm)
{
	// verify client
	verify_self(lua_vm);

	try {
		_client->disconnect();
		lua_pushboolean(lua_vm, true);
	} catch(std::exception& e)
	{
		luaL_error(lua_vm, e.what());
	}

	return 1;
}

int LunaRedisClient::set(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 1);
	
	// Read arguments
	const auto key    = luaL_checkstring(lua_vm, 2);
	const auto value  = luaL_checkstring(lua_vm, 3);

	// verify client
	verify_self(lua_vm);

	g_Module->GetJobManager().PushTask([client = _client, key, value]() -> const std::optional<std::any>
	{
		try {
			return client->set(key, value);
		} catch(std::exception& e)
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

int LunaRedisClient::get(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 1);

	// Read arguments
	const auto key    = luaL_checkstring(lua_vm, 2);

	// verify client
	verify_self(lua_vm);

	g_Module->GetJobManager().PushTask([client = _client, key]() -> const std::optional<std::any>
	{
		try {
			return client->get(key);
		} catch(std::exception& e)
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

int LunaRedisClient::subscribe(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 1);

	// Read arguments
	const auto channel = lua_tostring(lua_vm, 2);

	// verify client
	verify_self(lua_vm);
	
	// subscribe to the channel
	try {
		_client->subscribe(channel, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::string& channel, const std::string& message)
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
		luaL_error(lua_vm, e.what());
	}
	
	return 1;
}
	
int LunaRedisClient::unsubscribe(lua_State* lua_vm)
{
	// Read arugments
	const auto channel = luaL_checkstring(lua_vm, 1);

	// verify client
	verify_self(lua_vm);

	try
	{
		_client->unsubscribe(channel);
		lua_pushboolean(lua_vm, true);
	} catch(std::exception& e)
	{
		luaL_error(lua_vm, e.what());
	}

	return 1;
}

int LunaRedisClient::publish(lua_State* lua_vm)
{
	// Save function ref
	const auto func_ref = utils::lua_getfuncref(lua_vm, 1);

	// Read arguments
	const auto channel = lua_tostring(lua_vm, 2);
	const auto message = lua_tostring(lua_vm, 3);
	
	// verify client
	verify_self(lua_vm);

	g_Module->GetJobManager().PushTask([client = _client, channel, message]() -> const std::optional<std::any>
	{
		try {
			return client->publish(channel, message);
		} catch(std::exception& e)
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
// ReSharper restore CppMemberFunctionMayBeConst
