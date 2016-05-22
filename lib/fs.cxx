#include "fs.hxx"
#include <unistd.h>
#include "rtcheck.hxx"

File Open(std::string const &pathname, int flags, mode_t mode)
{
	File f(Open(pathname, flags, mode, std::nothrow));
	syserror_throwif(!f, "Can't open file " + pathname);
	return std::move(f);
}

File Open(std::string const &pathname, int flags, mode_t mode, std::nothrow_t)
{
	return File(open(pathname.c_str(), flags, mode));
}

File OpenAt(File &directory, std::string const &pathname, int flags, mode_t mode)
{
	File f(OpenAt(directory, pathname, flags, mode, std::nothrow));
	syserror_throwif(!f, "Can't open file " + pathname);
	return std::move(f);
}

File OpenAt(File &directory, std::string const &pathname, int flags, mode_t mode, std::nothrow_t)
{
	return File(openat(directory.get(), pathname.c_str(), flags, mode));
}
