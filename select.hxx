#pragma once
#include <string>
#include "data.hxx"

class Database;

class RowReference
{
	Database *db;
	Row *row;

public:
	char *getTeacher() const;
	char *getSubject() const;
	int getRoom() const;
	int getGroup() const;
	bool isMetaGroup() const;
	int getDay() const;
	int getLesson() const;
};

struct Param
{
	bool select;
	bool result;
};

struct StringParam: Param
{
	std::string value;
};

struct IntegerParam: Param
{
	long min;
	long max;
};

struct BooleanParam: Param
{
	bool value;
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
};

class Selection
{
	Database *db;
	SelectionParams p;

public:
	Selection(SelectionParams const& params);

	bool isValid(); // do we have more rows
	RowReference getRow(); // returns current row
	void next(); // shifts to the next row
};
