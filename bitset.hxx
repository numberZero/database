#pragma once
#include <cstdint>

static const int bits_x = 64;
static const int bits_y = 64;
typedef std::uint64_t BitfieldItem;
typedef std::uint_fast8_t BitfieldSubindex;
typedef std::uint_fast16_t BitfieldIndex;

class Bitfield;

struct SBitfieldBlock
{
	BitfieldItem data[bits_y];
};

struct SBitfieldRow
{
	SBitfieldBlock *block_array;
	long block_capacity;
};

struct SBitfieldPointer
{
	SBitfieldBlock *block;
	BitfieldSubindex sx, sy;
};

struct SBitfieldPointerEx: SBitfieldPointer
{
	Bitfield *bf;
	BitfieldIndex x, y;
};

class Bitfield
{
	SBitfieldRow *row_array;
	long row_capacity;

public:
	Bitfield();
	~Bitfield(); 
	bool get(BitfieldIndex x, BitfieldIndex y);
	void set(BitfieldIndex x, BitfieldIndex y);
	void clear(BitfieldIndex x, BitfieldIndex y);
};
