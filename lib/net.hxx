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

File Connect(std::string address, std::string service); ///< \returns connected socket
File Bind(std::string address, std::string service, bool reuseaddr); ///< \returns bound socket (no listen() call performed)

void SetSocketOption(File &socket, int level, int optname, int value);
void SetSocketOption(File &socket, int level, int optname, void const *value, std::size_t length);
