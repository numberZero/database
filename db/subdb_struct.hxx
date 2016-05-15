#pragma once
#include "subtable.hxx"

template <typename _Object, typename... Params>
class SubDB_Struct :
	protected Subtable<_Object>
{
private:
	typedef HashTable::PKey PKey;
	typedef HashTable::Hash Hash;

	PKey key_of(Id id) override;
	Hash hash(PKey key) override;
	bool equal(PKey key1, PKey key2) override;

protected:
	using Subtable<_Object>::Subtable;
	using Subtable<_Object>::table;

	Id add(Params... params);
	Id find(Params... params);
	Id need(Params... params);
};

typedef SubDB_Struct<Room, unsigned> Rooms, SubDB_Room;
typedef SubDB_Struct<Group, unsigned> Groups, SubDB_Group;
typedef SubDB_Struct<Time, unsigned, unsigned> Times, SubDB_Time;
