#include <iostream>
#include "misc.hxx"

std::string upcase(std::string const& str)
{
	std::string result(str);
	upcase_it(result);
	return std::move(result);
}

void upcase_it(std::string& str)
{
	for (char & c: str)
		c = std::toupper(c);
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
		throw ReadError("Key-value pair expected");
	auto p2 = line.find_first_not_of(space, pos);
	str = line.substr(0, pos);
	if(str != key)
		throw ReadError("Invalid key: " + key + " expected, " + str + " found");
	return line.substr(p2);
}

long readInteger(std::istream& file, std::string const& key)
{
	return std::stoi(readValue(file, key));
}

bool readBoolean(std::istream& file, std::string const& key)
{
	return parseBoolean(readValue(file, key));
}

bool parseBoolean(std::string const& text)
{
	std::string str = upcase(text);
	if((str == "Y") || (str == "T") || (str == "1") || (str == "YES") || (str == "TRUE"))
		return true;
	if((str == "N") || (str == "F") || (str == "0") || (str == "NO") || (str == "FALSE"))
		return false;
	throw ReadError("Boolean value expected, " + text + " found");
}
