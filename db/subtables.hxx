#pragma once
#pragma once
#include <iostream>
#include "dbcommon.hxx"
#include "table.hxx"
#include "hashtable.hxx"
#include "dbhelper.hxx"

#define DEFINE_SUBDB_CLASS(NAME, NAME_LC, KEY, PARAMS) \
	class SubDB_##NAME \
	{ \
	private: \
		friend HashTable<Id, KEY, SubDB_##NAME>; \
		KEY operator() (Id id) \
		{ \
			return getKey(NAME_LC##s[id].data); \
		} \
\
	protected: \
		Table<Container<NAME>> NAME_LC##s; \
		HashTable<Id, KEY, SubDB_##NAME> index_##NAME_LC; \
		void readTableRowData_##NAME_LC##s(std::istream &file); \
 \
		SubDB_##NAME() : \
			index_##NAME_LC(*this) \
		{ \
		} \
 \
	public: \
		Id add##NAME PARAMS ; \
		Id find##NAME PARAMS ; \
	} \

DEFINE_SUBDB_CLASS(Teacher, teacher, char const *, (std::string const &name));
DEFINE_SUBDB_CLASS(Subject, subject, char const *, (std::string const &name));
DEFINE_SUBDB_CLASS(Room, room, std::uint_fast32_t, (unsigned number));
DEFINE_SUBDB_CLASS(Group, group, std::uint_fast32_t, (unsigned number));
DEFINE_SUBDB_CLASS(Time, time, std::uint_fast32_t, (unsigned day, unsigned lesson));
