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
#ifdef USE_ZERO_SIZED_ARRAY
		Id rows[];
		Id &row(std::size_t n) { return rows[n]; }
		Id const &row(std::size_t n) const { return rows[n]; }
#else
		Id &row(std::size_t n) { return reinterpret_cast<Id *>(this + 1)[n]; }
		Id const &row(std::size_t n) const { return reinterpret_cast<Id const *>(this + 1)[n]; }
#endif
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
	Node *&next_empty(Node *node);

protected:
	HashTable(std::size_t default_node_entries = 16);
	~HashTable();

	std::size_t get_node_max_entries() const;

	Node *find(PKey key);
	Node *&find(Id id);
	Hash hash(Id id);

	Node *alloc_node();
	void free_node(Node *node);

	Node *add_node(Node *&chain_head);
	bool rem_node(Node *&chain_head); ///< \returns whether the chain was removed completely

	void insert(Id object);
	void erase(Id object);
	void update_row_id(Id object, Id from, Id to);
	Id get(PKey key); ///< \returns INVALID_ID if not found
	Id at(PKey key); ///< \throws std::out_of_range if not found

	virtual PKey key_of(Id id) = 0;
	virtual Hash hash(PKey key) = 0;
	virtual bool equal(PKey key1, PKey key2) = 0;

public:
	class RowIterator
	{
	private:
		Node const *node = nullptr;
		std::size_t position = 0;

	public:
		RowIterator() = default;
		RowIterator(Node const *);
		RowIterator(RowIterator const &) = default;
		RowIterator &operator= (RowIterator const &) = default;

		bool operator!= (RowIterator const &) const;
		bool operator! () const;
		Id operator* () const;
		RowIterator &operator++ ();
	};
};
