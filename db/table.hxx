#pragma once
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <memory>
#include <mutex>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "file.hxx"

static std::size_t const invalid_index = static_cast<std::size_t>(-1);

class BackedTable
{
private:
	typedef std::int16_t RefCount;

	struct Entry
	{
		RefCount rc;
		char data[];
	};

	File fd;
	void *data;

	std::size_t entry_size;
	std::size_t entry_count;
	std::size_t next_insert_id;
	std::size_t capacity;

	std::unique_ptr<std::size_t[]> free_entries_buffer;
	std::size_t free_entries_start;
	std::size_t free_entries_count;
	std::size_t free_entries_capacity;

	Entry *get_entry_addr(std::size_t index) const noexcept;
	bool is_entry(std::size_t index) const noexcept;
	Entry *get_entry(std::size_t index) const;

protected:
	std::pair<std::size_t, void *> create();
	void *get(std::size_t index);
	void const *get(std::size_t index) const;
	void grab(std::size_t index);
	void drop(std::size_t index);

	bool first(std::size_t &index);
	bool next(std::size_t &index);

	virtual void first_load(std::size_t index, void *object);

public:
	BackedTable(std::size_t item_size, std::string const &filename);
	~BackedTable();

	std::size_t size() const;
};

template <typename _Item>
class Table :
	public BackedTable
{
public:
	typedef _Item Item;
	static_assert(std::is_pod<Item>::value, "Item must be POD type");

	class Iterator
	{
		friend class Table;

	private:
		Table &table;
		std::size_t index;

		Iterator(Table &_table, std::size_t _index);

	public:
		Iterator(Iterator const &b) = default;
		Iterator& operator= (Iterator const &b) = default;
		bool operator!= (Iterator const& b) const;
		Item &operator* ();
		Item *operator-> ();
		Iterator &operator++ ();
	};

protected:
	void first_load(std::size_t index, void *item) override;
	virtual void first_load(std::size_t index, Item &item);

public:
	Table(std::string const &filename);

	std::pair<std::size_t, Item *> alloc();
	std::size_t insert(Item const& b);
	Item &get(std::size_t index);
	Item const &get(std::size_t index) const;
	using BackedTable::grab;
	using BackedTable::drop;

	Iterator begin();
	Iterator end();
};

template <typename _Item>
Table<_Item>::Iterator::Iterator(Table &_table, std::size_t _index) :
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
}

template <typename _Item>
Table<_Item>::Table(std::string const &filename) :
	BackedTable(sizeof(Item), filename)
{
}

template <typename _Item>
void Table<_Item>::first_load(std::size_t index, void *item)
{
	BackedTable::first_load(index, item);
	first_load(index, *reinterpret_cast<Item *>(item));
}

template <typename _Item>
void Table<_Item>::first_load(std::size_t index, Item &item)
{
}

template <typename _Item>
std::pair<std::size_t, _Item *> Table<_Item>::alloc()
{
	auto p = BackedTable::create();
	return{p.first, reinterpret_cast<Item *>(p.second)};
}

template <typename _Item>
std::size_t Table<_Item>::insert(Item const &b)
{
	auto p = BackedTable::create();
	new(p.second) Item(b);
	return p.first;
}

template <typename _Item>
_Item &Table<_Item>::get(std::size_t index)
{
	return *reinterpret_cast<Item *>(BackedTable::get(index));
}

template <typename _Item>
_Item const &Table<_Item>::get(std::size_t index) const
{
	return *reinterpret_cast<Item const *>(BackedTable::get(index));
}

template <typename _Item>
typename Table<_Item>::Iterator Table<_Item>::begin()
{
	std::size_t index;
	if(first(index))
		return{*this, index};
	return end();
}

template <typename _Item>
typename Table<_Item>::Iterator Table<_Item>::end()
{
	return{*this, invalid_index};
}
