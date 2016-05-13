#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include "db/db.hxx"
#include "client.hxx"
#include "config.hxx"
#include "misc.hxx"
#include "net.hxx"

Database db;
Socket server;
std::map<std::string, std::string> named_arguments;
std::list<std::string> positional_arguments;

void blockSIGPIPE()
{
	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &act, nullptr);
}

void readArguments(int argc, char **argv)
{
	int k;
	std::string key;
	for(k = 1; k != argc; ++k)
	{
		std::string value(argv[k]);
		if(value.substr(0, 2) == "--")
		{
			if(value == "--")
				break;
			key = value.substr(2);
			continue;
		}
		if(!key.empty())
			named_arguments.emplace(std::move(key), std::move(value));
		else
			positional_arguments.push_back(std::move(value));
		key.clear();
	}
}

std::string getArgument(std::string name, std::string def = "")
{
	auto iter = named_arguments.find(name);
	if(iter != named_arguments.end())
		return iter->second;
	return def;
}

int main(int argc, char **argv)
{
	blockSIGPIPE();
	readArguments(argc, argv);
	std::string file = getArgument("file");
	std::string addr = getArgument("addr", zolden_addr);
	std::string port = getArgument("port", std::to_string(zolden_port));
	std::clog << "Zolden database manipulation program" << std::endl;
	if(!file.empty())
	{
		std::clog << "Reading database from: " << file << std::endl;
		db.readText(file);
	}
	server = Bind(addr, port, true);
	if(0 != listen(server.get(), 20))
		throw std::system_error(errno, std::system_category(), "Can't listen on a socket");
	std::clog << "Listening at " << addr << ":" << port << std::endl;
	for(;;)
	{
		Socket fd(accept(server.get(), nullptr, nullptr));
		if(!fd)
			throw std::system_error(errno, std::system_category(), "Can't accept new connection");
		std::string id = std::to_string(fd.get());
		std::clog << "[MAIN] Client connected. (" + id + ")\n" << std::flush;
		std::thread t(Client(std::move(fd)));
		t.detach();
		std::clog << "[MAIN] Thread started. (" + id + ")\n" << std::flush;
	}
	return 0;
}
