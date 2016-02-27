#pragma once
#include <cstdio>
#include <utility>
#include "data.hxx"

template <typename _Object>
struct Table
{
	_Object *data;
	std::size_t count;
	std::size_t capacity;

	Table();
	~Table();
	Id add(_Object&& object);
	std::pair<Id, _Object *> add();
	_Object *get(Id id);
	void clear();

	_Object& operator[] (Id id);
};
