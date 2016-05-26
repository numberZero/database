#pragma once
#include <type_traits>
#include "backedtable.hxx"
#include "data.hxx"

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
		Id index;

		Iterator(Table &_table, Id _index);

	public:
		Iterator(Iterator const &b) = default;
		Iterator& operator= (Iterator const &b) = default;
		bool operator!= (Iterator const& b) const;
		Item &operator* ();
		Item *operator-> ();
		Iterator &operator++ ();
	};

private:
	void first_load(Id index, void *item) override;

protected:
	Table(File &&file, defer_load_t);
	using BackedTable::load;
	virtual void first_load(Id index, Item &item);

public:
	std::pair<Id, Item *> alloc();
	Id insert(Item const& b);
	Item &get(Id index);
	Item const &get(Id index) const;
	using BackedTable::grab;
	using BackedTable::drop;

	Iterator begin();
	Iterator end();
};

#ifdef USE_EXTERNAL_TEMPLATE_INSTANTIATION
extern template class Table<Teacher>;
extern template class Table<Subject>;
extern template class Table<Room>;
extern template class Table<Group>;
extern template class Table<Time>;
extern template class Table<Row>;
#else
#include "table.cxx"
#endif
