#include "db.hxx"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <string>
#include <fcntl.h>
#include "fs.hxx"
#include "misc.hxx"

db_temporary_t db_temporary;

Database::Database(std::string const &datadir) :
	SubDB_Teacher(Open(datadir + "/teacher.zolden-table", O_RDWR | O_CREAT), defer_load),
	SubDB_Subject(Open(datadir + "/subject.zolden-table", O_RDWR | O_CREAT), defer_load),
	SubDB_Room(Open(datadir + "/room.zolden-table", O_RDWR | O_CREAT), defer_load),
	SubDB_Group(Open(datadir + "/group.zolden-table", O_RDWR | O_CREAT), defer_load),
	SubDB_Time(Open(datadir + "/time.zolden-table", O_RDWR | O_CREAT), defer_load),
	SubDB_Row(Open(datadir + "/row.zolden-table", O_RDWR | O_CREAT), defer_load),
	temporary(false)
{
	Teachers::load();
	Subjects::load();
	Rooms::load();
	Groups::load();
	Times::load();
	Rows::load();
}

Database::Database(std::string const &datadir, db_temporary_t) :
#ifdef O_TMPFILE
	SubDB_Teacher(Open(datadir, O_RDWR | O_TMPFILE, 0640), defer_load),
	SubDB_Subject(Open(datadir.c_str(), O_RDWR | O_TMPFILE, 0640), defer_load),
	SubDB_Room(Open(datadir.c_str(), O_RDWR | O_TMPFILE, 0640), defer_load),
	SubDB_Group(Open(datadir.c_str(), O_RDWR | O_TMPFILE, 0640), defer_load),
	SubDB_Time(Open(datadir.c_str(), O_RDWR | O_TMPFILE, 0640), defer_load),
	SubDB_Row(Open(datadir.c_str(), O_RDWR | O_TMPFILE, 0640), defer_load)
#else
	Database(datadir)
#endif
{
	temporary = true;
}

void Database::readText(std::string const &filename)
{
	std::ifstream file(filename);
	readText(file);
}

void Database::writeText(std::string const &filename)
{
	std::ofstream file(filename);
	writeText(file);
}

void Database::readTableRowData_teachers(std::istream &file)
{
	Teachers::add(readValue(file, "Name"));
}

void Database::readTableRowData_subjects(std::istream &file)
{
	Subjects::add(readValue(file, "Name"));
}

void Database::readTableRowData_rooms(std::istream &file)
{
	Rooms::add(readInteger(file, "Number"));
}

void Database::readTableRowData_groups(std::istream &file)
{
	long number = readInteger(file, "Number");
	Groups::add(number);
}

void Database::readTableRowData_times(std::istream &file)
{
	long day = readInteger(file, "Day");
	long lesson = readInteger(file, "Lesson");
	Times::add(day, lesson);
}

void Database::readTableRowData_rows(std::istream &file)
{
	Id time = readInteger(file, "Time");
	Id room = readInteger(file, "Room");
	Id subject = readInteger(file, "Subject");
	Id teacher = readInteger(file, "Teacher");
	Id group = readInteger(file, "Group");
	Rows::need(teacher, subject, room, group, time);
}

void Database::readTable(std::istream &file, std::string const &name, void (Database::*reader)(std::istream &file))
{
	std::string key;
	std::string value;
	std::size_t count;
	value = readValue(file, "TABLE");
	if(value != name)
		throw DatabaseFileError("Unexpected table: " + name + " expected, " + value + " found");
	count = readInteger(file, "Count");
	for(std::size_t k = 0; k != count; ++k)
	{
		std::size_t j = readInteger(file, "ROW");
		if(j != k)
			throw DatabaseFileError("Wrong row numeration: row #" + std::to_string(k) + " marked as being #" + std::to_string(j));
		(this->*reader)(file);
	}
}

void Database::readText(std::istream &file)
{
	std::string value;
	std::getline(file, value);
	if(value != "DATABASE")
		throw DatabaseFileError("Database expected");
	value.clear();
#define readTable(file, table) readTable(file, #table, &Database::readTableRowData_ ## table)
	readTable(file,  teachers);
	readTable(file,  subjects);
	readTable(file,  rooms);
	readTable(file,  groups);
	readTable(file,  times);
#undef readTable
	readTable(file,  "entries", &Database::readTableRowData_rows);
	while(value.empty())
		std::getline(file, value);
	if(value != "END")
		throw DatabaseFileError("Database end expected");
}

void writeTableRowData(std::ostream &file, Teacher const &object)
{
	file << "Name " << object.name << "\n";
}

void writeTableRowData(std::ostream &file, Subject const &object)
{
	file << "Name " << object.name << "\n";
}

void writeTableRowData(std::ostream &file, Room const &object)
{
	file << "Number " << object.number << "\n";
}

void writeTableRowData(std::ostream &file, Group const &object)
{
	file << "Number " << object.number << "\n";
}

void writeTableRowData(std::ostream &file, Time const &object)
{
	file << "Day " << object.day << "\n";
	file << "Lesson " << object.lesson << "\n";
}

void writeTableRowData(std::ostream &file, Row const &object)
{
	file << "Time " << object.time << "\n";
	file << "Room " << object.room << "\n";
	file << "Subject " << object.subject << "\n";
	file << "Teacher " << object.teacher << "\n";
	file << "Group " << object.group << "\n";
}

template <typename _Object>
void writeTableRowData(std::ostream &file, _Object const &data)
{
	writeTableRowData(file, data);
}

template <typename _Object>
void writeTable(std::ostream &file, Table<_Object> &table, std::string const &name)
{
	Id n = table.size();
	file << "TABLE " << name << "\n";
	file << "Count " << n << "\n";
	file << "\n";
	Id k = 0;
	for(auto const& item: table)
	{
		file << "ROW " << k++ << "\n";
		writeTableRowData(file, item);
		file << "\n";
	}
	file << std::endl;
	if(k != n)
		throw std::runtime_error("Table writing failed: row number changed");
}

void Database::writeText(std::ostream &file)
{
	file << "DATABASE\n";
	file << "\n";
#define writeTable(file, table) writeTable(file, table, #table)
	writeTable(file, Teachers::table());
	writeTable(file, Subjects::table());
	writeTable(file, Rooms::table());
	writeTable(file, Groups::table());
	writeTable(file, Times::table());
#undef writeTable
	writeTable(file,  Rows::table(), "entries");
	file << "END\n";
	file.flush();
}

void Database::printDB(std::ostream &file, int width)
{
	(void)(width);
	int k = 0;
	for(Row const& prow: Rows::table())
	{
		file << "*** Row " << ++k << " ***\n";
		RowReference row(this, &prow);
		file << "Day: " << row.getDay() << "\n";
		file << "Lesson #" << row.getLesson() << "\n";
		file << "Room #" << row.getRoom() << "\n";
		file << "Subject: " << row.getSubject() << "\n";
		file << "Teacher: " << row.getTeacher() << "\n";
		file << "Group #" << row.getGroup() << "\n";
		file << std::endl; // also flushes the stream
	}
}

void Database::on_add(Id id, Row &row)
{
	Rows::on_add(id, row);
	Teachers::add_row(id, row.teacher);
	Subjects::add_row(id, row.subject);
	Rooms::add_row(id, row.room);
	Groups::add_row(id, row.group);
	Times::add_row(id, row.time);
}

RowReference Database::insert(RowData const &row)
{
	SRXW_WriteLockGuard gurad(lock);
	Id teacher = Teachers::need(row.teacher);
	Id subject = Subjects::need(row.subject);
	Id room = Rooms::need(row.room);
	Id group = Groups::need(row.group);
	Id time = Times::need(row.day, row.lesson);
	return{this, &Rows::table().get(Rows::need(teacher, subject, room, group, time))};
}

Selection Database::select(SelectionParams const &p)
{
	std::unique_ptr<SRXW_ReadLockGuard> guard(new SRXW_ReadLockGuard(lock));
	std::unique_ptr<PreSelection> s;
	Rows &rows = *this;
	if(!p.isValid())
		return Selection();
	if(p.teacher.is_key())
		s.reset(new PreSelection_SimpleKey(rows, SubDB_Teacher::begin(p.teacher.value)));
	else if(p.subject.is_key())
		s.reset(new PreSelection_SimpleKey(rows, SubDB_Subject::begin(p.subject.value)));
	else if(p.room.is_key())
		s.reset(new PreSelection_SimpleKey(rows, SubDB_Room::begin(p.room.min)));
	else if(p.group.is_key())
		s.reset(new PreSelection_SimpleKey(rows, SubDB_Group::begin(p.group.min)));
	else if(p.day.is_key() && p.lesson.is_key())
		s.reset(new PreSelection_SimpleKey(rows, SubDB_Time::begin(p.day.min, p.lesson.min)));
	else
		s.reset(new PreSelection_Full(rows)); // slow but always works
	return Selection(*this, p, std::move(s), std::move(guard));
}

std::size_t Database::remove(SelectionParams const &p)
{
	SRXW_WriteLockGuard gurad(lock);
	std::size_t count = 0;
	std::unique_ptr<PreSelection> s;
	if(!p.isValid())
		return 0;
	s.reset(new PreSelection_Full(*this)); // slow but always works
	for(; s->isValid(); s->next())
	{
		Id id = s->getRowId();
		Row &row = Rows::table().get(id);
		if(!RowReference(this, &row).check(p))
			continue;
		Teachers::remove_row(id, row.teacher);
		Subjects::remove_row(id, row.subject);
		Rooms::remove_row(id, row.room);
		Groups::remove_row(id, row.group);
		Times::remove_row(id, row.time);
		Rows::remove(id); // it always present (unless threads conflict)
		++count;
	}
	return count;
}
