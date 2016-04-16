#pragma once
#include <string>
#include "data.hxx"

class Database;

template <typename _Object>
struct Table;

struct Bitset;

struct RowRefList
{
	static const int node_capacity = 16;

	struct Node
	{
		Id rows[node_capacity];
		Node *next;
	};

	Node *head = nullptr;
	std::size_t count = 0;

	RowRefList() = default;
	~RowRefList();
	void addRow(Id row);
};

template <typename _Data>
struct Container
{
	_Data data;
	RowRefList rows;

	Container(_Data &&contents) :
		data(contents)
	{
	}

	void addRow(Id row)
	{
		rows.addRow(row);
	}
};

typedef Table<Row> Rows;

struct RowData
{
	std::string teacher;
	std::string subject;
	unsigned room;
	unsigned group;
	unsigned day;
	unsigned lesson;
};

class RowReference
{
	Database const *db;
	Row const *row;

public:
	RowReference(Database const *database, Row const *prow);

	char const *getTeacher() const;
	char const *getSubject() const;
	unsigned getRoom() const;
	unsigned getGroup() const;
	unsigned getDay() const;
	unsigned getLesson() const;

	RowData getData() const;
};

char const *getKey(Teacher const &object);
char const *getKey(Subject const &object);
std::uint_fast32_t getKey(Room const &object);
std::uint_fast32_t getKey(Group const &object);
std::uint_fast32_t getKey(Time const &object);

std::uint_fast32_t getRoomKey(std::uint16_t number);
std::uint_fast32_t getGroupKey(std::uint16_t number);
std::uint_fast32_t getTimeKey(std::uint16_t day, std::uint16_t lesson);
