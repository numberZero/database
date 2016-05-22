#include "subdb_string.hxx"
#include "dbcommon.hxx"
#include "hashers.hxx"

template class SubDB_String<Teacher, &Row::teacher>;
template class SubDB_String<Subject, &Row::subject>;

template <typename _Object, Id Row::*pid>
auto SubDB_String<_Object, pid>::key_of(Id id) -> PKey
{
	return reinterpret_cast<PKey>(table().get(id).name);
}

template <typename _Object, Id Row::*pid>
auto SubDB_String<_Object, pid>::hash(PKey key) -> Hash
{
	return hash_string(reinterpret_cast<char const *>(key));
}

template <typename _Object, Id Row::*pid>
bool SubDB_String<_Object, pid>::equal(PKey key1, PKey key2)
{
	return !std::strcmp(reinterpret_cast<char const *>(key1), reinterpret_cast<char const *>(key2));
}

template <typename _Object, Id Row::*pid>
Id SubDB_String<_Object, pid>::add(std::string const &name)
{
	std::size_t len = name.length();
	if(len >= _Object::name_len)
		throw DataError("Name too long");
	auto p = table().alloc();
	std::memcpy(p.second->name, name.c_str(), len);
	p.second->name[len] = 0;
	this->on_add(p.first, *p.second);
	return p.first;
}

template <typename _Object, Id Row::*pid>
Id SubDB_String<_Object, pid>::find(std::string const &name)
{
	return HashTable::get(reinterpret_cast<PKey>(name.c_str()));
}

template <typename _Object, Id Row::*pid>
Id SubDB_String<_Object, pid>::need(std::string const &name)
{
	Id id = HashTable::get(reinterpret_cast<PKey>(name.c_str()));
	if(id != INVALID_ID)
		return id;
	return add(name);
}

template <typename _Object, Id Row::*pid>
HashTable::RowIterator SubDB_String<_Object, pid>::begin(std::string const &name)
{
	return HashTable::RowIterator(HashTable::find(name.c_str()));
}
