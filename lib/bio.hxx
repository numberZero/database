#pragma once
#include <cinttypes>
#include "file.hxx"

struct BioEof {};

/**
 * Reads data block from a stream
 * \param [in]	fd		Socket descriptor
 * \param [out]	buffer	Output buffer
 * \param [out]	bytes	Block length
 * \throws BioEof if reaches EOF before end of data block
 * \throws BioError on other errors, as reported by the OS
 * 
 * Unlike \c read, this function either reads a whole block (of length \p bytes), or throws an exception
 */
void readBlock(File &fd, char *buffer, std::size_t bytes);

/**
 * Writes data block to a stream
 * \param [in]	fd		Socket descriptor
 * \param [in]	buffer	Input buffer
 * \param [in]	bytes	Block length
 * \throws BioError on errors, as reported by the OS
 * 
 * Unlike \c write, this function either writes a whole block (of length \p bytes), or throws an exception
 */
void writeBlock(File &fd, char const *buffer, std::size_t bytes);

/**
 * Reads next packet on the stream
 * \param [in]	fd		Socket descriptor
 * \param [out]	buffer	Packet contents (allocated by this function using <tt>new char[]</tt>)
 * \param [out]	bytes	Packet length
 * \throws BioEof if reaches EOF before end of packet
 * \throws BioError on other errors, as reported by the OS
 * \note \p buffer is freed on exception throw
 */
void readPacket(File &fd, char *&buffer, std::size_t &bytes);

/**
 * Reads next packet on the stream
 * \param [in]	fd		Socket descriptor
 * \param [out]	bytes	Packet length
 * \returns				Packet contents (allocated using <tt>new char[]</tt>)
 * \throws BioEof if reaches EOF before end of packet
 * \throws BioError on other errors, as reported by the OS
 * \note The same as <tt>readPacket(int, char *&, std::size_t &)</tt>
 */
char *readPacket(File &fd, std::size_t &bytes);

/**
 * Writes packet to the stream
 * \param [in]	fd		Socket descriptor
 * \param [in]	buffer	Packet contents
 * \param [in]	bytes	Packet length
 * \throws BioError on errors, as reported by the OS
 */
void writePacket(File &fd, char const *buffer, std::size_t bytes);
