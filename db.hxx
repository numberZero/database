#pragma once
#include <iostream>
#include "db.th.hxx"
#include "db.tc.hxx"

class RowReference
{
	Database *db;
	Row *row;

public:
	char *getTeacher() const;
	char *getSubject() const;
	int getRoom() const;
	int getGroup() const;
	bool isMetaGroup() const;
	int getDay() const;
	int getLesson() const;
};

class Database
{
	Table<Teacher> teachers;
	Table<Subject> subjects;
	Table<Room> rooms;
	Table<Group> groups;
	Table<Time> times;

	Table<Row> rows;
	void readText(std::string const& filename);
	void readText(std::istream& file);
	void writeText(std::string const& filename);
	void writeText(std::ostream& file);
	void printDB(std::ostream& file, int width = 75);
};
