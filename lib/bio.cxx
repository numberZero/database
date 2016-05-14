#include <cassert>
#include "bio.hxx"
#include "fs.hxx"
#include "packer.hxx"

void readBlock(File &fd, char *buffer, std::size_t bytes)
{
	while(bytes > 0)
	{
		std::size_t count = Read(fd, buffer, bytes);
		if(!count)
			throw BioEof();
		bytes -= count;
		buffer += count;
	}
}

void writeBlock(File &fd, char const *buffer, std::size_t bytes)
{
	while(bytes > 0)
	{
		std::size_t count = Write(fd, buffer, bytes);
		assert(count);
		bytes -= count;
		buffer += count;
	}
}

static constexpr int const packet_length_size = 4;

void readPacket(File &fd, char *&buffer, std::size_t &bytes)
{
	buffer = readPacket(fd, bytes);
}

char *readPacket(File &fd, std::size_t &bytes)
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

void writePacket(File &fd, char const *buffer, std::size_t bytes)
{
	char buf[packet_length_size];
	packer::type::Integer<std::size_t, packet_length_size>::static_serialize(buf, bytes);
	writeBlock(fd, buf, packet_length_size);
	writeBlock(fd, buffer, bytes);
}
