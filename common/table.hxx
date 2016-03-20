#pragma once
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <utility>

template <typename _Object>
struct Table
{
	_Object *data;
	std::size_t count;
	std::size_t capacity;

	Table();
	~Table();
	std::size_t add(_Object &&object);
	std::pair<std::size_t, _Object *> add();
	_Object *get(std::size_t id);

	_Object &operator[] (std::size_t id);
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
std::pair<std::size_t, _Object *> Table<_Object>::add()
{
	std::size_t id = count;
	if(count >= capacity)
	{
		if(capacity < 0x00000100)
			capacity += 0x00000010;
		else if(capacity < 0x00010000)
			capacity *= 2;
		else
			capacity += 0x00010000;
		_Object *nd = reinterpret_cast<_Object *>(std::realloc(data, sizeof(_Object) * capacity));
		if(!nd)
			throw std::bad_alloc();
		data = nd;
	}
	++count;
	return { id, data + id };
}

template <typename _Object>
std::size_t Table<_Object>::add(_Object &&object)
{
	auto p = add();
	*p.second = std::move(object);
	return p.first;
}

template <typename _Object>
_Object *Table<_Object>::get(std::size_t id)
{
	if(id >= count)
		return nullptr;
	return data + id;
}

template <typename _Object>
_Object &Table<_Object>::operator[] (std::size_t id)
{
	_Object *obj = get(id);
	if(!obj)
		throw std::invalid_argument("Table[] called with invalid id");
	return *obj;
}
