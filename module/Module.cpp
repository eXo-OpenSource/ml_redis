#include <cpp_redis/core/client.hpp>
#include "Module.h"
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
