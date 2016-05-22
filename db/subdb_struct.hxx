#pragma once
#include "subtable.hxx"

template <typename _Object, Id Row::*pid, typename... Params>
class SubDB_Struct :
	public Subtable<_Object, pid>
{
private:
	typedef HashTable::PKey PKey;
	typedef HashTable::Hash Hash;

	PKey key_of(Id id) override;
	Hash hash(PKey key) override;
	bool equal(PKey key1, PKey key2) override;

protected:
	using Subtable<_Object, pid>::Subtable;
	using Subtable<_Object, pid>::table;

	Id add(Params... params);
	Id find(Params... params);
	Id need(Params... params);

	HashTable::RowIterator begin(Params... params);
};

typedef SubDB_Struct<Room, &Row::room, std::uint32_t> Rooms, SubDB_Room;
typedef SubDB_Struct<Group, &Row::group, std::uint32_t> Groups, SubDB_Group;
typedef SubDB_Struct<Time, &Row::time, std::uint16_t, std::uint16_t> Times, SubDB_Time;
typedef SubDB_Struct<Row, nullptr, Id, Id, Id, Id, Id> Rows, SubDB_Row;
