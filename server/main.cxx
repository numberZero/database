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

void emptySignalHandler(int signal)
{
	std::clog << "Signal caught: " + std::to_string(signal) << std::endl;
}

void setupSignalHandling()
{
	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = emptySignalHandler;
	sigaction(SIGPIPE, &act, nullptr);
	sigaction(SIGINT, &act, nullptr);
}

void daemonize()
{
	pid_t child = fork();
	if(child == 0)
		return;
	if(child < 0)
		throw std::system_error(errno, std::system_category(), "Can't go to background");
	std::cout << child << std::endl; // Return PID
	std::exit(0);
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
			named_arguments.emplace(key, "");
			continue;
		}
		if(!key.empty())
			named_arguments.at(key) = std::move(value);
		else
			positional_arguments.push_back(std::move(value));
		key.clear();
	}
}

bool hasArgument(std::string name)
{
	auto iter = named_arguments.find(name);
	return iter != named_arguments.end();
}

std::string getArgument(std::string name, std::string def = "")
{
	auto iter = named_arguments.find(name);
	if(iter != named_arguments.end())
		return iter->second;
	return def;
}

struct TestTable: public BackedTable
{
	TestTable() : BackedTable(12, "/tmp/table1.dat")
	{
		std::cout << create().first << std::endl;
		std::cout << create().first << std::endl;
		std::cout << get(1) << std::endl;
		memset(get(1), 'z', 5);
		memset(get(1) + 5, 'a', 5);
		std::cout << create().first << std::endl;
		std::cout << create().first << std::endl;
	}

	~TestTable()
	{
		std::cout << "Finalization" << std::endl;
	}
};

TestTable test1;

int main(int argc, char **argv)
{
	setupSignalHandling();
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
	if(hasArgument("daemon"))
	{
		std::clog << "Going into background..." << std::endl;
		daemonize();
		std::clog << "Daemonized" << std::endl;
	}
	else
		std::clog << "Remaining in foreground" << std::endl;
	for(;;)
	{
		Socket fd(accept(server.get(), nullptr, nullptr));
		if(!fd)
		{
			if(errno == EINTR)
				break;
			throw std::system_error(errno, std::system_category(), "Can't accept new connection");
		}
		std::string id = std::to_string(fd.get());
		std::cerr << "[MAIN] Client connected. (" + id + ")\n";
		std::thread t(Client(std::move(fd)));
		t.detach();
		std::cerr << "[MAIN] Thread started. (" + id + ")\n";
	}
	std::clog << "Shutting down" << std::endl;
	pthread_exit(nullptr);
}
