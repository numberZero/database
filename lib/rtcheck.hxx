#pragma once
#include <system_error>

#define syserror(message) \
	throw std::system_error( \
		errno, \
		std::system_category(), \
		std::string() + "Error in\n" + __PRETTY_FUNCTION__ + "\n" + message \
	)

#define syserror_throwif(fail,message) \
	if(fail) \
		syserror(message)

#define syserror_ensure(assertion) \
	syserror_throwif(!(assertion), "Operation:\n" #assertion)

#define runtime_assert(assertion,message) \
	if(!(assertion)) \
		throw std::runtime_error(std::string() + "Runtime assertion failed.\n" \
			"Function:\n" + __PRETTY_FUNCTION__ + "\n" \
			"Operation:\n" #assertion "\n" \
			"Message:\n" + message \
		)
