#pragma once
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <utility>

typedef std::size_t Size, Index;

template <typename _Object>
class Table
{
private:
	_Object **data;
	Size count;
	Size capacity;

	Index alloc();

public:
	Table();
	~Table();
	std::pair<Index, _Object *> add();
	Index add(_Object &&object);
	void remove(Index id);
	_Object *get(Index id);
	_Object const *get(Index id) const;
	Size size() const;

	_Object &operator[] (Index id);
	_Object const &operator[] (Index id) const;
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
Index Table<_Object>::alloc()
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
std::pair<Index, _Object *> Table<_Object>::add()
{
	Index id = alloc();
	data[id] = new _Object();
	return { id, data[id] };
}

template <typename _Object>
Index Table<_Object>::add(_Object &&object)
{
	Index id = alloc();
	data[id] = new _Object(object);
	return id;
}

template <typename _Object>
void Table<_Object>::remove(Index id)
{
	if(id >= count)
		throw std::out_of_range("Table::remove called with invalid id");
	if(!data[id])
		throw std::invalid_argument("Table::remove called with invalid id");
	delete data[id];
	data[id] = nullptr;
}

template <typename _Object>
_Object *Table<_Object>::get(Index id)
{
	if(id >= count)
		return nullptr;
	return data[id];
}

template <typename _Object>
_Object const *Table<_Object>::get(Index id) const
{
	if(id >= count)
		return nullptr;
	return data[id];
}

template <typename _Object>
Size Table<_Object>::size() const
{
	return count;
}

template <typename _Object>
_Object &Table<_Object>::operator[] (Index id)
{
	_Object *obj = get(id);
	if(!obj)
		throw std::invalid_argument("Table[] called with invalid id");
	return *obj;
}

template <typename _Object>
_Object const &Table<_Object>::operator[] (Index id) const
{
	_Object const *obj = get(id);
	if(!obj)
		throw std::invalid_argument("const Table[] called with invalid id");
	return *obj;
}
