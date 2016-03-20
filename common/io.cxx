#include <cerrno>
#include <unistd.h>
#include "io.hxx"
#include "binary/message.hxx"

void readBlock(int fd, char *buffer, std::size_t bytes)
{
	while(bytes > 0)
	{
		ssize_t count = read(fd, buffer, bytes);
		if(!count)
			throw IoEofError(errno, std::system_category(), "readBlock() reached end of file");
		if(count < 0)
			throw IoError(errno, std::system_category(), "readBlock()");
		bytes -= count;
		buffer += count;
	}
}

void writeBlock(int fd, char const *buffer, std::size_t bytes)
{
	while(bytes > 0)
	{
		ssize_t count = write(fd, buffer, bytes);
		if(!count)
			throw IoEofError(errno, std::system_category(), "writeBlock() reached end of file");
		if(count < 0)
			throw IoError(errno, std::system_category(), "writeBlock()");
		bytes -= count;
		buffer += count;
	}
}

static constexpr int const packet_length_size = 4;

void readPacket(int fd, char *&buffer)
{
	std::size_t bytes;
	char buf[packet_length_size];
	readBlock(fd, buf, packet_length_size);
	packer::type::Integer<std::size_t, packet_length_size>::parse(buf, bytes);
	buffer = new char[bytes];
	readBlock(fd, buffer, bytes);
}

void writePacket(int fd, char const *buffer, std::size_t bytes)
{
	char buf[packet_length_size];
	packer::type::Integer<std::size_t, packet_length_size>::serialize(buf, bytes);
	writeBlock(fd, buf, packet_length_size);
	writeBlock(fd, buffer, bytes);
}
