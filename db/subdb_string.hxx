#pragma once
#include "subtable.hxx"

template <typename _Object>
class SubDB_String :
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

	Id add(std::string const &name);
	Id find(std::string const &name);
	Id need(std::string const &name);

	HashTable::RowIterator begin(std::string const &name);
};

typedef SubDB_String<Teacher> Teachers, SubDB_Teacher;
typedef SubDB_String<Subject> Subjects, SubDB_Subject;
