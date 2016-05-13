#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "table.hxx"

static int counter = 0;

static std::size_t increase_capacity(std::size_t capacity)
{
	if(capacity < 0x00000100)
		return capacity + 0x00000010;
	if(capacity < 0x00010000)
		return capacity * 2;
	return capacity + 0x00010000;
}

[[noreturn]] static void syserror(std::string const& what)
{
	throw std::system_error(errno, std::system_category(), what);
}

#define ensure(value,message) if(!value) syserror(message)

BackedTable::BackedTable(std::size_t item_size, std::string const &filename) :
	entry_size(item_size + sizeof(Entry)),
	entry_count(0),
	next_insert_id(0),
	free_entries_start(0),
	free_entries_count(0),
	free_entries_capacity(1024)
{
// open the backing storage
	std::string fn = filename;
	if(filename.empty())
		fn = "/tmp/zolden-data-" + std::to_string(++counter) + ".zdb";
	fd.reset(open(fn.c_str(), O_CREAT | O_RDWR, 0640));
	ensure(fd, "Can't open file " + filename + " for BackedTable");
	off_t bytes = lseek((int)fd, 0, SEEK_END);
	ensure(bytes == (off_t)-1, "Can't check file size for BackedTable");
	capacity = bytes / entry_size;
	data = mmap(nullptr, capacity * entry_size, PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);
	ensure(data, "Can't check file size for BackedTable");

// prepare metadata
	free_entries_buffer.reset(new std::size_t[free_entries_capacity]);
	for(std::size_t k = 0; k != capacity; ++k)
	{
		Entry *e(get_entry_addr(k));
		if(!e->rc) // if free
			continue; // we will add it later, if needed
		first_load(k, e->data);
		for(std::size_t m = next_insert_id; m != k; ++m)
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

BackedTable::Entry *BackedTable::get_entry_addr(std::size_t index) const noexcept
{
	return reinterpret_cast<BackedTable::Entry *>(reinterpret_cast<char *>(data) + entry_size * index);
}

bool BackedTable::is_entry(std::size_t index) const noexcept
{
	if(index >= capacity)
		return false;
	return get_entry_addr(index)->rc != 0;
}

BackedTable::Entry *BackedTable::get_entry(std::size_t index) const
{
	if(index >= capacity)
		throw std::out_of_range("BackedTable index out of range");
	Entry *entry(get_entry_addr(index));
	if(!entry->rc)
		throw std::out_of_range("BackedTable index points to non-existing entry");
	return entry;
}

std::pair<std::size_t, void *> BackedTable::create()
{
	std::size_t index;
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
			std::size_t new_capacity = increase_capacity(capacity);
			std::size_t new_size = new_capacity * entry_size;
			ensure(0 == ftruncate((int)fd, new_size), "Can't extend table file");
			void *file = mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);
			ensure(file, "Can't extend table mapping");
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

void *BackedTable::get(std::size_t index)
{
	return get_entry(index)->data;
}

void const *BackedTable::get(std::size_t index) const
{
	return get_entry(index)->data;
}

void BackedTable::grab(std::size_t index)
{
	Entry *entry(get_entry(index));
	if(++entry->rc < 0) // overflow
		throw std::range_error("Reference counting overflow");
}

void BackedTable::drop(std::size_t index)
{
	Entry *entry(get_entry(index));
	RefCount rc = --entry->rc;
	if(rc < 0) // underflow
		throw std::logic_error("Reference counting underflow");
	if(rc == 0) // last reference dropped
	{
		--entry_count;
		if(free_entries_count >= free_entries_capacity)
			throw std::runtime_error("Free entries buffer overflow");
		std::size_t pos = (free_entries_start + free_entries_count++) % free_entries_capacity;
		free_entries_buffer[pos] = index;
	}
}

bool BackedTable::first(std::size_t &index)
{
	index = invalid_index;
	return next(index);
}

bool BackedTable::next(std::size_t &index)
{
	while(++index < capacity)
		if(get_entry_addr(index)->rc)
			return true;
	return false;
}

void BackedTable::first_load(std::size_t index, void *object)
{
	++entry_count;
}

std::size_t BackedTable::size() const
{
	return entry_count;
}
