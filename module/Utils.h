#pragma once
#include "lua.h"
#include <string>
#include <stdexcept>
#include <sstream>
#include <unordered_map>

static const int index_value = -1;
static const int index_key   = -2;

class utils
{
public:	
	template<class _Type>
	static _Type* lua_checkobject(lua_State* lua_vm, const char* metatable, const int index = 1)
	{
		// get and verify our heap ptr and dereference it
		return *static_cast<_Type**>(luaL_checkudata(lua_vm, index, metatable));
	}

	template<class _Type>
	static void lua_createobject(lua_State* lua_vm, _Type* heap_object, const char* metatable)
	{
		const auto udata = static_cast<_Type**>(lua_newuserdata(lua_vm, sizeof(_Type*))); // Allocate space for a ptr to our heap object ptr
		*udata = heap_object; // assign our heap object to the udata ptr

		luaL_getmetatable(lua_vm, metatable); // assign the metatable to our udata
		lua_setmetatable(lua_vm, -2);
	}

	static int lua_getfuncref(lua_State* lua_vm, const int index)
	{
		// Verify argument
		luaL_checktype(lua_vm, index, LUA_TFUNCTION);
		
		// Save reference of the Lua callback function
		// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
		lua_pushvalue(lua_vm, index);
		return luaL_ref(lua_vm, LUA_REGISTRYINDEX);
	}
	
	static inline std::unordered_map<std::string, std::string> parse_table(lua_State* lua_vm, const int index)
	{
		std::unordered_map<std::string, std::string> result;

		for(lua_pushnil(lua_vm); lua_next(lua_vm, index) != 0; lua_pop(lua_vm, 1))
		{
			const auto type_key   = lua_type(lua_vm, index_key);
			const auto type_value = lua_type(lua_vm, index_value);
			if (type_key != LUA_TSTRING && type_key != LUA_TNUMBER)
			{
				std::stringstream ss;
				ss << "got invalid table key, expected string or number got " << lua_typename(lua_vm, type_key);
				throw std::runtime_error(ss.str());
			}

			if (type_value != LUA_TSTRING && type_value != LUA_TNUMBER)
			{
				std::stringstream ss;
				ss << "got invalid table value, expected string or number got " << lua_typename(lua_vm, type_value);
				throw std::runtime_error(ss.str());
			}

			auto to_string = [lua_vm](const int index) -> const std::string
			{
				const auto type = lua_type(lua_vm, index);
				if (type == LUA_TSTRING)
				{
					return lua_tostring(lua_vm, index);
				}

				if (type == LUA_TNUMBER)
				{
					return std::to_string(lua_tointeger(lua_vm, index));
				}
				return {};
			};

			result[to_string(index_key)] = to_string(index_value);
		}

		return result;
	}
};

