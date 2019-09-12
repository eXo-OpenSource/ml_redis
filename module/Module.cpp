#include <cpp_redis/core/client.hpp>
#include "Module.h"
#include "CFunctions.h"
Module* g_Module = nullptr;

constexpr std::size_t kNumWorkers = 2;

Module::Module(ILuaModuleManager* manager) : _moduleManager(manager), _jobManager(kNumWorkers)
{
}

Module::~Module()
{
	// Shutdown job manager threads
	_jobManager.Stop();
}

void Module::Start()
{
	// Start job manager worker threads
	_jobManager.Start();
}

void Module::Process()
{
	// Call complete callbacks on main thread
	_jobManager.SpreadResults();
}

void Module::register_lua_table_function(lua_State* lua_vm, const char* function_name, lua_CFunction function)
{
	lua_pushstring(lua_vm, function_name);
		lua_pushcfunction(lua_vm, function);
	lua_settable(lua_vm, -3);
}

void Module::register_redis_class(lua_State* lua_vm)
{
	// https://www.lua.org/pil/28.2.html
	// Metatable definition
	luaL_Reg redis_client_regs[] = {
		// Meta methods
		{ "__gc",		&CFunctions::redis_client_destruct_new },
		
		// Methods
		{ "new",		&CFunctions::redis_create_client_new },
		{ "connect",	&CFunctions::redis_connect_new },
		{ "get",		&CFunctions::redis_get_new },
		{ "set",		&CFunctions::redis_set_new },
		{ nullptr, nullptr }
	};

	// Register `RedisClient` metatable
	luaL_newmetatable(lua_vm, "RedisClient");
		lua_pushstring(lua_vm, "__index");
			lua_pushvalue(lua_vm, -2);
		lua_settable(lua_vm, -3);
	luaL_openlib(lua_vm, nullptr, redis_client_regs, 0);
	
	lua_setglobal(lua_vm, "RedisClient");
}
