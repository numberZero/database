#include <cerrno>
#include <unistd.h>
#include "packer.hxx"
#include "bio.hxx"

void readBlock(int fd, char *buffer, std::size_t bytes)
{
	while(bytes > 0)
	{
		ssize_t count = read(fd, buffer, bytes);
		if(!count)
			throw BioEof("readBlock() reached end of file");
		if(count < 0)
		{
			if(errno == EINTR)
				continue;
			throw BioError(errno, std::system_category(), "readBlock()");
		}
		bytes -= count;
		buffer += count;
	}
}

void writeBlock(int fd, char const *buffer, std::size_t bytes)
{
	while(bytes > 0)
	{
		ssize_t count = write(fd, buffer, bytes);
		if(count <= 0)
		{
			if(errno == EINTR)
				continue;
			throw BioError(errno, std::system_category(), "writeBlock()");
		}
		bytes -= count;
		buffer += count;
	}
}

static constexpr int const packet_length_size = 4;

void readPacket(int fd, char *&buffer, std::size_t &bytes)
{
	buffer = readPacket(fd, bytes);
}

char *readPacket(int fd, std::size_t &bytes)
{
	char buf[packet_length_size];
	readBlock(fd, buf, packet_length_size);
	packer::type::Integer<std::size_t, packet_length_size>::static_parse(buf, bytes);
	char *buffer = new char[bytes];
	try
	{
		readBlock(fd, buffer, bytes);
	}
	catch(...)
	{
		delete[] buffer;
		throw;
	}
	return buffer;
}

void writePacket(int fd, char const *buffer, std::size_t bytes)
{
	char buf[packet_length_size];
	packer::type::Integer<std::size_t, packet_length_size>::static_serialize(buf, bytes);
	writeBlock(fd, buf, packet_length_size);
	writeBlock(fd, buffer, bytes);
	fsync(fd); // make sure data is actually sent
}
