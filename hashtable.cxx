#include "hashtable.hxx"

std::size_t hashString(const char* key)
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
