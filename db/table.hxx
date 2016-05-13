#pragma once
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include "file.hxx"

class BackedTable
{
private:
	typedef std::int16_t RefCount;

	struct Entry
	{
		RefCount rc;
		char data[];
	};

	File fd;
	void *data;

	std::size_t entry_size;
	std::size_t entry_count;
	std::size_t next_insert_id;
	std::size_t capacity;

	std::unique_ptr<std::size_t[]> free_entries_buffer;
	std::size_t free_entries_start;
	std::size_t free_entries_count;
	std::size_t free_entries_capacity;

	Entry *get_entry(std::size_t index, bool no_check = false);

protected:
	std::pair<std::size_t, void *> create();
	void *get(std::size_t index);
	void grab(std::size_t index);
	void drop(std::size_t index);
	
	virtual void first_load(std::size_t index, void *object);

public:
	BackedTable(std::size_t item_size, std::string const &filename);
	~BackedTable();
};

template <typename _Object>
class Table
{
private:
	_Object ** data;
	std::size_t count;
	std::size_t capacity;

	std::size_t alloc();

public:
	Table();
	~Table();
	std::pair<std::size_t, _Object *> add();
	std::size_t add(_Object &&object);
	void remove(std::size_t id);
	_Object *get(std::size_t id);
	_Object const *get(std::size_t id) const;
	std::size_t size() const;

	_Object &operator[] (std::size_t id);
	_Object const &operator[] (std::size_t id) const;
};

template <typename _Object>
Table<_Object>::Table() :
	data(nullptr),
	count(0),
	capacity(0)
{
}

template <typename _Object>
Table<_Object>::~Table()
{
	std::free(data);
}

template <typename _Object>
std::size_t Table<_Object>::alloc()
{
	if(count >= capacity)
	{
		if(capacity < 0x00000100)
			capacity += 0x00000010;
		else if(capacity < 0x00010000)
			capacity *= 2;
		else
			capacity += 0x00010000;
		_Object **nd = reinterpret_cast<_Object **>(std::realloc(data, sizeof(_Object *) * capacity));
		if(!nd)
			throw std::bad_alloc();
		data = nd;
	}
	return count++;
}

template <typename _Object>
std::pair<std::size_t, _Object *> Table<_Object>::add()
{
	std::size_t id = alloc();
	data[id] = new _Object();
	return { id, data[id] };
}

template <typename _Object>
std::size_t Table<_Object>::add(_Object &&object)
{
	std::size_t id = alloc();
	data[id] = new _Object(object);
	return id;
}

template <typename _Object>
void Table<_Object>::remove(std::size_t id)
{
	if(id >= count)
		throw std::out_of_range("Table::remove called with invalid id");
	if(!data[id])
		throw std::invalid_argument("Table::remove called with invalid id");
	delete data[id];
	data[id] = nullptr;
}

template <typename _Object>
_Object *Table<_Object>::get(std::size_t id)
{
	if(id >= count)
		return nullptr;
	return data[id];
}

template <typename _Object>
_Object const *Table<_Object>::get(std::size_t id) const
{
	if(id >= count)
		return nullptr;
	return data[id];
}

template <typename _Object>
std::size_t Table<_Object>::size() const
{
	return count;
}

template <typename _Object>
_Object &Table<_Object>::operator[] (std::size_t id)
{
	_Object *obj = get(id);
	if(!obj)
		throw std::invalid_argument("Table[] called with invalid id");
	return *obj;
}

template <typename _Object>
_Object const &Table<_Object>::operator[] (std::size_t id) const
{
	_Object const *obj = get(id);
	if(!obj)
		throw std::invalid_argument("const Table[] called with invalid id");
	return *obj;
}
