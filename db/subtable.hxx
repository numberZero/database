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

	Subtable(File &&file, defer_load_t);
	DataTable &table();
	DataTable const &table() const;

	using DataTable::load;
	using DataTable::get;
	using HashTable::get;

	void first_load(Id index, _Object &item) override;
	virtual void on_add(Id index, _Object &item);

	void remove(Id row);
	void add_row(Id row, Id to);
	void remove_row(Id row, Id from);

	RowIterator end();
};
