#pragma once
#define DEFER_TEMPLATES__SUBTABLE
#include "subtable.hxx"

template <typename _Object, Id Row::*pid>
class SubDB_String :
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

	Id add(std::string const &name);
	Id find(std::string const &name);
	Id need(std::string const &name);

	HashTable::RowIterator begin(std::string const &name);
};

typedef SubDB_String<Teacher, &Row::teacher> Teachers, SubDB_Teacher;
typedef SubDB_String<Subject, &Row::subject> Subjects, SubDB_Subject;

#ifndef USE_EXTERNAL_TEMPLATE_INSTANTIATION
#ifndef TEMPLATES__SUBTABLE
#include "subtable.cxx"
#endif
#include "subdb_string.cxx"
#endif
