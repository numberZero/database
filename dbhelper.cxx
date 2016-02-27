#include "dbhelper.hxx"
#include "dbcommon.hxx"

char const *getKey(Teacher const& object)
{
	return object.name;
}

char const *getKey(Subject const& object)
{
	return object.name;
}

uint_fast32_t getKey(Room const& object)
{
	return getRoomKey(object.number);
}

uint_fast32_t getKey(Group const& object)
{
	return getGroupKey(object.number, object.meta);
}

uint_fast32_t getKey(Time const& object)
{
	return getTimeKey(object.day, object.lesson);
}

uint_fast32_t getRoomKey(std::uint16_t number)
{
	return number;
}

uint_fast32_t getGroupKey(std::uint16_t number, bool meta)
{
	return (meta ? 0x00010000 : 0x00000000) | number;
}

uint_fast32_t getTimeKey(std::uint16_t day, std::uint16_t lesson)
{
	return (day << 16) | lesson;
}

std::string readValue(std::istream& file, std::string const& key)
{
	static char const *space = "\t ";
	std::string line;
	std::string str;
	while(line.empty())
		std::getline(file, line);
	auto pos = line.find_first_of(space);
	if(pos == std::string::npos)
		throw DatabaseFileError("Key-value pair expected");
	auto p2 = line.find_first_not_of(space, pos);
	str = line.substr(0, pos);
	if(str != key)
		throw DatabaseFileError("Invalid key: " + key + " expected, " + str + " found");
	return line.substr(p2);
}

long readInteger(std::istream& file, std::string const& key)
{
	return std::stoi(readValue(file, key));
}

bool readBoolean(std::istream& file, std::string const& key)
{
	std::string value = readValue(file, key);
	std::string str = value;
	for (auto & c: str)
		c = std::toupper(c);
	if((str == "Y") || (str == "T") || (str == "1") || (str == "YES") || (str == "TRUE"))
		return true;
	if((str == "N") || (str == "F") || (str == "0") || (str == "NO") || (str == "FALSE"))
		return false;
	throw DatabaseFileError("Boolean value expected, " + value + " found");
}
