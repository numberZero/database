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
