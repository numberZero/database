#include <cassert>
#include <iostream>
#include <memory>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "binary/network.hxx"
#include "db/db.hxx"
#include "db/select.hxx"
#include "bio.hxx"
#include "misc.hxx"
#include "net.hxx"

Database db;
Socket server;

NEW_ERROR_CLASS(InvalidRequestException, runtime_error, std);

class Client
{
	Socket socket;

	void select(SelectionParams const &sp);

public:
	Client(Socket &&s);
	void operator() ();
};

Client::Client(Socket &&s) :
	socket(std::move(s))
{
}

void Client::select(SelectionParams const &sp)
{
	std::clog << "SELECT called" << std::endl;
	for(Selection sel(db, sp); sel.isValid(); sel.next())
	{
		RowData row = sel.getRow().getData();
		std::size_t size;
		size = NetworkType<RowData>::dynamic_size(row);
		std::unique_ptr<char[]> packet(new char[size]);
		assert(size == NetworkType<RowData>::dynamic_serialize(packet.get(), size, row));
		writePacket(socket.get(), packet.get(), size);
	}
	writePacket(socket.get(), nullptr, 0); // end of data
}

void Client::operator() ()
{
	try
	{
		for(;;)
		{
			std::size_t rem;
			std::size_t count;
			std::unique_ptr<char[]> packet(readPacket(socket.get(), rem));
			std::clog << "Packet arrived" << std::endl;
			char *ptr = packet.get();
			QueryType qt;
			SelectionParams sp;
			count = NetworkType<QueryType>::dynamic_parse(ptr, rem, qt);
			ptr += count;
			rem -= count;
			switch(qt)
			{
			case QueryType::Select:
				count = NetworkType<SelectionParams>::dynamic_parse(ptr, rem, sp);
				if(count != rem)
					throw InvalidRequestException("Garbage after request");
				packet.reset();
				select(sp);
				break;
//			case QueryType::Insert:
//				break;
//			case QueryType::Remove:
//				break;
			default:
				throw InvalidRequestException("Unknown request type");
			}
		}
	}
	catch(packer::PackingError const& e)
	{
		std::clog << "Invalid packet: " << e.what() << std::endl;
	}
	catch(InvalidRequestException const& e)
	{
		std::clog << "Invalid request: " << e.what() << std::endl;
	}
	catch(BioEof const& e)
	{
		std::clog << "Client disconnect" << std::endl;
	}
	catch(...)
	{
		std::clog << "Client error" << std::endl;
	}
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
