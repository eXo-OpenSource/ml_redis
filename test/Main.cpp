#include <iostream>
#include "cpp_redis/cpp_redis"

int main(int argc, char* argv[])
{
	try {
		const auto w_version_requested = MAKEWORD(2, 2);
		WSADATA wsa_data;
		const auto err = WSAStartup(w_version_requested, &wsa_data);
		if (err != 0) {
	        /* Tell the user that we could not find a usable */
	        /* Winsock DLL.                                  */
	        printf("WSAStartup failed with error: %d\n", err);
	        return 1;
		}

		cpp_redis::client client;
		client.connect();

		client.set("lol", "redis", [](cpp_redis::reply& reply)
		{
			std::cout << "Reply: " << reply << std::endl;
		});
		client.get("lol", [](cpp_redis::reply& reply) {
			std::cout << "Got from redis: " << reply << std::endl;
		});
		client.sync_commit();
	} catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	std::cin.get();
	return 0;
}
