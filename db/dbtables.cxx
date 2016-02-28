#include "dbtables.hxx"

Id SubDB_Teacher::addTeacher(std::string const& name)
{
	if(name.length() >= Teacher::name_len)
		throw DataError("Teacher name too long");
	auto p = teachers.add();
	Id id = p.first;
	Teacher &teacher = p.second->data;
	std::memcpy(teacher.name, name.data(), name.length());
	teacher.name[name.length()] = 0;
	index_teacher.add(id);
	return id;
}

Id SubDB_Subject::addSubject(std::string const& name)
{
	if(name.length() >= Subject::name_len)
		throw DataError("Subject name too long");
	auto p = subjects.add();
	Id id = p.first;
	Subject &subject = p.second->data;
	std::memcpy(subject.name, name.data(), name.length());
	subject.name[name.length()] = 0;
	index_subject.add(id);
	return id;
}

Id SubDB_Room::addRoom(unsigned number)
{
	Id id = rooms.add(Room{(std::uint16_t)number});
	index_room.add(id);
	return id;
}

Id SubDB_Group::addGroup(unsigned int number, bool meta)
{
	Id id = groups.add(Group{(std::uint16_t)number, meta});
	index_group.add(id);
	return id;
}

Id SubDB_Time::addTime(unsigned day, unsigned lesson)
{
	Id id = times.add(Time{(std::uint16_t)day, (std::uint16_t)lesson});
	index_time.add(id);
	return id;
}

Id SubDB_Teacher::findTeacher(std::string const& name)
{
	return index_teacher[name.c_str()];
}

Id SubDB_Subject::findSubject(std::string const& name)
{
	return index_subject[name.c_str()];
}

Id SubDB_Room::findRoom(unsigned number)
{
	return index_room[getRoomKey(number)];
}

Id SubDB_Group::findGroup(unsigned number, bool meta)
{
	return index_group[getGroupKey(number, meta)];
}

Id SubDB_Time::findTime(unsigned day, unsigned lesson)
{
	return index_time[getTimeKey(day, lesson)];
}
