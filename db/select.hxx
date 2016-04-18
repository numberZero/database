#pragma once
#include <memory>
#include <string>
#include "data.hxx"
#include "dbhelper.hxx"
#include "srxw.hxx"
#include "struct.hxx"

class PreSelection
{
public:
	virtual ~PreSelection() = default;
	virtual bool isValid() = 0;
	virtual Row const *getRow() = 0;
	virtual void next() = 0;
};

class PreSelection_Real:
	public PreSelection
{
protected:
	Rows const &rows;
	PreSelection_Real(Rows const &db);

public:
	Row const *getRow() override;
	virtual Id getRowId() = 0;
};

class PreSelection_Full:
	public PreSelection_Real
{
private:
	std::size_t index;

public:
	PreSelection_Full(Rows const &table);
	bool isValid() override;
	void next() override;
	Id getRowId() override;
};

class PreSelection_SimpleKey:
	public PreSelection_Real
{
private:
	Id id;
	RowRefList *rows;
	RowRefList::Node *node;
	std::size_t index;

public:
	template <typename _Object, typename _Key, typename _Table, typename _HashTable>
	PreSelection_SimpleKey(Rows const &db, _HashTable &ht, _Table &table, _Key key) :
		PreSelection_Real(db)
	{
		id = ht[key];
		rows = table[id].rows;
		node = rows->head;
		index = 0;
	}

	bool isValid() override;
	void next() override;
	Id getRowId() override;
};

class PreSelection_Bitset:
	public PreSelection
{
private:
	Bitset const &bs;
	IntegerParam rx;
	IntegerParam ry;
	Id Row::*px;
	Id Row::*py;
	Row values;

public:
	PreSelection_Bitset(Bitset const &bitset, Id Row::*param_x, IntegerParam range_x, Id Row::*param_y, IntegerParam range_y);

	bool isValid() override;
	Row const *getRow() override;
	void next() override;
};

class Selection
{
	Database *db;
	SelectionParams p;
	std::unique_ptr<PreSelection> s;
	std::unique_ptr<SRXW_ReadLockGuard> gurad;

	void reset(Selection &b);
	bool reach(); // finds first row that fits the query

public:
	Selection();
	Selection(Database &database, SelectionParams const &params, std::unique_ptr<PreSelection> &&preselect);
	Selection(Selection const &b) = delete;
	Selection(Selection &&b);

	Selection &operator= (Selection const &b) = delete;
	Selection &operator= (Selection &&b);

	bool isValid(); // do we have more rows
	RowReference getRow(); // returns current row
	void next(); // shifts to the next row
};
