#include "subtables.hxx"

template class SubDB<Teacher, char const *, std::string const &>;
template class SubDB<Subject, char const *, std::string const &>;
template class SubDB<Room, std::uint_fast32_t, unsigned>;
template class SubDB<Group, std::uint_fast32_t, unsigned>;
template class SubDB<Time, std::uint_fast32_t, unsigned, unsigned>;

template <typename _Object, typename... Params>
struct make
{
	static void Object(_Object &object, Params... params)
	{
		object = _Object{params...};
	}
};

template <typename _Object>
struct make<_Object, std::string const &>
{
	static void Object(_Object &object, std::string const &name)
	{
		if(name.length() >= _Object::name_len)
			throw DataError("Name too long");
		std::memcpy(object.name, name.data(), name.length());
		object.name[name.length()] = 0;
	}
};

template <typename _Object, typename _Key, typename... Params>
Id SubDB<_Object, _Key, Params...>::add(Params... params)
{
	auto p = data.alloc();
	make<_Object, Params...>::Object(p.second->data, params...);
	index.add(p.first);
	return p.first;
}

template <typename _Object, typename _Key, typename... Params>
Id SubDB<_Object, _Key, Params...>::find(Params... params)
{
	return index[getKey(params...)];
}

template <typename _Object, typename _Key, typename... Params>
Id SubDB<_Object, _Key, Params...>::need(Params... params)
{
	Id *id = index.get(getKey(params...));
	if(id)
		return *id;
	return add(params...);
}
