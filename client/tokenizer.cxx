#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include "tokenizer.hxx"

const std::map<char, std::string> BaseReader::escape_sequences = {
	{ 'r', "" }, // ignore W-style \r\n... and some invalid data as well
	{ 't', "\t" },
	{ 'n', "\n" },
	{ ' ', " " },
	{ '"', "\"" },
};

std::string const &BaseReader::getEscape(char ch)
{
	return escape_sequences.at(ch);
}

char BaseReader::readChar()
{
	std::char_traits<char>::int_type ch = in.get();
	if(ch == std::char_traits<char>::eof())
		throw EofError("Unexpected end of input sequence");
	c = ch;
	return c;
}

void BaseReader::readSpace(bool required)
{
	if(required && !std::isspace(c))
		throw InvalidCharacterError("Space expected");
	while(std::isspace(c))
		readChar();
}

void BaseReader::ensureEnd()
{
	if(!isEnd())
		throw InvalidCharacterError("Query end expected");
}

void BaseReader::ensureChar(char expected)
{
	if(c != expected)
		throw InvalidCharacterError(std::string() + "\"" + expected + "\" expected");
	readChar();
}

bool BaseReader::isEnd()
{
	return c == ';';
}

std::string BaseReader::readIdent()
{
	std::stringstream result;
	if(!std::isalpha(c))
		throw InvalidCharacterError("Identifier expected");
	result.put(c);
	for(readChar(); std::isalnum(c); readChar())
		result.put(c);
	return result.str();
}

std::string BaseReader::readString()
{
	std::stringstream result;
	if(c == '"') // we support quoted strings
	{
		for(readChar(); c != '"'; readChar())
			if(c == '\\') // we DO support escape sequences
				result << getEscape(readChar());
			else
				result.put(c);
		readChar();
	}
	else
		for(; std::isalnum(c) || (c == '-') || (c == '.') || (c == '\\'); readChar())
			if(c == '\\')
				result << getEscape(readChar());
			else
				result.put(c);
	return result.str();
}

std::set<std::string> BaseReader::readParams1()
{
	std::set<std::string> params;
	for(;;)
	{
		std::string name = readIdent();
		if(!params.insert(name).second)
			throw InvalidQueryError("Parameter already present: " + name);
		readSpace();
		if(isEnd())
			break;
		ensureChar(',');
		readSpace();
	}
	return std::move(params);
}

std::map<std::string, std::string> BaseReader::readParams2()
{
	std::map<std::string, std::string> params;
	if(isEnd())
		return std::move(params);
	for(;;)
	{
		std::string key = readIdent();
		readSpace();
		ensureChar('=');
		readSpace();
		std::string value = readString();
		if(!params.emplace(key, value).second)
			throw InvalidQueryError("Parameter already present: " + key);
		readSpace();
		if(isEnd())
			break;
		ensureChar(',');
		readSpace();
	}
	return std::move(params);
}

BaseReader::BaseReader(std::istream &stream) :
	in(stream)
{
}

void BaseReader::skipLine()
{
	while(readChar() != '\n');
}
