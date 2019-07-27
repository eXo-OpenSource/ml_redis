#pragma once
#include <unordered_set>
#include <any>
#include <optional>

#include "JobManager.h"
#include "RedisClient.h"
#include "ILuaModuleManager.h"

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

	inline void AddRedisClient(redis_client* client) { _redisClients.insert(client); }
	inline void RemoveRedisClient(redis_client* client) { _redisClients.erase(client); }
	inline bool HasRedisClient(redis_client* client) { return _redisClients.find(client) != _redisClients.end(); }

	inline JobManager<const std::optional<std::any>>& GetJobManager() { return _jobManager; }

private:
	ILuaModuleManager* _moduleManager;
	JobManager<const std::optional<std::any>> _jobManager;
	std::unordered_set<lua_State*> _luaStates;
	std::unordered_set<redis_client*> _redisClients;
};

extern Module* g_Module;
