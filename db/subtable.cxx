#include "subtable.hxx"
#include <cassert>

template class Subtable<Teacher>;
template class Subtable<Subject>;
template class Subtable<Room>;
template class Subtable<Group>;
template class Subtable<Time>;

template<typename _Object>
Subtable< _Object>::Subtable(std::string const &file):
	DataTable(file)
{
}

template<typename _Object>
Subtable< _Object>::Subtable(File && file):
	DataTable(std::move(file))
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
