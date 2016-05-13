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

typedef File Socket;

extern gai_category_t gai_category;

Socket Connect(std::string address, std::string service); ///< \returns connected socket
Socket Bind(std::string address, std::string service, bool reuseaddr); ///< \returns bound socket (no listen() call performed)
