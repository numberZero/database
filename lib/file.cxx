#include <utility>
#include <fcntl.h>
#include <unistd.h>
#include "file.hxx"

File::File(File &&b) noexcept
{
	std::swap(fd, b.fd); // this->fd is already initialized with (-1)
}

File::File(int s) noexcept
{
	fd = s;
}

File::~File()
{
	reset(-1);
}

File::operator bool() const noexcept
{
	return !!*this;
}

File::operator int() const noexcept
{
	return get();
}

File &File::operator = (File &&b) noexcept
{
	std::swap(fd, b.fd);
	return *this;
}

bool File::operator ! () const noexcept
{
	return fd < 0;
}

int File::get() const noexcept
{
	return fd;
}

void File::reset(int s)
{
	if(fd)
		close(fd);
	fd = s;
}

int File::release() noexcept
{
	int s = fd;
	fd = -1;
	return s;
}
