#pragma once
#include <cstdint>
#include <cstring>
#include <stdexcept>

template <
	typename _Object,
	typename _Key,
	_Key _GetKey(_Object const& object),
	std::size_t _Hash(_Key const& key),
	std::size_t _Size = 0x00010000
	>
struct HashTable
{
	struct Node
	{
		_Object *content;
		Node *next;
	};

	Node *data;

	HashTable()
	{
		data = new Node[_Size];
		std::memset(data, 0, sizeof(Node) * _Size);
	}

	~HashTable()
	{
		delete[] data;
	}

	std::size_t hash(_Key const& key)
	{
		return _Hash(key) % _Size;
	}

	std::size_t hash(_Object const& object)
	{
		return hash(_GetKey(object));
	}

	void add(_Object *object)
	{
		std::size_t id = hash(object);
		Node *node = new Node;
		node->content = object;
		node->next = data[id];
		data[id] = node;
	}

	_Object *get(_Key const& key)
	{
		std::size_t id = hash(key);
		Node *node = &data[id];
		while(node)
		{
			if(key == _GetKey(node->content))
				return node->content;
			node = node->next;
		}
		return nullptr;
	}

	_Object& operator[] (_Key const& key)
	{
		_Object *obj = get(key);
		if(!obj)
			throw std::out_of_range("Object not found in HastTable");
		return *obj;
	}
};
