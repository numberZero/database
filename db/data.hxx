#pragma once
#include <cstdint>

typedef std::uint32_t Id;
static const Id INVALID_ID = ~(Id)0;

struct Teacher
{
	static constexpr std::size_t const name_len = 128;
	char name[name_len];
};

struct Subject
{
	static constexpr std::size_t const name_len = 128;
	char name[name_len];
};

struct Room
{
	std::uint16_t number;
};

struct Group
{
	std::uint16_t number;
	bool meta;
};

struct Time
{
	std::uint16_t day;
	std::uint16_t lesson;
};

struct Row
{
	Id teacher;
	Id subject;
	Id room;
	Id group;
	Id time;
};
