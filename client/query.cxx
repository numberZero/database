#include <cctype>
#include <string>
#include <sstream>
#include "db/dbcommon.hxx"
#include "misc.hxx"
#include "query.hxx"

IntegerParam parseRange(std::string const& range)
{
	IntegerParam p;
	p.do_check = true;
	std::string::size_type k = range.find_first_of('-');
	if(k == std::string::npos)
	{
		p.min = p.max = std::stoi(range);
	}
	else
	{
		p.min = std::stoi(range.substr(0, k));
		p.max = std::stoi(range.substr(k + 1));
	}
	return std::move(p);
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
