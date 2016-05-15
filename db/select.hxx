#pragma once
#include <memory>
#include <string>
#include "data.hxx"
#include "dbhelper.hxx"
#include "hashtable.hxx"
#include "srxw.hxx"
#include "struct.hxx"

class PreSelection
{
protected:
	Rows const &rows;

	PreSelection(Rows const &db);

public:
	virtual ~PreSelection() = default;

	Row const *getRow();

	virtual bool isValid() = 0;
	virtual Id getRowId() = 0;
	virtual void next() = 0;
};

class PreSelection_Full:
	public PreSelection
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
	public PreSelection
{
private:
	HashTable::RowIterator iterator;

public:
	PreSelection_SimpleKey(Rows const &db, HashTable::RowIterator iter);
	bool isValid() override;
	void next() override;
	Id getRowId() override;
};

class Selection
{
	Database *db;
	SelectionParams p;
	std::unique_ptr<PreSelection> s;
	std::unique_ptr<SRXW_ReadLockGuard> guard;

	void reset(Selection &b);
	bool reach(); // finds first row that fits the query

public:
	Selection();
	Selection(Database &database, SelectionParams const &params, std::unique_ptr<PreSelection> &&preselect, std::unique_ptr<SRXW_ReadLockGuard> &&lguard);
	Selection(Selection const &b) = delete;
	Selection(Selection &&b);

	Selection &operator= (Selection const &b) = delete;
	Selection &operator= (Selection &&b);

	bool isValid(); // do we have more rows
	RowReference getRow(); // returns current row
	void next(); // shifts to the next row
};
