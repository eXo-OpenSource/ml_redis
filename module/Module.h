#pragma once
#include <unordered_set>
#include <any>
#include <optional>
#include <cstring>

#include "ILuaModuleManager.h"
#include "JobManager.h"
#include "RedisClient.h"
#include "Luna.h"

struct lua_State;
class ILuaModuleManager;

class Module
{
public:
	Module(ILuaModuleManager* manager);
	~Module();

	void Start();
	void Process();

	inline void AddLuaVM(lua_State* luaVM) { _luaStates.insert(luaVM); }
	inline void RemoveLuaVM(lua_State* luaVM) { _luaStates.erase(luaVM); }
	inline bool HasLuaVM(lua_State* luaVM) { return _luaStates.find(luaVM) != _luaStates.end(); }

	inline void AddRedisClient(const std::shared_ptr<redis_client>& client) { _redisClients.insert(client); }
	inline void RemoveRedisClient(const std::shared_ptr<redis_client>& client) { _redisClients.erase(client); }
	inline bool HasRedisClient(const std::shared_ptr<redis_client>& client) { return _redisClients.find(client) != _redisClients.end(); }

	inline JobManager<const std::optional<std::any>>& GetJobManager() { return _jobManager; }

	static void register_table_function(lua_State* lua_vm, const char* function_name, lua_CFunction function);
	static void register_class(lua_State* lua_vm, const char* metatable, const luaL_Reg* registry);
	
private:
	ILuaModuleManager* _moduleManager;
	JobManager<const std::optional<std::any>> _jobManager;
	std::unordered_set<lua_State*> _luaStates;
	std::unordered_set<std::shared_ptr<redis_client>> _redisClients;
};

extern Module* g_Module;
