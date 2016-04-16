#include <iostream>
#include <map>
#include <set>
#include "misc.hxx"
#include "reader.hxx"

SelectionParams QueryReader::readSelectParams()
{
	SelectionParams result;
	auto params = readParams2();
	decltype(params)::iterator iter;
#define SELPARAM(name) \
		iter = params.find(#name); \
		if(iter != params.end()) { \
			result.name.set(iter->second); \
			params.erase(iter); \
		}
	SELPARAM(teacher);
	SELPARAM(subject);
	SELPARAM(room);
	SELPARAM(group);
	SELPARAM(day);
	SELPARAM(lesson);
	if(params.size())
		throw InvalidQueryError("Unknown SELECT parameter: " + params.begin()->first);
	return std::move(result);
}

SelectionParams QueryReader::readPrintParams()
{
	SelectionParams result;
	auto params = readParams1();
	decltype(params)::iterator iter;
	bool printall = !params.size();
#define PRNPARAM(name) \
		if(printall) \
			result.name.do_return = true; \
		else { \
			iter = params.find(#name); \
			if(iter != params.end()) { \
				result.name.do_return = true; \
				params.erase(iter); \
			} \
		}
	PRNPARAM(teacher);
	PRNPARAM(subject);
	PRNPARAM(room);
	PRNPARAM(group);
	PRNPARAM(day);
	PRNPARAM(lesson);
	if(params.size())
		throw InvalidQueryError("Unknown PRINT parameter: " + *params.begin());
	return std::move(result);
}

RowData QueryReader::readInsertParams()
{
	RowData result;
	auto params = readParams2();
	decltype(params)::iterator iter;
#define INSPARAM(name,conv) \
		iter = params.find(#name); \
		if(iter == params.end()) \
			throw InvalidQueryError("Missing INSERT parameter: " #name); \
		result.name = conv(iter->second); \
		params.erase(iter)
	INSPARAM(teacher, );
	INSPARAM(subject, );
	INSPARAM(room, std::stoi);
	INSPARAM(group, std::stoi);
	INSPARAM(day, std::stoi);
	INSPARAM(lesson, std::stoi);
	if(params.size())
		throw InvalidQueryError("Unknown INSERT parameter: " + params.begin()->first);
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
