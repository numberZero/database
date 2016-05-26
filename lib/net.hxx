#pragma once
#include <string>
#include <system_error>
#include "file.hxx"

class gai_category_t : public std::error_category
{
public:
	char const *name() const noexcept override;
	std::string message(int rv) const override;
};

struct gai_deleter
{
	void operator() (struct addrinfo *ai);
};

extern gai_category_t gai_category;

union IP4addr
{
	std::uint8_t bytes[4];
	std::uint32_t data;
};

struct AddressIPv4
{
	IP4addr addr;
	std::uint16_t port;
};

File Connect(std::string address, std::string service); ///< \returns connected socket
File Bind(std::string address, std::string service, bool reuseaddr); ///< \returns bound socket (no listen() call performed)

void SetSocketOption(File &socket, int level, int optname, int value);
void SetSocketOption(File &socket, int level, int optname, void const *value, std::size_t length);

AddressIPv4 GetSocketAddressIPv4(File &socket);

namespace std
{
	string to_string(IP4addr const &);
	string to_string(AddressIPv4 const &);
}
