#define TEMPLATES__SUBTABLE
#include "subtable.hxx"
#include <cassert>
#include "subdb_string.hxx"
#include "subdb_struct.hxx"

template class Subtable<Teacher, &Row::teacher>;
template class Subtable<Subject, &Row::subject>;
template class Subtable<Room, &Row::room>;
template class Subtable<Group, &Row::group>;
template class Subtable<Time, &Row::time>;
template class Subtable<Row, nullptr>;

template <typename _Object, Id Row::*pid>
Subtable< _Object, pid>::Subtable(File && file, defer_load_t):
	DataTable(std::move(file), defer_load),
	HashTable(pid ? 16 : 0) // really, we don’t need row list iff we have no field in the row
{
}

template <typename _Object, Id Row::*pid>
auto Subtable< _Object, pid>::table() -> DataTable &
{
	return *static_cast<DataTable*>(this);
}

template <typename _Object, Id Row::*pid>
auto Subtable< _Object, pid>::table() const -> DataTable const &
{
	return *static_cast<DataTable const *>(this);
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::first_load(Id index, _Object &item)
{
	on_add(index, item);
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::change_id(Id from, Id to)
{
	HashTable::insert(to);
	if(pid)
	{
		Rows *rows = dynamic_cast<Rows *>(this);
		Node *&a = HashTable::find(from);
		for(HashTable::RowIterator iter(a); iter != end(); ++iter)
		{
			Id id = *iter;
			rows->get(id).*pid = to;
			add_row(id, to);
		}
	}
	else
	{
		dynamic_cast<SubDB_Teacher *>(this)->update_row_id(from, to);
		dynamic_cast<SubDB_Subject *>(this)->update_row_id(from, to);
		dynamic_cast<SubDB_Room *>(this)->update_row_id(from, to);
		dynamic_cast<SubDB_Group *>(this)->update_row_id(from, to);
		dynamic_cast<SubDB_Time *>(this)->update_row_id(from, to);
	}
	HashTable::erase(from);
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::on_add(Id index, _Object &)
{
	HashTable::insert(index);
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::remove(Id row)
{
	HashTable::erase(row);
	if(!table().drop(row))
		throw std::runtime_error("Removing a node with many references");
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::add_row(Id row, Id to)
{
#ifdef DO_REFCOUNTING
	table().grab(to);
#endif
	Node *&head = HashTable::find(to);
	Node *node = head;
	std::size_t max_entries = HashTable::get_node_max_entries();
	assert(head->row_count <= max_entries);
	if(head->row_count == max_entries)
		node = add_node(head);
	node->row(node->row_count++) = row;
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::remove_row(Id row, Id from)
{
	Node *&head = HashTable::find(from);
	assert(head->row_count > 0);
	Node *node = head;
	while(node)
	{
		for(std::size_t k = 0; k != node->row_count; ++k)
			if(node->row(k) == row)
			{
				node->row(k) = head->row(--head->row_count);
				if(!head->row_count)
					if(HashTable::rem_node(head))
						table().drop(from);
#ifdef DO_REFCOUNTING
				table().drop(from);
#endif
				return;
			}
		node = node->next_sameid;
	}
	throw std::logic_error("Row not found");
}

template <typename _Object, Id Row::*pid>
void Subtable< _Object, pid>::update_row_id(Id from, Id to)
{
	assert(pid);
	Rows *rows = dynamic_cast<Rows *>(this);
	update_row_id(rows->get(from).*pid, from, to);
}

template <typename _Object, Id Row::*pid>
HashTable::RowIterator Subtable< _Object, pid>::end()
{
	return RowIterator();
}
