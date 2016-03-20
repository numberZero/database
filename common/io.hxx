#pragma once
#include <cinttypes>
#include "misc.hxx"

NEW_ERROR_CLASS(IoError, system_error, std);
NEW_ERROR_CLASS(IoEofError, IoError, );

void readBlock(int fd, char *buffer, std::size_t bytes);
void writeBlock(int fd, char const *buffer, std::size_t bytes);

void readPacket(int fd, char *& buffer, std::size_t& bytes); // allocates buffer with new char[]
void writePacket(int fd, char const *buffer, std::size_t bytes);
