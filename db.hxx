#pragma once
#include <iostream>
#include "db.th.hxx"
#include "db.tc.hxx"
#include "select.hxx"
#include "bitset.hxx"

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

	Bitset index_teacher_subject;
	Bitset index_teacher_room;
	Bitset index_teacher_group;
	Bitset index_teacher_time;

	Bitset index_subject_room;
	Bitset index_subject_group;
	Bitset index_subject_time;

	Bitset index_room_group;
	Bitset index_room_time;

	Bitset index_group_time;

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
	Id addRow(Id time, Id room, Id subject, Id teacher, Id group);

	void readText(std::string const& filename);
	void readText(std::istream& file);
	void writeText(std::string const& filename);
	void writeText(std::ostream& file);
	void printDB(std::ostream& file, int width = 75);

	void clear();
};
