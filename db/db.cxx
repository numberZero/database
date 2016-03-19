#include "db.hxx"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <string>
#include "misc.hxx"

void Database::readText(std::string const& filename)
{
	std::ifstream file(filename);
	readText(file);
}

void Database::writeText(std::string const& filename)
{
	std::ofstream file(filename);
	writeText(file);
}

void Database::readTableRowData_teachers(std::istream& file)
{
	addTeacher(readValue(file, "Name"));
}

void Database::readTableRowData_subjects(std::istream& file)
{
	addSubject(readValue(file, "Name"));
}

void Database::readTableRowData_rooms(std::istream& file)
{
	addRoom(readInteger(file, "Number"));
}

void Database::readTableRowData_groups(std::istream& file)
{
	long number = readInteger(file, "Number");
	addGroup(number);
}

void Database::readTableRowData_times(std::istream& file)
{
	long day = readInteger(file, "Day");
	long lesson = readInteger(file, "Lesson");
	addTime(day, lesson);
}

void Database::readTableRowData_rows(std::istream& file)
{
	Id time = readInteger(file, "Time");
	Id room = readInteger(file, "Room");
	Id subject = readInteger(file, "Subject");
	Id teacher = readInteger(file, "Teacher");
	Id group = readInteger(file, "Group");
	addRow(teacher, subject, room, group, time);
}

void Database::readTable(std::istream& file, std::string const& name, void (Database::*reader)(std::istream& file))
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

void Database::readText(std::istream& file)
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

void writeTableRowData(std::ostream& file, Teacher const& object)
{
	file << "Name " << object.name << "\n";
}

void writeTableRowData(std::ostream& file, Subject const& object)
{
	file << "Name " << object.name << "\n";
}

void writeTableRowData(std::ostream& file, Room const& object)
{
	file << "Number " << object.number << "\n";
}

void writeTableRowData(std::ostream& file, Group const& object)
{
	file << "Number " << object.number << "\n";
}

void writeTableRowData(std::ostream& file, Time const& object)
{
	file << "Day " << object.day << "\n";
	file << "Lesson " << object.lesson << "\n";
}

void writeTableRowData(std::ostream& file, Row const& object)
{
	file << "Time " << object.time << "\n";
	file << "Room " << object.room << "\n";
	file << "Subject " << object.subject << "\n";
	file << "Teacher " << object.teacher << "\n";
	file << "Group " << object.group << "\n";
}

template <typename _Object>
void writeTableRowData(std::ostream& file, Container<_Object> const& container)
{
	writeTableRowData(file, container.data);
}

template <typename _Object>
void writeTable(std::ostream& file, Table<_Object>& table, std::string const& name)
{
	file << "TABLE " << name << "\n";
	file << "Count " << table.count << "\n";
	file << "\n";
	for(Id k = 0; k != table.count; ++k)
	{
		file << "ROW " << k << "\n";
		writeTableRowData(file, table[k]);
		file << "\n";
	}
	file << std::endl;
}

void Database::writeText(std::ostream& file)
{
	file << "DATABASE\n";
	file << "\n";
#define writeTable(file, table) writeTable(file, table, #table)
	writeTable(file,  teachers);
	writeTable(file,  subjects);
	writeTable(file,  rooms);
	writeTable(file,  groups);
	writeTable(file,  times);
#undef writeTable
	writeTable(file,  rows, "entries");
	file << "END\n";
	file.flush();
}

void Database::printDB(std::ostream& file, int width)
{
	(void)(width);
	for(std::size_t k = 0; k != rows.count; ++k)
	{
		file << "*** Row " << k << " ***\n";
		RowReference row(this, &rows[k]);
		file << "Day: " << row.getDay() << "\n";
		file << "Lesson #" << row.getLesson() << "\n";
		file << "Room #" << row.getRoom() << "\n";
		file << "Subject: " << row.getSubject() << "\n";
		file << "Teacher: " << row.getTeacher() << "\n";
		file << "Group #" << row.getGroup() << "\n";
		file << std::endl; // also flushes the stream
	}
}

Id Database::addRow(Id teacher, Id subject, Id room, Id group, Id time)
{
	Id id = rows.add(Row{teacher, subject, room, group, time});
	Row *row = &rows[id];

	teachers[teacher].addRow(row);
	subjects[subject].addRow(row);
	rooms[room].addRow(row);
	groups[group].addRow(row);
	times[time].addRow(row);

	index_teacher_subject.set(teacher, subject);
	index_teacher_room.set(teacher, room);
	index_teacher_group.set(teacher, group);
	index_teacher_time.set(teacher, time);

	index_subject_room.set(subject, room);
	index_subject_group.set(subject, group);
	index_subject_time.set(subject, time);

	index_room_group.set(room, group);
	index_room_time.set(room, time);

	index_group_time.set(group, time);

	return id;
}
