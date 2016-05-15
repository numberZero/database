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

template <typename _Reference>
struct HTNode
{
	_Reference content;
	HTNode *next;
};

template <typename _Reference>
struct HTChunk
{
	typedef HTNode<_Reference> Node;
	Node *data;
	HTChunk *next;

	HTChunk(std::size_t size);
	~HTChunk();
};

template <
	typename _Reference,
	typename _Key,
	typename _GetKey,
	std::size_t _hash(_Key key) = ht_hash,
	bool _equal(_Key key1, _Key key2) = ht_equal
	>
class HashTable
{
private:
	typedef HTNode<_Reference> Node;
	typedef HTChunk<_Reference> Chunk;

	_GetKey &get_key;
	std::size_t table_size;
	std::size_t chunk_size;
	Node *table;
	Chunk *chunk;
	Node *empty;

	std::size_t hash(_Key key);
	std::size_t hash(_Reference object);

public:
	HashTable(_GetKey &_get_key, std::size_t _table_size, std::size_t _chunk_size);
	HashTable(_GetKey &_get_key, std::size_t _table_size = 0x00010000);
	~HashTable();
	void add(_Reference object);
	_Reference const *get(_Key key);
	_Reference const &operator[] (_Key key);
};

template<> bool ht_equal<char const *>(char const *key1, char const *key2);
template<> bool ht_equal<std::uint16_t>(std::uint16_t key1, std::uint16_t key2);
template<> bool ht_equal<std::uint32_t>(std::uint32_t key1, std::uint32_t key2);
template<> bool ht_equal<std::uint64_t>(std::uint64_t key1, std::uint64_t key2);

template<> std::size_t ht_hash<char const *>(char const *key);
template<> std::size_t ht_hash<std::uint16_t>(std::uint16_t key);
template<> std::size_t ht_hash<std::uint32_t>(std::uint32_t key);
template<> std::size_t ht_hash<std::uint64_t>(std::uint64_t key);

#define template_HashTable template < \
	typename _Reference, \
	typename _Key, \
	typename _GetKey, \
	std::size_t _hash(_Key key), \
	bool _equal(_Key key1, _Key key2) \
	>
#define HashTable_templated HashTable<_Reference, _Key, _GetKey, _hash, _equal>

template <typename _Reference>
HTChunk<_Reference>::HTChunk(std::size_t size)
{
	next = nullptr;
	data = new HTNode<_Reference>[size];
	if(!size)
		return;
	for(std::size_t k = 0; k != size - 1; ++k)
		data[k].next = &data[k + 1];
	data[size - 1].next = nullptr;
}

template <typename _Reference>
HTChunk<_Reference>::~HTChunk()
{
	delete[] data;
	if(next)
		delete next;
}

template_HashTable
HashTable_templated::HashTable(_GetKey &_get_key, std::size_t _table_size, std::size_t _chunk_size) :
	get_key(_get_key),
	table_size(_table_size),
	chunk_size(_chunk_size),
	table(new Node[table_size]),
	chunk(new Chunk(chunk_size)),
	empty(chunk->data)
{
	std::memset(table, 0, sizeof(Node) * table_size);
}

template_HashTable
HashTable_templated::HashTable(_GetKey &_get_key, std::size_t _table_size) :
	HashTable(_get_key, _table_size, _table_size)
{
}

template_HashTable
HashTable_templated::~HashTable()
{
	delete[] table;
	delete chunk;
}

template_HashTable
void HashTable_templated::add(_Reference object)
{
	std::size_t pos = hash(object);
	Node *node = &table[pos];
	if(node->next) // node is used
	{
		Node *node2 = empty;
		empty = node2->next;
		if(!empty)
		{ // that was the last empty node... let's allocate another chunk of memory!
			Chunk *c2 = new Chunk(chunk_size);
			c2->next = chunk;
			chunk = c2;
			empty = chunk->data;
		}
		node2->content = object;
		if(node->next == node) // node has no chain
			node2->next = nullptr;
		else
			node2->next = node->next;
		node->next = node2;
	}
	else
	{
		node->next = node; // no chain indicator
		node->content = object;
	}
}

template_HashTable
_Reference const *HashTable_templated::get(_Key key)
{
	std::size_t pos = hash(key);
	Node *node = &table[pos];
	if(!node->next) // empty node indicator
		return nullptr;
	if(_equal(key, get_key(node->content)))
		return &node->content;
	if(node->next == node) // no chain indicator
		return nullptr;
	while((node = node->next))
		if(_equal(key, get_key(node->content)))
			return &node->content;
	return nullptr;
}

template_HashTable
_Reference const &HashTable_templated::operator[](_Key key)
{
	_Reference const *obj = get(key);
	if(!obj)
		throw std::out_of_range("Object not found in HashTable");
	return *obj;
}

template_HashTable
std::size_t HashTable_templated::hash(_Reference object)
{
	return hash(get_key(object));
}

template_HashTable
std::size_t HashTable_templated::hash(_Key key)
{
	return _hash(key) % table_size;
}

#undef HashTable_templated
#undef template_HashTable
