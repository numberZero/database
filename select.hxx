#pragma once
#include <string>
#include "data.hxx"
#include "dbhelper.hxx"

class Database;

class RowReference
{
	Database *db;
	Row *row;

public:
	RowReference(Database *database, 	Row *prow);

	char const *getTeacher() const;
	char const *getSubject() const;
	int getRoom() const;
	int getGroup() const;
	bool isMetaGroup() const;
	int getDay() const;
	int getLesson() const;
};

struct Param
{
	bool do_check = false;
	bool do_return = false;
	bool is_valid = true;
};

struct StringParam: Param
{
	std::string value;
	void refine(StringParam const& b);
	bool check(char const *data);
};

struct IntegerParam: Param
{
	long min;
	long max;
	void refine(IntegerParam const& b);
	bool check(long data);
};

struct BooleanParam: Param
{
	bool value;
	void refine(BooleanParam const& b);
	bool check(bool data);
};

struct SelectionParams
{
	StringParam teacher;
	StringParam subject;
	IntegerParam room;
	IntegerParam group;
	BooleanParam meta;
	IntegerParam day;
	IntegerParam lesson;

	void refine(SelectionParams const& b);
	bool check(RowReference row);
};

class PreSelection
{
public:
	virtual bool isValid() = 0;
	virtual Row *getRow() = 0;
	virtual void next() = 0;
};

class PreSelection_SimpleKey:
	public PreSelection
{
private:
	Id id;
	RowRefList *rows;
	RowRefList::Node *node;
	std::size_t index;

public:
	template <typename _Object, typename _Key, typename _Table, typename _HashTable>
	PreSelection_SimpleKey(_HashTable& ht, _Table& table, _Key key)
	{
		id = ht[key];
		rows = table[id].rows;
		node = rows->head;
		index = 0;
	}

	bool isValid() override;
	Row *getRow() override;
	void next() override;
};

class Selection
{
	Database *db;
	SelectionParams p;
	PreSelection *s;

public:
	Selection(Database *database, SelectionParams const& params);

	void perform(); // initializes the selection
	bool isValid(); // do we have more rows
	RowReference getRow(); // returns current row
	void next(); // shifts to the next row
};
