#pragma once
#include <iosfwd>
#include <set>
#include <stdexcept>
#include <string>
#include <map>

class BaseReader
{
protected:
	typedef std::runtime_error error;
	static const std::map<char, std::string> escape_sequences;
	std::istream& in;
	char c;

	static std::string const& getEscape(char ch);

	char readChar();
	void readSpace(bool required = false);
	void readEnd();

	bool isEnd();

	std::string readIdent();
	std::string readString();
	std::set<std::string> readParams1();
	std::map<std::string, std::string> readParams2();

	BaseReader(std::istream& stream);
};
