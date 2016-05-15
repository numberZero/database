#pragma once
#pragma once
#include <iostream>
#include "data.hxx"
#include "table.hxx"
#include "hashtable.hxx"

template <typename _Object, typename _Key, typename... Params>
class SubDB
{
public:
	typedef _Object Object;
	typedef _Key Key;
	typedef SubDB<Object, Key, Params...> Self;

	typedef Table<Object> Data;
	typedef HashTable<Id, Key, Self> Index;

private:
	friend Index;
	Key operator() (Id id);

protected:
	Data data;
	Index index;

	SubDB(std::string const &file);
	SubDB(File &&file);

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
