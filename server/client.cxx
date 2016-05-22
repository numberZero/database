#include <cassert>
#ifdef USE_MANUAL_TCP_FLUSH
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#include "protocol.hxx"
#include "db/select.hxx"
#include "db/db.hxx"
#include "bio.hxx"
#include "client.hxx"
#include "net.hxx"

NEW_ERROR_CLASS(InvalidRequestException, runtime_error, std);

Client::Client(File &&s) :
	socket(std::move(s))
{
#ifdef USE_MANUAL_TCP_FLUSH
	SetSocketOption(socket, IPPROTO_TCP, TCP_CORK, 1);
#endif
}

void Client::sendAnswerHeader(bool success, int mc)
{
	ResultHeader hdr { success, mc };
	std::size_t const size = NetworkType<ResultHeader>::StaticSize;
	char buffer[size];
	NetworkType<ResultHeader>::static_serialize(buffer, hdr);
	writePacket(socket, buffer, size);
}

void Client::sendMessage(int errcode, std::string message)
{
	ResultMessage msg { errcode, message };
	std::size_t size = NetworkType<ResultMessage>::dynamic_size(msg);
	std::unique_ptr<char[]> buffer(new char[size]);
	char *body = buffer.get();
	std::size_t bytes = NetworkType<ResultMessage>::dynamic_serialize(body, size, msg);
	assert(size == bytes);
	writePacket(socket, buffer.get(), size);
}

void Client::sendAnswerHeader(int errcode, std::string message)
{
	sendAnswerHeader(false, 1);
	sendMessage(errcode, message);
}

void Client::flush()
{
#ifdef USE_MANUAL_TCP_FLUSH
	SetSocketOption(socket, IPPROTO_TCP, TCP_NODELAY, 1);
#endif
}

void Client::select(SelectionParams const &sp)
{
#ifndef NDEBUG
	std::clog << "SELECT called" << std::endl;
#endif
	sendAnswerHeader();
	for(Selection sel(db->select(sp)); sel.isValid(); sel.next())
	{
		RowData row = sel.getRow().getData();
		std::size_t size;
		size = NetworkType<RowData>::dynamic_size(row);
		std::unique_ptr<char[]> packet(new char[size]);
		std::size_t bytes = NetworkType<RowData>::dynamic_serialize(packet.get(), size, row);
		assert(size == bytes);
		writePacket(socket, packet.get(), size);
	}
	writePacket(socket, nullptr, 0); // end of data
	flush();
}

void Client::insert(RowData const &row)
{
	db->insert(row);
	sendAnswerHeader();
	flush();
}

void Client::remove(SelectionParams const &rp)
{
	db->remove(rp);
	sendAnswerHeader();
	flush();
}

void Client::operator() ()
{
	std::string pref = "[" + std::to_string(socket.get()) + "] ";
	try
	{
		for(;;)
		{
			std::size_t rem;
			std::size_t count;
			std::unique_ptr<char[]> packet(readPacket(socket, rem));
#ifndef NDEBUG
			std::clog << pref + " Packet arrived\n" << std::flush;
#endif
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
		std::clog << pref + "Invalid packet: " << e.what() << std::endl;
	}
	catch(InvalidRequestException const &e)
	{
		std::clog << pref + "Invalid request: " << e.what() << std::endl;
	}
	catch(BioEof const &e)
	{
		std::clog << pref + "Client disconnect" << std::endl;
	}
	catch(std::system_error const &e)
	{
		std::clog << pref + "Client thread system error: " << e.what() << " (" << e.code() << ")" << std::endl;
	}
	catch(std::logic_error const &e)
	{
		std::clog << pref + "Database error: " << e.what() << std::endl;
		std::terminate();
	}
	catch(std::exception const &e)
	{
		std::clog << pref + "Unknown client thread error: " << e.what() << std::endl;
		std::terminate();
	}
	catch(...)
	{
		std::clog << pref + "Non-standard client thread error" << std::endl;
		std::terminate();
	}
}
