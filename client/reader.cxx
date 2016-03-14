#include <iostream>
#include <map>
#include <set>
#include "misc.hxx"
#include "reader.hxx"

SelectionParams QueryReader::readSelectParams()
{
	SelectionParams result;
	auto params = readParams2();
	for(std::pair<std::string, std::string> const& param: params)
		std::cout << param.first << ": " << param.second << std::endl;
	return std::move(result);
}

SelectionParams QueryReader::readPrintParams()
{
	SelectionParams result;
	auto params = readParams1();
	for(std::string const& param: params)
		std::cout << param << std::endl;
	return std::move(result);
}

RowData QueryReader::readInsertParams()
{
	RowData result;
	auto params = readParams2();
	for(std::pair<std::string, std::string> const& param: params)
		std::cout << param.first << ": " << param.second << std::endl;
	return std::move(result);
}

void QueryReader::read_select()
{
	std::cout << "SELECT" << std::endl;
	readSelectParams();
}

void QueryReader::read_reselect()
{
	std::cout << "SELECT AGAIN" << std::endl;
	readSelectParams();
}

void QueryReader::read_insert()
{
	std::cout << "INSERT" << std::endl;
	readInsertParams();
}

void QueryReader::read_remove()
{
	std::cout << "REMOVE" << std::endl;
	readSelectParams();
}

void QueryReader::read_print()
{
	std::cout << "PRINT" << std::endl;
	readPrintParams();
}

void QueryReader::read_help()
{
	std::cout << "HELP" << std::endl;
}

void QueryReader::read_exit()
{
	std::cout << "EXIT" << std::endl;
}

void QueryReader::callReadFunction(std::string const& type)
{
#define QUERY_TYPE(qtype) \
	if(type == #qtype) \
		return read_##qtype()
	QUERY_TYPE(select);
	QUERY_TYPE(reselect);
	QUERY_TYPE(insert);
	QUERY_TYPE(remove);
	QUERY_TYPE(print);
	QUERY_TYPE(help);
	QUERY_TYPE(exit);
#undef QUERY_TYPE
	throw error("Invalid query");
}

void QueryReader::readQuery()
{
	readChar();
	readSpace();
	std::string type = readIdent();
	readSpace();
	locase_it(type);
	callReadFunction(type);
	readEnd();
	std::cout << "END" << std::endl;
	std::cout << std::endl;
}

QueryReader::QueryReader(std::istream& stream) :
	BaseReader(stream)
{
}

void readQuery(std::istream& in)
{
	QueryReader rd(in);
	rd.readQuery();
}
