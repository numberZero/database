#include <cassert>
#include <cctype>
#include <string>
#include <sstream>
#include "protocol.hxx"
#include "bio.hxx"
#include "misc.hxx"
#include "query.hxx"

QueryMachineState global_state {
	File(),
	std::cin,
	std::cout,
	SelectionParams()
};

/**
 * Sends a query in a well-defined format
 * 
 * \tparam DataType Content type (required)
 * \param socket The connection to the server
 * \param [in] type Query type
 * \param [in] data Query contents
 * \throws std::exception subclasses when something goes wrong (see `packer::*`, `writePacket`, `new` for details)
 */
template <typename DataType>
void sendQuery(File &socket, QueryType type, DataType data)
{
	std::size_t head_size = NetworkType<QueryType>::StaticSize;
	std::size_t body_size = NetworkType<DataType>::dynamic_size(data);
	std::size_t packet_size = head_size + body_size;
	std::unique_ptr<char[]> buffer(new char[packet_size]);
	char *packet = buffer.get();
	char *head = packet;
	char *body = packet + head_size;
	NetworkType<QueryType>::static_serialize(head, type);
	std::size_t bytes = NetworkType<DataType>::dynamic_serialize(body, body_size, data);
	assert(body_size == bytes);
	writePacket(socket, packet, packet_size);
}

/**
 * Reads the server answer
 * 
 * \tparam DataType Content type (required)
 * \param socket The connection to the server
 * \param [out] data Query contents
 * \returns Is answer present (that is, non-empty)
 * \throws ProtocolError if answer is not in the correct form
 * \throws std::exception subclasses when something goes wrong (see `packer::*`, `readPacket`, `new` for details)
 */
template <typename DataType>
bool recvAnswer(File &socket, DataType &data)
{
	char *packet;
	std::size_t packet_size;
	std::size_t bytes;
	readPacket(socket, packet, packet_size);
	std::unique_ptr<char[]> buffer(packet); // auto-deleter
	if(!packet_size)
		return false;
	try
	{
		bytes = NetworkType<DataType>::dynamic_parse(packet, packet_size, data);
	}
	catch(packer::ParseError const &e)
	{
		throw ProtocolError(e.what());
	}
	if(bytes != packet_size)
		throw ProtocolError("Invalid packet");
	return true;
}

/**
 * Reads the server answer
 * 
 * \tparam DataType Content type (required)
 * \param socket The connection to the server
 * \returns The answer
 * \throws ProtocolError if answer absent or is not in the correct form
 * \throws std::exception subclasses when something goes wrong
 */
template <typename DataType>
DataType recvAnswer(File &socket)
{
	DataType result;
	if(!recvAnswer<DataType>(socket, result))
		throw ProtocolError("Answer is required");
	return std::move(result);
}

void processAnswerHeader(File &socket, std::ostream &cout)
{
	ResultHeader header = recvAnswer<ResultHeader>(socket);
	if(header.success)
	{
		if(!header.message_count)
			return;
		for(int k = 0; k != header.message_count; ++k)
		{
			ResultMessage message = recvAnswer<ResultMessage>(socket);
			cout << "[warn] #" << message.code << ": " << message.text << std::endl;
		}
		return;
	}
	if(!header.message_count)
		throw RemoteError("Unknown server-side error");
	if(header.message_count == 1)
	{
		ResultMessage message = recvAnswer<ResultMessage>(socket);
		throw RemoteError("Error #" + std::to_string(message.code) + ": " + message.text);
	}
	std::stringstream str;
	str << "Multiple errors:\n";
	for(int k = 0; k != header.message_count; ++k)
	{
		ResultMessage message = recvAnswer<ResultMessage>(socket);
		str << "[fail] #" << message.code << ": " << message.text << std::endl;
	}
	throw RemoteError(str.str());
}

char const *QuerySelect::name() const
{
	if(re)
		return "RESELECT";
	else
		return "SELECT";
}

void QuerySelect::perform()
{
	if(!re)
		global_state.params = SelectionParams();
	global_state.params.refine(params);
}

char const *QueryInsert::name() const
{
	return "INSERT";
}

void QueryInsert::perform()
{
	sendQuery<RowData>(global_state.connection, QueryType::Insert, row);
	processAnswerHeader(global_state.connection, global_state.cout);
}

char const *QueryRemove::name() const
{
	return "REMOVE";
}

void QueryRemove::perform()
{
	sendQuery<SelectionParams>(global_state.connection, QueryType::Remove, params);
	processAnswerHeader(global_state.connection, global_state.cout);
}

char const *QueryPrint::name() const
{
	return "PRINT";
}

void QueryPrint::perform()
{
	global_state.params.clearReturn();
	global_state.params.refine(params);
	sendQuery<SelectionParams>(global_state.connection, QueryType::Select, global_state.params);
	processAnswerHeader(global_state.connection, global_state.cout);
	for(std::size_t id = 1;; ++id)
	{
		RowData row;
		if(!recvAnswer<RowData>(global_state.connection, row))
		{ // end of data
			global_state.cout << "*** END ***" << std::endl;
			break;
		}
		global_state.cout << "*** Row " << id << " ***\n";
#define PRINTPAR(name) \
		if(global_state.params.name.do_return) \
			global_state.cout << #name ": " << row.name << "\n"
		PRINTPAR(teacher);
		PRINTPAR(subject);
		PRINTPAR(room);
		PRINTPAR(group);
		PRINTPAR(day);
		PRINTPAR(lesson);
		global_state.cout << std::endl;
	}
}

char const *QueryHelp::name() const
{
	return "HELP";
}

void QueryHelp::perform()
{
	global_state.cout << "=== Help ===\n";
	global_state.cout << "Queries available:\n";
	global_state.cout << "\tselect\tSelect rows for following print\n";
	global_state.cout << "\treselect\tUpdate current selection\n";
	global_state.cout << "\tprint\tPrint rows from the current selection\n";
	global_state.cout << "\tinsert\tAdd row to the database\n";
	global_state.cout << "\tremove\tDelete rows from the database\n";
	global_state.cout << "\thelp\tShow this help\n";
	global_state.cout << "\texit\tExit the client\n";
	global_state.cout << std::flush;
}

char const *QueryExit::name() const
{
	return "EXIT";
}

void QueryExit::perform()
{
	throw ExitException();
}
