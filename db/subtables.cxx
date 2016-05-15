#include "subtables.hxx"
#include "dbcommon.hxx"
#include "hashers.hxx"

#ifndef USE_TEMPORARY_FOR_KEY
#define USE_VARIABLE_FOR_TEMPKEY
#endif

template class SubDB<Teacher, char const *, std::string const &>;
template class SubDB<Subject, char const *, std::string const &>;
template class SubDB<Room, std::uint_fast32_t, unsigned>;
template class SubDB<Group, std::uint_fast32_t, unsigned>;
template class SubDB<Time, std::uint_fast32_t, unsigned, unsigned>;

template <typename _Object, typename... Params>
struct helper
{
	typedef _Object Object;
	typedef _Object TempKey;
	typedef HashTable::Hash Hash;
	typedef HashTable::PKey PKey;

	static void make(Object &object, Params... params)
	{
		object = Object{params...};
	}
#ifdef USE_VARIABLE_FOR_TEMPKEY
	static TempKey mk_tmp_key(Params... params)
	{
		return{params...};
	}

	static PKey  use_tmp_key(TempKey &key)
	{
		return &key;
	}
#else
	static Object tmp_key(Params... params)
	{
		return{params...};
	}
#endif
	static PKey key_of(Object &object)
	{
		return &object;
	}

	static Hash hash(PKey key)
	{
		return hash_bytes(key, sizeof(Object));
	}

	static bool equal(PKey key1, PKey key2)
	{
		return !std::memcmp(key1, key2, sizeof(Object));
	}
};

template <typename _Object>
struct helper<_Object, std::string const &>
{
	typedef _Object Object;
	typedef char const *TempKey;
	typedef HashTable::Hash Hash;
	typedef HashTable::PKey PKey;

	static void make(Object &object, std::string const &name)
	{
		if(name.length() >= Object::name_len)
			throw DataError("Name too long");
		std::memcpy(object.name, name.data(), name.length());
		object.name[name.length()] = 0;
	}
#ifdef USE_VARIABLE_FOR_TEMPKEY
	static TempKey mk_tmp_key(std::string const &name)
	{
		return name.c_str();
	}

	static PKey  use_tmp_key(TempKey &key)
	{
		return key;
	}
#else
	static char const &tmp_key(std::string const &name)
	{
		return *name.c_str();
	}
#endif
	static PKey key_of(Object &object)
	{
		return object.name;
	}

	static Hash hash(PKey key)
	{
		return hash_string(reinterpret_cast<char const *>(key));
	}

	static bool equal(PKey key1, PKey key2)
	{
		return !std::strcmp(reinterpret_cast<char const *>(key1), reinterpret_cast<char const *>(key2));
	}
};

#define Helper	helper< _Object, Params...>
#define Subtable	SubDB< _Object, _Key, Params...>
#define template_Subtable	template<typename _Object, typename _Key, typename... Params>

template_Subtable Subtable::SubDB(std::string const &file):
	data(file)
{
}

template_Subtable Subtable::SubDB(File && file):
	data(std::move(file))
{
}

template_Subtable auto Subtable::key_of(Id id) -> PKey
{
	return Helper::key_of(data.get(id));
}

template_Subtable auto Subtable::hash(PKey key) -> Hash
{
	return Helper::hash(key);
}

template_Subtable bool Subtable::equal(PKey key1, PKey key2)
{
	return Helper::equal(key1, key2);
}

template_Subtable Id Subtable::add(Params... params)
{
	auto p = data.alloc();
	Helper::make(*p.second, params...);
	insert(p.first);
	return p.first;
}

template_Subtable Id Subtable::find(Params... params)
{
#ifdef USE_VARIABLE_FOR_TEMPKEY
	typename Helper::TempKey key(Helper::mk_tmp_key(params...));
	return at(Helper::use_tmp_key(key));
#else
	return at(&Helper::tmp_key(params...));
#endif
}

template_Subtable Id Subtable::need(Params... params)
{
#ifdef USE_VARIABLE_FOR_TEMPKEY
	typename Helper::TempKey key(Helper::mk_tmp_key(params...));
	Id id = get(Helper::use_tmp_key(key));
#else
	Id id = get(&Helper::tmp_key(params...));
#endif
	if(id != INVALID_ID)
		return id;
	return add(params...);
}
