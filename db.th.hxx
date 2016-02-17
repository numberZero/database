#pragma once
#include <cstdio>
#include <stdexcept>
#include "data.hxx"

class Database;

template <typename _Object>
class Table;

template <typename _Object>
struct ObjectReference
{
	typedef _Object Object;
	typedef ObjectReference<Object> Self;
	friend Table<Object>;

	ObjectReference() = default;
	ObjectReference(ObjectReference const&) = delete;
	ObjectReference(ObjectReference&&) = delete;

private:
	Object *object;
	union
	{
		off_t offset;
		Self *next;
	};

	bool isEmpty() const;
	Self *getNext();
	off_t getOffset();

public:
	Object *get() const;
	Object &operator* ();
	Object *operator-> ();
};

template <typename _Object>
class Table
{
public:
	typedef _Object Object;
	typedef ObjectReference<Object> Reference;

private:
	Reference *data;
	Reference *empty;
	std::size_t count;
	std::size_t capacity;
	std::size_t last_insert_id;

public:
	Table();
	~Table();
	Id add(Object *object);
	void remove(Id id);
	Reference &get(Id id);
};

template <typename _Object>
class IterableTable:
	public Table<_Object>
{
public:
	typedef _Object Object;
	typedef Table<Object> Parent;
	typedef IterableTable<Object> Self;
	using typename Parent::Reference;

	class Iterator
	{
		friend Self;
		Reference *ref;
		Iterator(Reference *reference);
	public:
		Iterator() = delete;
		Iterator(Iterator const &) = default;
		Iterator(Iterator &&) = default;
		bool operator== (Iterator const &b);
		bool operator!= (Iterator const &b);
		Iterator &operator++ ();
		Object &operator* ();
		Object *operator-> ();
	};

	Iterator begin();
	Iterator end();
};
