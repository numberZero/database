#pragma once
#include <atomic>
#include <string>
#include "data.hxx"
#include "struct.hxx"

class Database;

template <typename _Object>
struct Table;

typedef Table<Row> Rows;

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

	bool check(SelectionParams const &sp) const;
};

char const *getKey(Teacher const &object);
char const *getKey(Subject const &object);
std::uint_fast32_t getKey(Room const &object);
std::uint_fast32_t getKey(Group const &object);
std::uint_fast32_t getKey(Time const &object);

char const *getKey(std::string name);
std::uint_fast32_t getKey(std::uint16_t number);
std::uint_fast32_t getKey(std::uint16_t day, std::uint16_t lesson);
