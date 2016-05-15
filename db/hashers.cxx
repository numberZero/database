#include <cstring>
#include "hashers.hxx"

std::size_t hash_bytes(void const *data, std::size_t count)
{
	char const *key(reinterpret_cast<char const *>(data));
	std::size_t value = 0;
	for(std::size_t k = 0; k != count; ++k)
	{
		value += *key;
		value ^= (value << 3) | ((value >> 7) & 7);
		++key;
	}
	return value;
}

std::size_t hash_string(char const *data)
{
	char const *key(data);
	std::size_t value = 0;
	while(*key)
	{
		value += *key;
		value ^= (value << 3) | ((value >> 7) & 7);
		++key;
	}
	return value;
}
