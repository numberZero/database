#include "subdb_struct.hxx"
#include "dbcommon.hxx"
#include "hashers.hxx"

template class SubDB_Struct<Room, std::uint32_t>;
template class SubDB_Struct<Group, std::uint32_t>;
template class SubDB_Struct<Time, std::uint16_t, std::uint16_t>;
template class SubDB_Struct<Row, Id, Id, Id, Id, Id>;

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
	this->on_add(p.first, *p.second);
	return p.first;
}

template<typename _Object, typename... Params>
Id SubDB_Struct< _Object, Params...>::find(Params... params)
{
	_Object key{params...};
	return HashTable::get(&key);
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
