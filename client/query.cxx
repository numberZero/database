#include <cctype>
#include <string>
#include <sstream>
#include "db/dbcommon.hxx"
#include "misc.hxx"
#include "query.hxx"
#include "binary/network.hxx"

QueryMachineState global_state {
	0,
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
