#ifndef NDEBUG
#include <iostream>
#endif
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "backedtable.hxx"
#include "rtcheck.hxx"

static int counter = 0;

static Id increase_capacity(Id capacity)
{
	if(capacity < 0x00000100)
		return capacity + 0x00000010;
	if(capacity < 0x00010000)
		return capacity * 2;
	return capacity + 0x00010000;
}


BackedTable::BackedTable(std::size_t item_size, File &&file, defer_load_t) :
	fd(std::move(file)),
	entry_size(item_size + sizeof(Entry)),
	entry_count(0),
	next_insert_id(0),
	free_entries_start(0),
	free_entries_count(0),
	free_entries_capacity(1024),
	free_entries_buffer(new Id[free_entries_capacity])
{
	off_t bytes = lseek((int)fd, 0, SEEK_END);
	syserror_throwif(bytes == (off_t)-1, "Can't check file size for BackedTable");
	capacity = bytes / entry_size;
	data = mmap(nullptr, capacity * entry_size, PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);
	syserror_throwif(!data, "Can't check file size for BackedTable");
}

BackedTable::BackedTable(std::size_t item_size, std::string const &filename) :
	BackedTable(
		item_size,
		File(open(filename.c_str(), O_RDWR | O_CREAT, 0640))
	)
{
}

BackedTable::BackedTable(std::size_t item_size, File &&file) :
	BackedTable(item_size, std::move(file), defer_load)
{
	load();
}

void BackedTable::load()
{
	for(Id k = 0; k != capacity; ++k)
	{
		Entry *e(get_entry_addr(k));
		if(!e->rc) // if free
			continue; // we will add it later, if needed
		++entry_count;
		first_load(k, e->data);
		for(Id m = next_insert_id; m != k; ++m)
		{ // [next_insert_id; k) all are free
			if(free_entries_count >= free_entries_capacity)
				throw std::runtime_error("Free entries buffer overflow");
			std::size_t pos = free_entries_count++;
			free_entries_buffer[pos] = m;
		}
		next_insert_id = k + 1;
	}
}

BackedTable::~BackedTable()
{
	munmap(data, capacity * entry_size);
}

BackedTable::Entry *BackedTable::get_entry_addr(Id index) const noexcept
{
	return reinterpret_cast<BackedTable::Entry *>(reinterpret_cast<char *>(data) + entry_size * index);
}

bool BackedTable::is_entry(Id index) const noexcept
{
	if(index >= capacity)
		return false;
	return get_entry_addr(index)->rc != 0;
}

BackedTable::Entry *BackedTable::get_entry(Id index) const
{
	if(index >= capacity)
		throw std::out_of_range("BackedTable index out of range");
	Entry *entry(get_entry_addr(index));
	if(!entry->rc)
		throw std::out_of_range("BackedTable index points to non-existing entry");
	return entry;
}

std::pair<Id, void *> BackedTable::create()
{
	Id index;
	if(free_entries_count)
	{
		index = free_entries_buffer[free_entries_start];
		--free_entries_count;
		++free_entries_start;
		if(free_entries_start == free_entries_capacity)
			free_entries_start = 0;
	}
	else
	{
		index = next_insert_id++;
		if(index >= capacity)
		{ // extending the table
			Id new_capacity = increase_capacity(capacity);
			std::size_t new_size = new_capacity * entry_size;
			syserror_throwif(ftruncate((int)fd, new_size), "Can't extend table file");
			void *file = mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);
			syserror_throwif(!file, "Can't extend table mapping");
			munmap(data, capacity * entry_size);
			data = file;
			capacity = new_capacity;
		}
	}
	Entry *entry(get_entry_addr(index));
	if(entry->rc)
		throw std::runtime_error("Database file corrupted");
	entry->rc = 1;
	++entry_count;
	return{index, entry->data};
}

void *BackedTable::get(Id index)
{
	return get_entry(index)->data;
}

void const *BackedTable::get(Id index) const
{
	return get_entry(index)->data;
}

void BackedTable::grab(Id index)
{
	Entry *entry(get_entry(index));
	if(!++entry->rc) // overflow
		throw std::range_error("Reference counting overflow");
}

bool BackedTable::drop(Id index)
{
	Entry *entry(get_entry(index));
	RefCount rc = --entry->rc;
	if(rc < 0) // underflow
		throw std::logic_error("Reference counting underflow");
	if(rc == 0) // last reference dropped
	{
#ifndef NDEBUG
		std::clog << "Last reference dropped to " << index  << std::endl;
#endif
		--entry_count;
		if(free_entries_count >= free_entries_capacity)
			throw std::runtime_error("Free entries buffer overflow");
		std::size_t pos = (free_entries_start + free_entries_count++) % free_entries_capacity;
		free_entries_buffer[pos] = index;
		return true;
	}
	return false;
}

bool BackedTable::first(Id &index) const
{
	index = invalid_index;
	return next(index);
}

bool BackedTable::next(Id &index) const
{
	while(++index < capacity)
		if(get_entry_addr(index)->rc)
			return true;
	return false;
}

void BackedTable::first_load(Id index, void *object)
{
}

Id BackedTable::size() const
{
	return entry_count;
}
