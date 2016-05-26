#pragma once
#include "data.hxx"
#include "dbcommon.hxx"
#include "dbhelper.hxx"
#include "hashtable.hxx"
#include "select.hxx"
#include "srxw.hxx"
#include "subdb_string.hxx"
#include "subdb_struct.hxx"
#include "table.hxx"

extern struct db_temporary_t {} db_temporary;

class Database:
	public SubDB_Teacher,
	public SubDB_Subject,
	public SubDB_Room,
	public SubDB_Group,
	public SubDB_Time,
	public SubDB_Row
{
	friend class RowReference;
	friend class Selection;

private:
	bool temporary;

	void on_add(Id index, Row &item) override;

	void readTableRowData_teachers(std::istream &file);
	void readTableRowData_subjects(std::istream &file);
	void readTableRowData_rooms(std::istream &file);
	void readTableRowData_groups(std::istream &file);
	void readTableRowData_times(std::istream &file);
	void readTableRowData_rows(std::istream &file);
	void readTable(std::istream &file, std::string const &name, void (Database::*reader)(std::istream &file));

	SRXWLock lock;

public:
	Database(std::string const& datadir);
	Database(std::string const& datadir, db_temporary_t);
	Id addRow(Id teacher, Id subject, Id room, Id group, Id time);

	void readText(std::string const &filename);
	void readText(std::istream &file);
	void writeText(std::string const &filename);
	void writeText(std::ostream &file);
	void printDB(std::ostream &file, int width = 75);

	RowReference insert(RowData const &row);
	Selection select(SelectionParams const &p);
	std::size_t remove(SelectionParams const &p);
};

extern std::unique_ptr<Database> db;
