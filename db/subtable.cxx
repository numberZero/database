#include "subtable.hxx"
#include <cassert>

template class Subtable<Teacher>;
template class Subtable<Subject>;
template class Subtable<Room>;
template class Subtable<Group>;
template class Subtable<Time>;
template class Subtable<Row>;

template<typename _Object>
Subtable< _Object>::Subtable(File && file, defer_load_t):
	DataTable(std::move(file), defer_load)
{
}

template<typename _Object>
auto Subtable< _Object>::table() -> DataTable &
{
	return *static_cast<DataTable*>(this);
}

template<typename _Object>
auto Subtable< _Object>::table() const -> DataTable const &
{
	return *static_cast<DataTable const *>(this);
}

template<typename _Object>
void Subtable< _Object>::first_load(Id index, _Object &item)
{
	on_add(index, item);
}

template<typename _Object>
void Subtable< _Object>::on_add(Id index, _Object &item)
{
	HashTable::insert(index);
}

template<typename _Object>
void Subtable< _Object>::remove(Id row)
{
	HashTable::erase(row);
	if(!table().drop(row))
		throw std::runtime_error("Removing a node with many references");
}

template<typename _Object>
void Subtable< _Object>::add_row(Id row, Id to)
{
	table().grab(to);
	Node *&head = HashTable::find(to);
	std::size_t max_entries = HashTable::get_node_max_entries();
	assert(head->row_count <= max_entries);
	if(head->row_count == max_entries)
		head = add_node(head);
	head->rows[head->row_count++] = row;
}

template<typename _Object>
void Subtable< _Object>::remove_row(Id row, Id from)
{
	Node *&head = HashTable::find(from);
	assert(head->row_count > 0);
	Node *node = head;
	while(node)
	{
		for(std::size_t k = 0; k != node->row_count; ++k)
			if(node->rows[k] == row)
			{
				node->rows[k] = head->rows[--head->row_count];
				if(!head->row_count)
					if(HashTable::rem_node(head))
						table().drop(from);
				table().drop(from);
				return;
			}
		node = node->next_sameid;
	}
	throw std::logic_error("Row not found");
}

template<typename _Object>
HashTable::RowIterator Subtable< _Object>::end()
{
	return RowIterator();
}
