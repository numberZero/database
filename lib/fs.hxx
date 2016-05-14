#pragma once
#include <string>
#include <fcntl.h>
#include "file.hxx"

File Open(std::string const &pathname, int flags, mode_t mode = 0644);
File Open(std::string const &pathname, int flags, mode_t mode, std::nothrow_t);

File OpenAt(File &directory, std::string const &pathname, int flags, mode_t mode = 0777);
File OpenAt(File &directory, std::string const &pathname, int flags, mode_t mode, std::nothrow_t);
