#include <cassert>
#include <cstring>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "net.hxx"

gai_category_t gai_category;

char const *gai_category_t::name() const noexcept
{
	return "getaddrinfo";
}

std::string gai_category_t::message(int rv) const
{
	return std::string(gai_strerror(rv));
}

void gai_deleter::operator() (addrinfo *ai)
{
	freeaddrinfo(ai);
}

Socket::Socket(Socket &&b) noexcept
{
	std::swap(fd, b.fd); // this->fd is already initialized with (-1)
}

Socket::Socket(int s) noexcept
{
	fd = s;
}

Socket::~Socket()
{
	reset(-1);
}

Socket &Socket::operator = (Socket &&b) noexcept
{
	std::swap(fd, b.fd);
	return *this;
}

bool Socket::operator ! () const noexcept
{
	return fd < 0;
}

int Socket::get() const noexcept
{
	return fd;
}

void Socket::reset(int s)
{
	if(fd)
		close(fd);
	fd = s;
}

int Socket::release() noexcept
{
	int s = fd;
	fd = -1;
	return s;
}

template <bool Callback(int fd, addrinfo &addr)>
int Choose(std::string address, std::string service, int flags)
{
	addrinfo hint;
	addrinfo *info = nullptr;
	std::unique_ptr<addrinfo, gai_deleter> list;

	std::memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = flags;

	int err = getaddrinfo(address.c_str(), service.c_str(), &hint, &info);
	list.reset(info);
	if(err)
		throw std::system_error(err, gai_category);

	errno = 0;
	for(addrinfo *info = list.get(); info; info = info->ai_next)
	{
		int s = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
		if(s < 0)
			continue;
		try
		{
			if(Callback(s, *info))
				return s;
		}
		catch(...)
		{
			close(s);
			throw;
		}
		close(s);
	}
	throw std::system_error(errno, std::system_category());
}

bool ConnectCallback(int fd, addrinfo &addr)
{
	return 0 == connect(fd, addr.ai_addr, addr.ai_addrlen);
}

bool BindCallback(int fd, addrinfo &addr)
{
	return 0 == bind(fd, addr.ai_addr, addr.ai_addrlen);
}

Socket Connect(std::string address, std::string service)
{
	return Socket(Choose<ConnectCallback>(address, service, AI_V4MAPPED | AI_ADDRCONFIG));
}

Socket Bind(std::string address, std::string service)
{
	return Socket(Choose<BindCallback>(address, service, AI_V4MAPPED | AI_ADDRCONFIG | AI_PASSIVE));
}
