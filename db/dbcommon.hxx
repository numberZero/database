#pragma once
#include <stdexcept>
#include "misc.hxx"

NEW_ERROR_CLASS(DatabaseLogicError, logic_error, std);
NEW_ERROR_CLASS(DatabaseError, runtime_error, std);
NEW_ERROR_CLASS(DataError, DatabaseError, );

using DatabaseFileError = ReadError;
