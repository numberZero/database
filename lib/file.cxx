#include <new>
#include <utility>
#include <fcntl.h>
#include <unistd.h>
#include "file.hxx"
#include "rtcheck.hxx"

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
	return fd == -1;
}

int File::get() const noexcept
{
	return fd;
}

void File::reset(int s)
{
	if(fd != -1)
		close(fd);
	fd = s;
}

int File::release() noexcept
{
	int s = fd;
	fd = -1;
	return s;
}

std::size_t Read(File &file, void *buffer, std::size_t bytes)
{
	ssize_t result(Read(file, buffer, bytes, std::nothrow));
	syserror_throwif(result < 0, "Read failed");
	return result;
}

ssize_t Read(File &file, void *buffer, std::size_t bytes, std::nothrow_t)
{
	return read(file.get(), buffer, bytes);
}

std::size_t Write(File &file, void const *buffer, std::size_t bytes)
{
	ssize_t result(Write(file, buffer, bytes, std::nothrow));
	syserror_throwif(result < 0, "Write failed");
	return result;
}

ssize_t Write(File &file, void const *buffer, std::size_t bytes, std::nothrow_t)
{
	return write(file.get(), buffer, bytes);
}

void Sync(File &file)
{
	syserror_throwif(!Sync(file, std::nothrow), "Sync failed");
}

bool Sync(File &file, std::nothrow_t)
{
	return !fsync(file.get());
}
