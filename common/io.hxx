#pragma once
#include <cinttypes>
#include "misc.hxx"

NEW_ERROR_CLASS(IoError, system_error, std);
NEW_ERROR_CLASS(IoEofError, IoError, );

/**
 * Reads data block from a stream
 * \param [in]	fd		Socket descriptor
 * \param [out]	buffer	Output buffer
 * \param [out]	bytes	Block length
 * \throws IoEofError if reaches EOF before end of data block
 * \throws IoError on other errors, as reported by the OS
 * 
 * Unlike \c read, this function either reads a whole block (of length \p bytes), or throws an exception
 */
void readBlock(int fd, char *buffer, std::size_t bytes);

/**
 * Writes data block to a stream
 * \param [in]	fd		Socket descriptor
 * \param [in]	buffer	Input buffer
 * \param [in]	bytes	Block length
 * \throws IoError on errors, as reported by the OS
 * 
 * Unlike \c write, this function either writes a whole block (of length \p bytes), or throws an exception
 */
void writeBlock(int fd, char const *buffer, std::size_t bytes);

/**
 * Reads next packet on the stream
 * \param [in]	fd		Socket descriptor
 * \param [out]	buffer	Packet contents (allocated by this function using <tt>new char[]</tt>)
 * \param [out]	bytes	Packet length
 * \throws IoEofError if reaches EOF before end of packet
 * \throws IoError on other errors, as reported by the OS
 * \note \p buffer is freed on exception throw
 */
void readPacket(int fd, char *&buffer, std::size_t &bytes);

/**
 * Writes packet to the stream
 * \param [in]	fd		Socket descriptor
 * \param [in]	buffer	Packet contents
 * \param [in]	bytes	Packet length
 * \throws IoError on errors, as reported by the OS
 */
void writePacket(int fd, char const *buffer, std::size_t bytes);
