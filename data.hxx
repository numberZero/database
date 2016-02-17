#pragma once
#include <cstdint>

typedef std::uint32_t Id;

struct Teacher
{
	char name[128];
};

struct Subject
{
	char name[128];
};

struct Room
{
	std::uint16_t number;
};

struct Group
{
	bool meta;
};

struct Time
{
	std::uint16_t day;
	std::uint16_t lesson;
};

struct Row
{
	Id time;
	Id room;
	Id subject;
	Id teacher;
	Id group;
};
