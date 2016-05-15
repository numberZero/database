#pragma once
#include "data.hxx"
#include "table.hxx"
#include "hashtable.hxx"

template <typename _Object>
class Subtable :
	protected Table<_Object>,
	protected HashTable
{
protected:
	typedef Table<_Object> DataTable;

	Subtable(std::string const &file);
	Subtable(File &&file);
	DataTable &table();
	DataTable const &table() const;

	void add_row(Id row, Id to);
	void remove_row(Id row, Id from);

	RowIterator end();
};
