#include <cassert>
#include <iostream>
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

void blockSIGPIPE()
{
	struct sigaction act;
	std::memset(&act, 0, sizeof(act));
	act.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &act, nullptr);
}

int main(int argc, char **argv)
{
	blockSIGPIPE();
	if(argc > 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " [ <data-file-name> ]" << std::endl;
		std::cerr << std::endl;
		return -1;
	}
	std::clog << "Zolden database manipulation program" << std::endl;
	if(argc == 2)
	{
		std::clog << "Reading database from: " << argv[1] << std::endl;
		db.readText(argv[1]);
	}
	server = Bind(zolden_addr, std::to_string(zolden_port), true);
	if(0 != listen(server.get(), 20))
		throw std::system_error(errno, std::system_category(), "Can't listen on a socket");
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
