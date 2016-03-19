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

PQuery QueryReader::read_select()
{
	std::unique_ptr<QuerySelect> q(new QuerySelect());
	q->params = readSelectParams();
	q->re = false;
	return std::move(q);
}

PQuery QueryReader::read_reselect()
{
	std::unique_ptr<QuerySelect> q(new QuerySelect());
	q->params = readSelectParams();
	q->re = true;
	return std::move(q);
}

PQuery QueryReader::read_insert()
{
	std::unique_ptr<QueryInsert> q(new QueryInsert());
	q->row = readInsertParams();
	return std::move(q);
}

PQuery QueryReader::read_remove()
{
	std::unique_ptr<QueryRemove> q(new QueryRemove());
	q->params = readSelectParams();
	return std::move(q);
}

PQuery QueryReader::read_print()
{
	std::unique_ptr<QueryPrint> q(new QueryPrint());
	q->params = readPrintParams();
	return std::move(q);
}

PQuery QueryReader::read_help()
{
	return PQuery(new QueryHelp());
}

PQuery QueryReader::read_exit()
{
	return PQuery(new QueryExit());
}

PQuery QueryReader::callReadFunction(std::string const& type)
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
	throw UnknownQueryTypeError("Unknown query type: " + type);
}

PQuery QueryReader::readQuery()
{
	try
	{
		readChar();
		readSpace();
	}
	catch(EofError)
	{
		throw ExitException();
	}
	std::string type = readIdent();
	readSpace();
	locase_it(type);
	PQuery q(callReadFunction(type));
	readEnd();
	return std::move(q);
}

QueryReader::QueryReader(std::istream& stream) :
	BaseReader(stream)
{
}

PQuery readQuery(std::istream& in)
{
	QueryReader rd(in);
	return rd.readQuery();
}
