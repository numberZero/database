#pragma once
#include "data.hxx"
#include "table.hxx"
#include "hashtable.hxx"

template <typename _Object, Id Row::*pid>
class Subtable :
	public Table<_Object>,
	public HashTable
{
protected:
	typedef Table<_Object> DataTable;

	Subtable(File &&file, defer_load_t);
	DataTable &table();
	DataTable const &table() const;

	using DataTable::load;
	using HashTable::get;

	void first_load(Id index, _Object &item) override;
	void change_id(Id from, Id to) override;
	virtual void on_add(Id index, _Object &item);

	void remove(Id row);
	void add_row(Id row, Id to);
	void remove_row(Id row, Id from);

	RowIterator end();

public:
	using DataTable::get;
};
