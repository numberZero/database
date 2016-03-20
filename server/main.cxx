#include <iostream>
#include <memory>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "binary/network.hxx"
#include "db/db.hxx"
#include "net.hxx"

Database db;
Socket server;

class Client
{
	Socket socket;

public:
	Client(Socket &&s);
	void operator() ();
};

Client::Client(Socket &&s) :
	socket(std::move(s))
{
}

void Client::operator() ()
{
}

int main(int argc, char **argv)
{
	if(argc > 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " [ <data-file-name> ]" << std::endl;
		std::cerr << std::endl;
		return -1;
	}
	std::clog << "Database manipulation program" << std::endl;
	if(argc == 2)
	{
		std::clog << "Reading database from: " << argv[1] << std::endl;
		db.readText(argv[1]);
	}
	server = Bind("127.0.0.1", std::to_string(zolden_port));
	if(0 != listen(server.get(), 20))
		throw std::system_error(errno, std::system_category(), "Can't listen on a socket");
	for(;;)
	{
		Socket fd(accept(server.get(), nullptr, nullptr));
		if(!fd)
			throw std::system_error(errno, std::system_category(), "Can't accept new connection");
		std::clog << "Client connected. ";
		std::thread t(Client(std::move(fd)));
		t.detach();
		std::clog << "Thread started." << std::endl;
	}
	return 0;
}
