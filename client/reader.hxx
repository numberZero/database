#pragma once
#include "tokenizer.hxx"
#include "db/dbhelper.hxx"
#include "db/select.hxx"

class QueryReader:
	public BaseReader
{
protected:
	SelectionParams readSelectParams();
	SelectionParams readPrintParams();
	RowData readInsertParams();

private:
	void read_select();
	void read_reselect();
	void read_insert();
	void read_remove();
	void read_print();
	void read_help();
	void read_exit();

	void callReadFunction(std::string const& type);

public:
	QueryReader(std::istream& stream);
	void readQuery();
};

void readQuery(std::istream& in);
