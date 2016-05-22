#include "subdb_string.hxx"
#include "dbcommon.hxx"
#include "hashers.hxx"

template class SubDB_String<Teacher>;
template class SubDB_String<Subject>;

template<typename _Object>
auto SubDB_String< _Object>::key_of(Id id) -> PKey
{
	return reinterpret_cast<PKey>(table().get(id).name);
}

template<typename _Object>
auto SubDB_String< _Object>::hash(PKey key) -> Hash
{
	return hash_string(reinterpret_cast<char const *>(key));
}

template<typename _Object>
bool SubDB_String< _Object>::equal(PKey key1, PKey key2)
{
	return !std::strcmp(reinterpret_cast<char const *>(key1), reinterpret_cast<char const *>(key2));
}

template<typename _Object>
Id SubDB_String< _Object>::add(std::string const &name)
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

template<typename _Object>
Id SubDB_String< _Object>::find(std::string const &name)
{
	return HashTable::get(reinterpret_cast<PKey>(name.c_str()));
}

template<typename _Object>
Id SubDB_String< _Object>::need(std::string const &name)
{
	Id id = HashTable::get(reinterpret_cast<PKey>(name.c_str()));
	if(id != INVALID_ID)
		return id;
	return add(name);
}

template<typename _Object>
HashTable::RowIterator SubDB_String< _Object>::begin(std::string const &name)
{
	return HashTable::RowIterator(HashTable::find(name.c_str()));
}
