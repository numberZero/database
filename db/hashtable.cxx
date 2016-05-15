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
	std::memset(c2, 0, chunk_size);
	c2->next = chunk;
	chunk = c2;
}

auto HashTable::next_empty(Node *node) -> Node *&
{
	return *reinterpret_cast<Node **>(node);
}

std::size_t HashTable::get_node_max_entries() const
{
	return node_entries;
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

auto HashTable::find(Id id) -> Node *&
{
	Hash pos = hash(key_of(id)) % table_entries;
	Node **pnode = &table[pos];
	Node *node;
	while((node = *pnode))
	{
		if(node->id == id)
			return *pnode;
		pnode = &node->next_samehash;
	}
	throw std::out_of_range("Node with given ID not found");
}

auto HashTable::hash(Id id) -> Hash
{
	return hash(key_of(id));
}

auto HashTable::alloc_node() -> Node *
{
	Node *node;
	if(empty)
	{ // we have an empty node in the queue
		node = empty;
		empty = next_empty(node);
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
	return node;
}

void HashTable::free_node(Node *node)
{
	next_empty(node) = empty;
	empty = node;
}

auto HashTable::add_node(Node *&chain_head) -> Node *
{
	Node *node = alloc_node();
	node->id = chain_head->id;
	node->next_sameid = chain_head;
	node->next_samehash = chain_head->next_samehash; // necessary as chain_head is somewhere in the samehash chain
	chain_head = node;
	return node;
}

bool HashTable::rem_node(Node *&chain_head)
{
	Node *node = chain_head;
	if(node->next_sameid)
	{ // shortening chain
		chain_head = node->next_sameid;
		free_node(node);
		return false;
	}
	else
	{ // that was the last node in the chain; removing the chain
		chain_head = node->next_samehash;
		free_node(node);
		return true;
	}
}

void HashTable::insert(Id object)
{
	std::size_t pos = hash(object) % table_entries;
	Node *&place = table[pos];
	Node *node = alloc_node();
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


HashTable::RowIterator::RowIterator(Node const *_node) :
	node(_node),
	position(0)
{
}

bool HashTable::RowIterator::operator!=(RowIterator const &b) const
{
	return (node == b.node) && (position == b.position);
}

bool HashTable::RowIterator::operator!() const
{
	return !node;
}

Id HashTable::RowIterator::operator*() const
{
	return node->rows[position];
}

HashTable::RowIterator &HashTable::RowIterator::operator++()
{
	if(++position > node->row_count)
	{
		node = node->next_sameid;
		position = 0;
	}
}
