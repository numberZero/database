#include "bitset.hxx"

Bitset::Bitset()
{
	row_array = nullptr;
	row_capacity = 0;
}

Bitset::~Bitset()
{
}

bool Bitset::get(BitsetIndex x, BitsetIndex y) const
{
	(void)x;
	(void)y;
	return false;
}

void Bitset::set(BitsetIndex x, BitsetIndex y)
{
	(void)x;
	(void)y;
}

void Bitset::clear(BitsetIndex x, BitsetIndex y)
{
	(void)x;
	(void)y;
}