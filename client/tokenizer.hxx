#pragma once
#include <iosfwd>
#include <set>
#include <stdexcept>
#include <string>
#include <map>
#include "misc.hxx"

NEW_ERROR_CLASS(ReaderError, runtime_error, std);
NEW_ERROR_CLASS(EofError, ReaderError, );
NEW_ERROR_CLASS(InvalidQueryError, ReaderError, );
NEW_ERROR_CLASS(InvalidCharacterError, InvalidQueryError, );

class BaseReader
{
protected:
	static const std::map<char, std::string> escape_sequences;
	std::istream &in;
	char c;

	static std::string const &getEscape(char ch);

	char readChar();
	void readSpace(bool required = false);
	void readEnd();

	bool isEnd();

	std::string readIdent();
	std::string readString();
	std::set<std::string> readParams1();
	std::map<std::string, std::string> readParams2();

	BaseReader(std::istream &stream);

public:
	void skipLine(); // force skip till next newline
};
