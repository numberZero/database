#include "hashtable.hxx"
#include <cassert>
#include <cstdlib>

HashTable::HashTable() :
	node_entries(0x00000010),
	chunk_entries(0x00001000),
	table_entries(0x00010000),
	node_size(sizeof(Node) + sizeof(Id[node_entries])),
	chunk_size(sizeof(Chunk) + node_size * chunk_entries),
	table_size(sizeof(Node *) * table_entries),
	table(nullptr),
	chunk(nullptr),
	empty(nullptr)
{
	table = reinterpret_cast<Node **>(std::malloc(table_size));
	std::memset(table, 0, table_size);
	allocate_chunk(); // we must have at least one data chunk
}

HashTable::~HashTable()
{
	std::free(table);
	while(chunk)
	{
		Chunk *c2 = chunk->next;
		std::free(chunk);
		chunk = c2;
	}
}

void HashTable::allocate_chunk()
{
	Chunk *c2 = reinterpret_cast<Chunk *>(std::malloc(chunk_size));
	c2->next = chunk;
	c2->insert_id = 0;
	chunk = c2;
}

auto HashTable::find(PKey key) -> Node *
{
	Hash pos = hash(key) % table_entries;
	Node *node = table[pos];
	while(node)
	{
		if(equal(key, key_of(node->id)))
			return node;
		node = node->next_samehash;
	}
}

auto HashTable::hash(Id id) -> Hash
{
	return hash(key_of(id));
}

void HashTable::insert(Id object)
{
	std::size_t pos = hash(object) % table_entries;
	Node *&place = table[pos];
	Node *node;
	if(empty)
	{ // we have an empty node in the queue
		node = empty;
		empty = *reinterpret_cast<Node **>(node);
	}
	else
	{ // empty nodes queue is empty itself
		if(chunk->insert_id >= chunk_entries) // only == is possible
			allocate_chunk();
		// now we certainly have some space in the current memory chunk
		node = reinterpret_cast<Node *>(
			reinterpret_cast<char *>(chunk) +
			sizeof(Chunk) +
			node_size * chunk->insert_id++
		);
	}
	node->id = object;
	node->next_samehash = place;
	node->next_sameid = nullptr;
	node->row_count = 0;
	place = node;
}
/*
void HashTable::erase(Id object)
{

}
*/
Id HashTable::get(PKey key)
{
	Node *node = find(key);
	if(!node)
		return INVALID_ID;
	return node->id;
}

Id HashTable::at(PKey key)
{
	Id id = get(key);
	if(id == INVALID_ID)
		throw std::out_of_range("Object not found in hash-table");
}
