#define TABLE_CXX
#include "data.hxx"
#include "table.hxx"

template class Table<Teacher>;
template class Table<Subject>;
template class Table<Room>;
template class Table<Group>;
template class Table<Time>;
template class Table<Row>;

template <typename _Item>
Table<_Item>::Iterator::Iterator(Table &_table, Id _index) :
	table(_table), index(_index)
{
}

template <typename _Item>
bool Table<_Item>::Iterator::operator!=(Iterator const &b) const
{
	return (&table == &b.table) && (index == b.index);
}

template <typename _Item>
_Item &Table<_Item>::Iterator::operator*()
{
	return table.get(index);
}

template <typename _Item>
_Item *Table<_Item>::Iterator::operator->()
{
	return &table.get(index);
}

template <typename _Item>
typename Table<_Item>::Iterator &Table<_Item>::Iterator::operator++()
{
	if(!table.next(index))
		index = invalid_index;
	return *this;
}

template <typename _Item>
Table<_Item>::Table(File &&file, defer_load_t) :
	BackedTable(sizeof(Item), std::move(file), defer_load)
{
}

template <typename _Item>
void Table<_Item>::first_load(Id index, void *item)
{
	BackedTable::first_load(index, item);
	first_load(index, *reinterpret_cast<Item *>(item));
}

template <typename _Item>
void Table<_Item>::first_load(Id, Item &)
{
}

template <typename _Item>
std::pair<Id, _Item *> Table<_Item>::alloc()
{
	auto p = BackedTable::create();
	return{p.first, reinterpret_cast<Item *>(p.second)};
}

template <typename _Item>
Id Table<_Item>::insert(Item const &b)
{
	auto p = BackedTable::create();
	new(p.second) Item(b);
	return p.first;
}

template <typename _Item>
_Item &Table<_Item>::get(Id index)
{
	return *reinterpret_cast<Item *>(BackedTable::get(index));
}

template <typename _Item>
_Item const &Table<_Item>::get(Id index) const
{
	return *reinterpret_cast<Item const *>(BackedTable::get(index));
}

template <typename _Item>
typename Table<_Item>::Iterator Table<_Item>::begin()
{
	Id index;
	if(first(index))
		return{*this, index};
	return end();
}

template <typename _Item>
typename Table<_Item>::Iterator Table<_Item>::end()
{
	return{*this, invalid_index};
}
