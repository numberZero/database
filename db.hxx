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

class DatabaseError:
	public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class DataError:
	public DatabaseError
{
public:
	using DatabaseError::DatabaseError;
};

class DatabaseFileError:
	public DatabaseError
{
public:
	using DatabaseError::DatabaseError;
};

class Database
{
	Table<Teacher> teachers;
	Table<Subject> subjects;
	Table<Room> rooms;
	Table<Group> groups;
	Table<Time> times;

	Table<Row> rows;

	void readTableRowData_teachers(std::istream& file);
	void readTableRowData_subjects(std::istream& file);
	void readTableRowData_rooms(std::istream& file);
	void readTableRowData_groups(std::istream& file);
	void readTableRowData_times(std::istream& file);
	void readTableRowData_rows(std::istream& file);
	void readTable(std::istream& file, std::string const& name, void (Database::*reader)(std::istream& file));

public:
	Id addTeacher(std::string const& name);
	Id addSubject(std::string const& name);
	Id addRoom(unsigned number);
	Id addGroup(unsigned number, bool meta = false);
	Id addTime(unsigned day, unsigned lesson);
	Id addRow(Id time, Id room, Id subject, Id teacher, Id group);;

	void readText(std::string const& filename);
	void readText(std::istream& file);
	void writeText(std::string const& filename);
	void writeText(std::ostream& file);
	void printDB(std::ostream& file, int width = 75);

	void clear();
};
