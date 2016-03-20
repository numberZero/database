#include <cassert>
#include <cctype>
#include <string>
#include <sstream>
#include "binary/network.hxx"
#include "db/dbcommon.hxx"
#include "bio.hxx"
#include "misc.hxx"
#include "query.hxx"

QueryMachineState global_state {
	-1,
	std::cin,
	std::cout,
	{}
};

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

}

char const *QueryRemove::name() const
{
	return "REMOVE";
}

void QueryRemove::perform()
{

}

char const *QueryPrint::name() const
{
	return "PRINT";
}

void QueryPrint::perform()
{
	global_state.params.clearReturn();
	global_state.params.refine(params);
	std::size_t head_size = NetworkType<QueryType>::StaticSize;
	std::size_t body_size = NetworkType<SelectionParams>::dynamic_size(global_state.params);
	std::size_t packet_size = head_size + body_size;
	std::unique_ptr<char[]> buffer(new char[packet_size]);
	char *packet = buffer.get();
	char *head = packet;
	char *body = packet + head_size;
	NetworkType<QueryType>::static_serialize(head, QueryType::Select);
	assert(body_size == NetworkType<SelectionParams>::dynamic_serialize(body, body_size, global_state.params));
	writePacket(global_state.connection, packet, packet_size);
	for(std::size_t id = 1;; ++id)
	{
		buffer.reset(); // free unneeded memory
		readPacket(global_state.connection, packet, packet_size);
		buffer.reset(packet); // make sure it will be freed when necessary
		if(!packet_size)
		{ // empty packet indicates end of data
			global_state.cout << "*** END ***" << std::endl;
			break;
		}
		RowData row;
		std::size_t bytes = NetworkType<RowData>::dynamic_parse(packet, packet_size, row);
		if(bytes != packet_size)
			throw ProtocolError("Invalid row packet");
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
}

char const *QueryExit::name() const
{
	return "EXIT";
}

void QueryExit::perform()
{
	throw ExitException();
}
