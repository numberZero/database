#pragma once
#include <string>

struct Param
{
	bool do_check = false;
	bool do_return = false;
	bool is_valid = true;
};

struct StringParam: Param
{
	std::string value;

	bool is_key() const;
	void refine(StringParam const &b);
	bool check(char const *data) const;
	void set(std::string const &_value);
};

struct IntegerParam: Param
{
	long min = 0;
	long max = 0;

	bool is_key() const;
	void refine(IntegerParam const &b);
	bool check(long data) const;
	void set(long _value);
	void set(long _min, long _max);
	void set(std::string const &_value);
};

struct SelectionParams
{
	StringParam teacher;
	StringParam subject;
	IntegerParam room;
	IntegerParam group;
	IntegerParam day;
	IntegerParam lesson;

	void refine(SelectionParams const &b);
	void clearReturn();
	bool isValid() const;
};

struct RowData
{
	std::string teacher;
	std::string subject;
	unsigned room;
	unsigned group;
	unsigned day;
	unsigned lesson;
};
