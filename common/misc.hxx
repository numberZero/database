#pragma once
#include <ios>
#include <stdexcept>
#include <string>

#define NEW_ERROR_CLASS(name,base,basens)  \
	struct name: basens::base \
	{ \
		using basens::base::base; \
	}

NEW_ERROR_CLASS(ReadError, runtime_error, std);

std::string upcase(std::string const& str);
void upcase_it(std::string& str);

std::string locase(std::string const& str);
void locase_it(std::string& str);

std::string trim(std::string const& str, std::string const& delimiters = " \f\n\r\t\v");
void trim_it(std::string& str, std::string const& delimiters = " \f\n\r\t\v");

std::string readValue(std::istream& file, std::string const& key);
long readInteger(std::istream& file, std::string const& key);
bool readBoolean(std::istream& file, std::string const& key);

bool parseBoolean(std::string const& text);
