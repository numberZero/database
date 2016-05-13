#pragma once
#pragma once
#include <iostream>
#include "dbcommon.hxx"
#include "table.hxx"
#include "hashtable.hxx"
#include "dbhelper.hxx"

template <typename _Object, typename _Key, typename... Params>
class SubDB
{
public:
	typedef _Object Object;
	typedef _Key Key;
	typedef SubDB<Object, Key, Params...> Self;

	typedef Table<Container<Object>> Data;
	typedef HashTable<Id, Key, Self> Index;

private:
	friend Index;
	Key operator() (Id id)
	{
		return getKey(data.get(id).data);
	}

protected:
	Data data;
	Index index;
	void readTableRowData(std::istream &file);

	SubDB() :
		data(""),
		index(*this)
	{
	}

public:
	Id add(Params... params);
	Id find(Params... params);
	Id need(Params... params);
};

typedef SubDB<Teacher, char const *, std::string const &> Teachers, SubDB_Teacher;
typedef SubDB<Subject, char const *, std::string const &> Subjects, SubDB_Subject;
typedef SubDB<Room, std::uint_fast32_t, unsigned> Rooms, SubDB_Room;
typedef SubDB<Group, std::uint_fast32_t, unsigned> Groups, SubDB_Group;
typedef SubDB<Time, std::uint_fast32_t, unsigned, unsigned> Times, SubDB_Time;
