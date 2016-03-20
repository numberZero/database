#pragma once
#include <string>
#include <system_error>

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

struct Socket
{
	Socket() = default;
	Socket(Socket const &) = delete;
	Socket(Socket &&) noexcept;
	explicit Socket(int s) noexcept;
	~Socket();

	Socket &operator = (Socket const &) = delete;
	Socket &operator = (Socket&&) noexcept;

	bool operator ! () const noexcept;

	int get() const noexcept;
	void reset(int s);
	int release() noexcept;

private:
	int fd = -1;
};

extern gai_category_t gai_category;

Socket Connect(std::string address, std::string service); ///< \returns connected socket
Socket Bind(std::string address, std::string service); ///< \returns bound socket (no listen() call performed)
