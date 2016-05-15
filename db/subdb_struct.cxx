#include "subdb_struct.hxx"
#include "dbcommon.hxx"
#include "hashers.hxx"

template class SubDB_Struct<Room, unsigned>;
template class SubDB_Struct<Group, unsigned>;
template class SubDB_Struct<Time, unsigned, unsigned>;

template<typename _Object, typename... Params>
auto SubDB_Struct< _Object, Params...>::key_of(Id id) -> PKey
{
	return reinterpret_cast<PKey>(&table().get(id));
}

template<typename _Object, typename... Params>
auto SubDB_Struct< _Object, Params...>::hash(PKey key) -> Hash
{
	return hash_bytes(key, sizeof(_Object));
}

template<typename _Object, typename... Params>
bool SubDB_Struct< _Object, Params...>::equal(PKey key1, PKey key2)
{
	return !std::memcmp(key1, key2, sizeof(_Object));
}

template<typename _Object, typename... Params>
Id SubDB_Struct< _Object, Params...>::add(Params... params)
{
	auto p = table().alloc();
	*p.second = _Object{params...};
	HashTable::insert(p.first);
	return p.first;
}

template<typename _Object, typename... Params>
Id SubDB_Struct< _Object, Params...>::find(Params... params)
{
	_Object key{params...};
	return HashTable::at(&key);
}

template<typename _Object, typename... Params>
Id SubDB_Struct< _Object, Params...>::need(Params... params)
{
	_Object key{params...};
	Id id = HashTable::get(&key);
	if(id != INVALID_ID)
		return id;
	return add(params...);
}

template<typename _Object, typename... Params>
HashTable::RowIterator SubDB_Struct< _Object, Params...>::begin(Params... params)
{
	_Object key{params...};
	return HashTable::RowIterator(HashTable::find(&key));
}
