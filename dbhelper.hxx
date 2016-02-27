#pragma once
#include <istream>
#include <string>
#include "data.hxx"

template <typename _Object>
struct Reference
{
	Id id;
	_Object *obj;
};

typedef Reference<Teacher> TeacherRef;
typedef Reference<Subject> SubjectRef;
typedef Reference<Room> RoomRef;
typedef Reference<Group> GroupRef;
typedef Reference<Time> TimeRef;

char const *getTeacherName(TeacherRef const& object);
char const *getSubjectName(SubjectRef const& object);
std::uint_fast32_t getRoomKey(RoomRef const& object);
std::uint_fast32_t getGroupKey(GroupRef const& object);
std::uint_fast32_t getTimeKey(TimeRef const& object);

std::uint_fast32_t getRoomKey(std::uint16_t number);
std::uint_fast32_t getGroupKey(std::uint16_t number, bool meta);
std::uint_fast32_t getTimeKey(std::uint16_t day, std::uint16_t lesson);

std::string readValue(std::istream& file, std::string const& key);
long readInteger(std::istream& file, std::string const& key);
bool readBoolean(std::istream& file, std::string const& key);
