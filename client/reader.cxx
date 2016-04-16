#include <iostream>
#include <map>
#include <set>
#include "misc.hxx"
#include "reader.hxx"

SelectionParams QueryReader::readSelectParams()
{
	SelectionParams result;
	auto params = readParams2();
	for(std::pair<std::string, std::string> const &param: params)
	{
#define SELPARAM(name) \
		if(param.first == #name) \
			result.name.set(param.second)
		SELPARAM(teacher); else
		SELPARAM(subject); else
		SELPARAM(room); else
		SELPARAM(group); else
		SELPARAM(day); else
		SELPARAM(lesson); else
		throw InvalidQueryError("Unknown SELECT parameter");
	}
	return std::move(result);
}

SelectionParams QueryReader::readPrintParams()
{
	SelectionParams result;
	auto params = readParams1();
	for(std::string const &param: params)
	{
#define PRNPARAM(name) \
		if(param == #name) \
			result.name.do_return = true
		PRNPARAM(teacher); else
		PRNPARAM(subject); else
		PRNPARAM(room); else
		PRNPARAM(group); else
		PRNPARAM(day); else
		PRNPARAM(lesson); else
		throw InvalidQueryError("Unknown PRINT parameter");
	}
	return std::move(result);
}

RowData QueryReader::readInsertParams()
{
	RowData result;
	auto params = readParams2();
	for(std::pair<std::string, std::string> const &param: params)
	{
#define INSPARAMS(name) \
		if(param.first == #name) \
			result.name = param.second
#define INSPARAMI(name) \
		if(param.first == #name) \
			result.name = std::stoi(param.second)
		INSPARAMS(teacher); else
		INSPARAMS(subject); else
		INSPARAMI(room); else
		INSPARAMI(group); else
		INSPARAMI(day); else
		INSPARAMI(lesson); else
		throw InvalidQueryError("Unknown INSERT parameter");
	}
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

PQuery QueryReader::callReadFunction(std::string const &type)
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
		throw ExitException(); // treat EOF before query beginning as normal exit
	}
	std::string type = readIdent();
	readSpace();
	locase_it(type);
	PQuery q(callReadFunction(type));
	ensureEnd();
	return std::move(q);
}

QueryReader::QueryReader(std::istream &stream) :
	BaseReader(stream)
{
}

PQuery readQuery(std::istream &in)
{
	QueryReader rd(in);
	return rd.readQuery();
}
