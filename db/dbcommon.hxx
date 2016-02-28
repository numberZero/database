#pragma once
#include <stdexcept>

class DatabaseLogicError:
	public std::logic_error
{
public:
	using std::logic_error::logic_error;
};

class DatabaseError:
	public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

class DataError:
	public DatabaseError
{
public:
	using DatabaseError::DatabaseError;
};

class DatabaseFileError:
	public DatabaseError
{
public:
	using DatabaseError::DatabaseError;
};
