#include "Module.h"
#include <ostream>
#include <iostream>
#include <cpp_redis/core/reply.hpp>
#include <cpp_redis/core/client.hpp>
#include <tacopie/network/tcp_socket.hpp>

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
