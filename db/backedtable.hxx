#pragma once
#include <cstdint>
#include <memory>
#include "data.hxx"
#include "file.hxx"

static Id const invalid_index = INVALID_ID;
static struct defer_load_t {} defer_load;

class BackedTable
{
private:
	typedef std::uint32_t RefCount;

	struct Entry
	{
		RefCount rc;
		char data[];
	};

	File fd;
	void *data;

	std::size_t entry_size;
	Id entry_count;
	Id next_insert_id;
	Id capacity;

	std::size_t free_entries_start;
	std::size_t free_entries_count;
	std::size_t free_entries_capacity;
	std::unique_ptr<Id[]> free_entries_buffer;

	Entry *get_entry_addr(Id index) const noexcept;
	bool is_entry(Id index) const noexcept;
	Entry *get_entry(Id index) const;

protected:
	std::pair<Id, void *> create();
	void *get(Id index);
	void const *get(Id index) const;
	void grab(Id index);
	bool drop(Id index); ///< \returns whether the item was deleted

	virtual void first_load(Id index, void *object);

	BackedTable(std::size_t item_size, File &&file, defer_load_t);
	void load();

public:
	BackedTable(std::size_t item_size, std::string const &filename);
	BackedTable(std::size_t item_size, File &&file);
	virtual ~BackedTable();

	bool first(Id &index) const;
	bool next(Id &index) const;

	Id size() const;
};
