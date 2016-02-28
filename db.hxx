#pragma once
#include <iostream>
#include "dbcommon.hxx"
#include "table.hxx"
#include "select.hxx"
#include "bitset.hxx"
#include "hashtable.hxx"
#include "dbhelper.hxx"
#include "dbtables.hxx"

class Database:
	public SubDB_Teacher,
	public SubDB_Subject,
	public SubDB_Room,
	public SubDB_Group,
	public SubDB_Time
{
	friend class RowReference;

private:
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
	Id addRow(Id time, Id room, Id subject, Id teacher, Id group);

	void readText(std::string const& filename);
	void readText(std::istream& file);
	void writeText(std::string const& filename);
	void writeText(std::ostream& file);
	void printDB(std::ostream& file, int width = 75);

	void clear();
};
