#pragma once
#include <cstdint>

typedef std::uint32_t Id;
static const Id INVALID_ID = ~(Id)0;

struct [[gnu::packed]] Teacher
{
	static constexpr std::size_t const name_len = 128;
	char name[name_len];
};

struct [[gnu::packed]] Subject
{
	static constexpr std::size_t const name_len = 128;
	char name[name_len];
};

struct [[gnu::packed]] Room
{
	std::uint16_t number;
};

struct [[gnu::packed]] Group
{
	std::uint16_t number;
};

struct [[gnu::packed]] Time
{
	std::uint16_t day;
	std::uint16_t lesson;
};

struct [[gnu::packed]] Row
{
	Id teacher;
	Id subject;
	Id room;
	Id group;
	Id time;
};
