#pragma once
#include <cstdint>

static const int bits_x = 64;
static const int bits_y = 64;
typedef std::uint64_t BitsetItem;
typedef std::uint_fast8_t BitsetSubindex;
typedef std::uint_fast16_t BitsetIndex;

class Bitset;

struct SBitsetBlock
{
	BitsetItem data[bits_y];
};

struct SBitsetRow
{
	SBitsetBlock *block_array;
	long block_capacity;
};

struct SBitsetPointer
{
	SBitsetBlock *block;
	BitsetSubindex sx, sy;
};

struct SBitsetPointerEx: SBitsetPointer
{
	Bitset *bf;
	BitsetIndex x, y;
};

class Bitset
{
	SBitsetRow *row_array;
	long row_capacity;

public:
	Bitset();
	~Bitset();
	bool get(BitsetIndex x, BitsetIndex y) const;
	void set(BitsetIndex x, BitsetIndex y);
	void clear(BitsetIndex x, BitsetIndex y);
};
