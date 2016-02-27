#pragma once
#include <istream>
#include <string>
#include "data.hxx"

struct RowRefList
{
	static const int node_capacity = 16;

	struct Node
	{
		Row *rows[node_capacity];
		Node *next;
	};

	Node *head = nullptr;

	RowRefList() = default;
	~RowRefList();
	void addRow(Row *row);
};

template <typename _Data>
struct Container
{
	_Data data;
	RowRefList rows;
	void addRow(Row *row)
	{
		rows.addRow(row);
	}
};

char const *getKey(Teacher const& object);
char const *getKey(Subject const& object);
std::uint_fast32_t getKey(Room const& object);
std::uint_fast32_t getKey(Group const& object);
std::uint_fast32_t getKey(Time const& object);

std::uint_fast32_t getRoomKey(std::uint16_t number);
std::uint_fast32_t getGroupKey(std::uint16_t number, bool meta);
std::uint_fast32_t getTimeKey(std::uint16_t day, std::uint16_t lesson);

std::string readValue(std::istream& file, std::string const& key);
long readInteger(std::istream& file, std::string const& key);
bool readBoolean(std::istream& file, std::string const& key);
