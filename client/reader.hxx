#pragma once
#include "tokenizer.hxx"
#include "query.hxx"
#include "db/dbhelper.hxx"
#include "db/select.hxx"

NEW_ERROR_CLASS(UnknownQueryTypeError, InvalidQueryError, );

class QueryReader:
	public BaseReader
{
protected:
	SelectionParams readSelectParams();
	SelectionParams readPrintParams();
	RowData readInsertParams();

private:
	PQuery read_select();
	PQuery read_reselect();
	PQuery read_insert();
	PQuery read_remove();
	PQuery read_print();
	PQuery read_help();
	PQuery read_exit();

	PQuery callReadFunction(std::string const& type);

public:
	QueryReader(std::istream& stream);
	PQuery readQuery();
};

PQuery readQuery(std::istream& in);
