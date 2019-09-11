#include "Common.h"
#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include <cstring>
#include "Module.h"
#include <cassert>

#ifndef WIN32
	#include "luaimports/luaimports.linux.h"
#endif

ILuaModuleManager10* pModuleManager = nullptr;

// Initialization function (module entrypoint)
MTAEXPORT bool InitModule(ILuaModuleManager10* pManager, char* szModuleName, char* szAuthor, float* fVersion)
{
#ifndef WIN32
	ImportLua();
#endif

    pModuleManager = pManager;

	// Set the module info
    const auto module_name	= "Redis client module";
    const auto author		= "StiviK";
	std::memcpy(szModuleName, module_name,	MAX_INFO_LENGTH);
	std::memcpy(szAuthor,     author,		MAX_INFO_LENGTH);
	*fVersion = 1.0f;
	
	// Load module
	g_Module = new Module(pManager);
	g_Module->Start();

    return true;
}

MTAEXPORT void RegisterFunctions(lua_State* lua_vm)
{
	if (!pModuleManager || !lua_vm)
		return;

	// Add lua vm to states list (to check validity)
	g_Module->AddLuaVM(lua_vm);

	// Register functions
	pModuleManager->RegisterFunction(lua_vm, "redisTest",			&CFunctions::Test);

	// Register globals
	lua_newtable(lua_vm);
		Module::register_lua_table_function(lua_vm, "create", &CFunctions::redis_create_client);
		Module::register_lua_table_function(lua_vm, "destroy", &CFunctions::redis_client_destruct);
		Module::register_lua_table_function(lua_vm, "connect", &CFunctions::redis_connect);
		Module::register_lua_table_function(lua_vm, "disconnect", &CFunctions::redis_disconnect);
		Module::register_lua_table_function(lua_vm, "set", &CFunctions::redis_set);
		Module::register_lua_table_function(lua_vm, "get", &CFunctions::redis_get);


	
		Module::register_lua_table_function(lua_vm, "test", &CFunctions::redis_test);
	lua_setglobal(lua_vm, "Redis");
}

MTAEXPORT bool DoPulse()
{
	g_Module->Process();

	return true;
}

MTAEXPORT bool ShutdownModule()
{
	// Unload module
	delete g_Module;

    return true;
}

MTAEXPORT bool ResourceStopping(lua_State* luaVM)
{
	// Invalidate lua vm by removing it from the valid list
	g_Module->RemoveLuaVM(luaVM);

	return true;
}

MTAEXPORT bool ResourceStopped(lua_State* luaVM)
{
	return true;
}
