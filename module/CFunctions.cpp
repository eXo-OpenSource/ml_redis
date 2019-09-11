#include "Module.h"
#include "CFunctions.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::redis_create_client(lua_State* lua_vm)
{
	// redis_client redis_create_client()
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_create_client.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}
	
	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);
	
	g_Module->GetJobManager().PushTask([]() -> const std::optional<std::any>
	{	
		try
		{
			return new ml_redis::redis_client();
		} catch(std::exception& e)
		{
			stringstream ss;
			ss << "Failed to create to redis client. [" << e.what() << "]\n";
			pModuleManager->ErrorPrintf(ss.str().c_str());
			
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
				lua_pushlightuserdata(lua_vm, std::any_cast<ml_redis::redis_client*>(result.value()));
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

int CFunctions::redis_client_destruct(lua_State* lua_vm)
{
	// bool redis_client_destruct(redis_client client)
	if (lua_type(lua_vm, 1) != LUA_TLIGHTUSERDATA)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_client_destruct.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 1));

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_client_destruct, invalid client has been passed.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	delete client;
	lua_pushboolean(lua_vm, true);
	return 1;
}

int CFunctions::redis_connect(lua_State* lua_vm)
{
	// bool redis_connect(redis_client client, string host, number port)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 3) != LUA_TSTRING || lua_type(lua_vm, 4) != LUA_TNUMBER)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_connect.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);
	
	// Read arguments
	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 2));
	const auto host   = lua_tostring(lua_vm, 3);
	const auto port   = lua_tonumber(lua_vm, 4);

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_connect, invalid client has been passed.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	g_Module->GetJobManager().PushTask([client, host, port]() -> const std::optional<std::any>
	{	
		try
		{
			client->connect(host, port);
			
			return true;
		} catch(std::exception& e)
		{
			stringstream ss;
			ss << "Failed to connect to redis server. [" << e.what() << "]\n";
			pModuleManager->ErrorPrintf(ss.str().c_str());
			
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
				const auto connect_result = std::any_cast<bool>(result.value());
				lua_pushboolean(lua_vm, connect_result);
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

int CFunctions::redis_disconnect(lua_State* lua_vm)
{
	// void redis_disconnect(redis_client client)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TLIGHTUSERDATA)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_disconnect.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);
	
	// Read arguments
	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 2));

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_disconnect, invalid client has been passed.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	g_Module->GetJobManager().PushTask([lua_vm, client]() -> const std::optional<std::any>
	{
		try {
			client->disconnect();
			
			return true;
		} catch(exception& e)
		{
			std::cout << e.what() << std::endl;
			lua_pushboolean(lua_vm, false);
			
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
				const auto connect_result = std::any_cast<bool>(result.value());
				lua_pushboolean(lua_vm, connect_result);
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

int CFunctions::redis_set(lua_State* lua_vm)
{
	// bool redis_set(redis_client client, string key, string value)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 3) != LUA_TSTRING || lua_type(lua_vm, 4) != LUA_TSTRING)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_set.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);
	
	// Read arguments
	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 2));
	const auto key    = lua_tostring(lua_vm, 3);
	const auto value  = lua_tostring(lua_vm, 4);

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_set, invalid client has been passed.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	g_Module->GetJobManager().PushTask([lua_vm, client, key, value]() -> const std::optional<std::any>
	{
		try {
			return client->set(key, value);
		} catch(exception& e)
		{
			std::cout << e.what() << std::endl;
			lua_pushboolean(lua_vm, false);
			
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
	// string redis_get(redis_client client, string key)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 3) != LUA_TSTRING)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_get.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);

	// Read arguments
	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 2));
	const auto key    = lua_tostring(lua_vm, 3);

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_get, invalid client has been passed.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	g_Module->GetJobManager().PushTask([lua_vm, client, key]() -> const std::optional<std::any>
	{
		try {
			return client->get(key);
		} catch(exception& e)
		{
			std::cout << e.what() << std::endl;
			lua_pushboolean(lua_vm, false);
			
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
	// string redis_get(redis_client client, string key)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 3) != LUA_TSTRING)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_subscribe.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);

	// Read arguments
	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 2));
	const auto channel = lua_tostring(lua_vm, 3);

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_get, invalid client has been passed.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}
	
	// subscribe to the channel
	client->subscribe(channel, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::string& channel, const std::string& message)
	{
		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		if (!g_Module->HasLuaVM(lua_vm))
			return;

		// Push stored reference to callback function to the stack
		lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);
		
		lua_pushstring(lua_vm, channel.c_str());
		lua_pushstring(lua_vm, message.c_str());
		
		// Finally, call the function
		const auto err = lua_pcall(lua_vm, 2, 0, 0);
		if (err != 0)
			pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
	});

	lua_pushboolean(lua_vm, true);
	return 1;
}

int CFunctions::redis_publish(lua_State* lua_vm)
{
	// bool redis_publish(redis_client client, string channel, string message)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 3) != LUA_TSTRING || lua_type(lua_vm, 4) != LUA_TSTRING)
	{
		pModuleManager->ErrorPrintf("Bad argument @ redis_publish.\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref = luaL_ref(lua_vm, LUA_REGISTRYINDEX);

	// Read arguments
	const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 2));
	const auto channel = lua_tostring(lua_vm, 3);
	const auto message = lua_tostring(lua_vm, 4);

	// verify client
	if (!g_Module->HasRedisClient(client))
	{
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	g_Module->GetJobManager().PushTask([lua_vm, client, channel, message]() -> const std::optional<std::any>
	{
		try {
			return client->publish(channel, message);
		} catch(exception& e)
		{
			std::cout << e.what() << std::endl;
			lua_pushboolean(lua_vm, false);
			
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
		const auto client = static_cast<ml_redis::redis_client*>(lua_touserdata(lua_vm, 1));

		// verify client
		if (!g_Module->HasRedisClient(client))
		{
			lua_pushboolean(lua_vm, false);
			return 1;
		}

		lua_pushboolean(lua_vm, client->is_connected());
		return 1;
	} catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		lua_pushboolean(lua_vm, false);
		return 1;
	}
}
