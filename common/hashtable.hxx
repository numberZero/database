#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <utility>

template <typename _Key>
bool ht_equal(_Key key1, _Key key2);

template <typename _Key>
std::size_t ht_hash(_Key key);

template <
	typename _Reference,
	typename _Key,
	typename _GetKey,
	std::size_t _Hash(_Key key) = ht_hash,
	bool _Equal(_Key key1, _Key key2) = ht_equal,
	std::size_t _Size = 0x00010000
	>
struct HashTable
{
	struct Node
	{
		_Reference content;
		Node *next;
	};

	_GetKey &getKey;
	Node **data;

	HashTable(_GetKey &key_getter) :
		getKey(key_getter),
		data(new Node *[_Size])
	{
		std::memset(data, 0, sizeof(Node *) * _Size);
	}

	~HashTable()
	{
		delete[] data;
	}

	std::size_t hash(_Key key)
	{
		return _Hash(key) % _Size;
	}

	std::size_t hash(_Reference object)
	{
		return hash(getKey(object));
	}

	void add(_Reference object)
	{
		std::size_t id = hash(object);
		Node *node = new Node;
		node->content = std::move(object);
		node->next = data[id];
		data[id] = node;
	}

	_Reference *get(_Key key)
	{
		std::size_t id = hash(key);
		Node *node = data[id];
		while(node)
		{
			if(_Equal(key, getKey(node->content)))
				return &node->content;
			node = node->next;
		}
		return nullptr;
	}

	_Reference &operator[] (_Key key)
	{
		_Reference *obj = get(key);
		if(!obj)
			throw std::out_of_range("Object not found in HastTable");
		return *obj;
	}
};

template<> bool ht_equal<char const *>(char const *key1, char const *key2);
template<> bool ht_equal<std::uint16_t>(std::uint16_t key1, std::uint16_t key2);
template<> bool ht_equal<std::uint32_t>(std::uint32_t key1, std::uint32_t key2);
template<> bool ht_equal<std::uint64_t>(std::uint64_t key1, std::uint64_t key2);

template<> std::size_t ht_hash<char const *>(char const *key);
template<> std::size_t ht_hash<std::uint16_t>(std::uint16_t key);
template<> std::size_t ht_hash<std::uint32_t>(std::uint32_t key);
template<> std::size_t ht_hash<std::uint64_t>(std::uint64_t key);
