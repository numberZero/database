#pragma once
#include <cstdint>

typedef std::uint32_t Id;

struct Teacher
{
	static const std::size_t name_len = 128;
	char name[name_len];
};

struct Subject
{
	static const std::size_t name_len = 128;
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
