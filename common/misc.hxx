#pragma once
#include <ios>
#include <stdexcept>
#include <string>

class ReadError:
	public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

std::string upcase(std::string const& str);
void upcase_it(std::string& str);

std::string readValue(std::istream& file, std::string const& key);
long readInteger(std::istream& file, std::string const& key);
bool readBoolean(std::istream& file, std::string const& key);

bool parseBoolean(std::string const& text);
