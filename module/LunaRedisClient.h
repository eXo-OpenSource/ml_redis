#pragma once
#include "Module.h"

class ILuaModuleManager10;
struct lua_State;

extern ILuaModuleManager10* pModuleManager;

// http://lua-users.org/wiki/SimplerCppBinding
class LunaRedisClient
{
public:
	static const char className[];
	static Luna<LunaRedisClient>::RegType methods[];

	LunaRedisClient(lua_State*);
	
	void verify_self(lua_State* lua_vm) const;

	int connect(lua_State* lua_vm);
	int disconnect(lua_State* lua_vm);
	int set(lua_State* lua_vm);
	int get(lua_State* lua_vm);
	int subscribe(lua_State* lua_vm);
	int unsubscribe(lua_State* lua_vm);
	int publish(lua_State* lua_vm);

private:
	std::shared_ptr<redis_client> _client;
};
