#include "db.hxx"
#include <cstring>
#include <algorithm>
#include <fstream>
#include <string>

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
	bool meta = readBoolean(file, "Meta");
	addGroup(number, meta);
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
	addRow(time, room, subject, teacher, group);
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
	clear();
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
	file << "Meta " << (object.meta ? "YES" : "NO") << "\n";
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
	for(long k = 0; k != table.count; ++k)
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
}

void Database::printDB(std::ostream& file, int width)
{
	(void)(width);
	for(std::size_t k = 0; k != rows.count; ++k)
	{
		file << "*** Row " << k << " ***\n";
		file << "Day: " << times[rows[k].time].data.day << "\n";
		file << "Lesson #" << times[rows[k].time].data.lesson << "\n";
		file << "Room #" << rooms[rows[k].room].data.number << "\n";
		file << "Subject: " << subjects[rows[k].subject].data.name << "\n";
		file << "Teacher: " << teachers[rows[k].teacher].data.name << "\n";
		if(groups[rows[k].group].data.meta)
			file << "Metagroup #";
		else
			file << "Group #";
		file << groups[rows[k].group].data.number << "\n";
		file << std::endl; // also flushes the stream
	}
}

Id Database::addRow(Id time, Id room, Id subject, Id teacher, Id group)
{
	Id id = rows.add(Row{time, room, subject, teacher, group});

// 	index_teacher[teacher].

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
}

void Database::clear()
{
	teachers.clear();
	subjects.clear();
	rooms.clear();
	groups.clear();
	times.clear();
	rows.clear();
}
