#include <cassert>
#include "binary/network.hxx"
#include "db/select.hxx"
#include "db/db.hxx"
#include "bio.hxx"
#include "client.hxx"

extern Database db;

NEW_ERROR_CLASS(InvalidRequestException, runtime_error, std);

Client::Client(Socket &&s) :
	socket(std::move(s))
{
}

void Client::sendAnswerHeader(bool success, int mc)
{
	ResultHeader hdr { success, mc };
	std::size_t const size = NetworkType<ResultHeader>::StaticSize;
	char buffer[size];
	NetworkType<ResultHeader>::static_serialize(buffer, hdr);
	writePacket(socket.get(), buffer, size);
}

void Client::sendMessage(int errcode, std::string message)
{
	ResultMessage msg { errcode, message };
	std::size_t size = NetworkType<ResultMessage>::dynamic_size(msg);
	std::unique_ptr<char[]> buffer(new char[size]);
	char *body = buffer.get();
	assert(size == NetworkType<ResultMessage>::dynamic_serialize(body, size, msg));
	writePacket(socket.get(), buffer.get(), size);
}

void Client::sendAnswerHeader(int errcode, std::string message)
{
	sendAnswerHeader(false, 1);
	sendMessage(errcode, message);
}

void Client::select(SelectionParams const &sp)
{
	std::clog << "SELECT called" << std::endl;
	sendAnswerHeader();
	for(Selection sel(db.select(sp)); sel.isValid(); sel.next())
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

void Client::insert(RowData const &row)
{
	db.insert(row);
	sendAnswerHeader();
}

void Client::remove(SelectionParams const &rp)
{
	db.remove(rp);
	sendAnswerHeader();
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
			RowData row;
			count = NetworkType<QueryType>::dynamic_parse(ptr, rem, qt);
			ptr += count;
			rem -= count;
			switch(qt)
			{
			case QueryType::Select:
				count = NetworkType<SelectionParams>::dynamic_parse(ptr, rem, sp);
				if(count != rem)
					throw InvalidRequestException("Garbage after request (SELECT)");
				packet.reset();
				select(sp);
				break;

			case QueryType::Insert:
				count = NetworkType<RowData>::dynamic_parse(ptr, rem, row);
				if(count != rem)
					throw InvalidRequestException("Garbage after request (INSERT)");
				packet.reset();
				insert(row);
				break;

			case QueryType::Remove:
				count = NetworkType<SelectionParams>::dynamic_parse(ptr, rem, sp);
				if(count != rem)
					throw InvalidRequestException("Garbage after request (REMOVE)");
				packet.reset();
				remove(sp);
				break;

			default:
				sendAnswerHeader(400, "Unknown request type");
#ifdef DISCONNECT_ON_INVALID_REQUEST
				throw InvalidRequestException("Unknown request type");
#endif
			}
		}
	}
	catch(packer::PackingError const &e)
	{
		std::clog << "Invalid packet: " << e.what() << std::endl;
	}
	catch(InvalidRequestException const &e)
	{
		std::clog << "Invalid request: " << e.what() << std::endl;
	}
	catch(BioEof const &e)
	{
		std::clog << "Client disconnect" << std::endl;
	}
	catch(std::exception const &e)
	{
		std::clog << "Client error: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::clog << "Client error" << std::endl;
	}
}
