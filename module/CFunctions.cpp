#include <sstream>

#include "Module.h"
#include "CFunctions.h"
	
#ifndef _WIN32
	#include <sys/stat.h>
#endif
int CFunctions::redis_create_client(lua_State* lua_vm)
{
	try {
		auto client = new cpp_redis::client();
		client->connect();

		lua_pushlightuserdata(lua_vm, static_cast<void*>(client));
	} catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		lua_pushboolean(lua_vm, false);
	}

	return 1;
}

int CFunctions::redis_test(lua_State* lua_vm)
{
	try
	{
		const auto ptr = static_cast<cpp_redis::client*>(lua_touserdata(lua_vm, 1));
		std::cout << ptr->is_connected() << std::endl;
	} catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
		lua_pushboolean(lua_vm, false);
	}

	return 1;
}
