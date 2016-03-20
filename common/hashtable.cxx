#include "hashtable.hxx"

template<>
bool ht_equal<char const *>(char const *key1, char const *key2)
{
	return !std::strcmp(key1, key2);
}

template<>
bool ht_equal<std::uint16_t>(std::uint16_t key1, std::uint16_t key2)
{
	return key1 == key2;
}

template<>
bool ht_equal<std::uint32_t>(std::uint32_t key1, std::uint32_t key2)
{
	return key1 == key2;
}

template<>
bool ht_equal<std::uint64_t>(std::uint64_t key1, std::uint64_t key2)
{
	return key1 == key2;
}

template<>
std::size_t ht_hash<char const *>(const char *key)
{
	std::size_t value = 0;
	while(*key)
	{
		value += *key;
		value ^= (value << 3) | ((value >> 7) & 7);
		++key;
	}
	return value;
}

template<>
std::size_t ht_hash<std::uint16_t>(std::uint16_t key)
{
	return key;
}

template<>
std::size_t ht_hash<std::uint32_t>(std::uint32_t key)
{
	return key;
}

template<>
std::size_t ht_hash<std::uint64_t>(std::uint64_t key)
{
	return key;
}
