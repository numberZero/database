#include <cstdlib>
#include <stdexcept>
#include "db.th.hxx"

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
std::pair<Id, _Object *> Table<_Object>::add()
{
	Id id = count;
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
Id Table<_Object>::add(_Object&& object)
{
	auto p = add();
	*p.second = std::move(object);
	return p.first;
}

template <typename _Object>
_Object *Table<_Object>::get(Id id)
{
	if(id >= count)
		return nullptr;
	return data + id;
}

template <typename _Object>
void Table<_Object>::clear()
{
	std::free(data);
	data = nullptr;
	count = 0;
	capacity = 0;
}

template <typename _Object>
_Object& Table<_Object>::operator[] (Id id)
{
	_Object *obj = get(id);
	if(!obj)
		throw std::invalid_argument("Table[] called with invalid id");
	return *obj;
}
