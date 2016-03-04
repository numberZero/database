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

std::string const& BaseReader::getEscape(char ch)
{
	return escape_sequences.at(ch);
}

char BaseReader::readChar()
{
	c = std::char_traits<char>::not_eof(in.get());
	if(!c)
		throw error("Unexpected end of input sequence");
	return c;
}

void BaseReader::readSpace(bool required)
{
	if(required && !std::isspace(c))
		throw error("Space expected");
	while(std::isspace(c))
		readChar();
}

void BaseReader::readEnd()
{
	if(!isEnd())
		throw error("Query end expected");
}

bool BaseReader::isEnd()
{
	return c == ';';
}

std::string BaseReader::readIdent()
{
	std::stringstream result;
	if(!std::isalpha(c))
		throw error("Identifier expected");
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
			throw error("Parameter already present: " + name);
		readSpace();
		if(c == ';')
			break;
		if(c != ',')
			throw error("Comma expected");
		readSpace();
	}
	return std::move(params);
}

std::map<std::string, std::string> BaseReader::readParams2()
{
	std::map<std::string, std::string> params;
	for(;;)
	{
		std::string key = readIdent();
		readSpace();
		if(c != '=')
			throw error("\"=\" expected");
		readChar();
		readSpace();
		std::string value = readString();
		if(!params.emplace(key, value).second)
			throw error("Parameter already present: " + key);
		readSpace();
		if(c == ';')
			break;
		if(c != ',')
			throw error("Comma expected");
		readChar();
		readSpace();
	}
	return std::move(params);
}

BaseReader::BaseReader(std::istream& stream) :
	in(stream)
{
}
