#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <utility>
#include "data.hxx"

class HashTable
{
public:
	typedef std::size_t Hash;
	typedef void const *PKey;

protected:
	struct Node
	{
		Id id;
		Node *next_samehash;
		Node *next_sameid;
		std::size_t row_count;
		Id rows[];
	};

private:
	struct Chunk
	{
		Chunk *next;
		std::size_t insert_id;
// 		Node data[]; // not using array to avoid confusion as Node size is not constant
	};

	std::size_t node_entries;
	std::size_t chunk_entries;
	std::size_t table_entries;
	std::size_t node_size;
	std::size_t chunk_size;
	std::size_t table_size;
	Node **table;
	Chunk *chunk;
	Node *empty;

	void allocate_chunk();

protected:
	HashTable();
	~HashTable();

	Node *find(PKey key);
	Hash hash(Id id);

	void insert(Id object);
	void erase(Id object);
	Id get(PKey key); ///< \returns INVALID_ID if not found
	Id at(PKey key); ///< \throws std::out_of_range if not found

	virtual PKey key_of(Id id) = 0;
	virtual Hash hash(PKey key) = 0;
	virtual bool equal(PKey key1, PKey key2) = 0;
};
