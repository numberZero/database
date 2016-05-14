#include <cassert>
#include <cstring>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "net.hxx"
#include "rtcheck.hxx"

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

template <bool Callback(File &fd, addrinfo &addr)>
File Choose(std::string address, std::string service, int flags)
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
		File s(socket(info->ai_family, info->ai_socktype, info->ai_protocol));
		if(!s)
			continue;
		if(Callback(s, *info))
			return std::move(s);
	}
	syserror("Can't open suitable socket");
}

bool ConnectCallback(File &fd, addrinfo &addr)
{
	return !connect(fd.get(), addr.ai_addr, addr.ai_addrlen);
}

bool BindCallback(File &fd, addrinfo &addr)
{
	return !bind(fd.get(), addr.ai_addr, addr.ai_addrlen);
}

bool BindCallback_RA(File &fd, addrinfo &addr)
{
	SetSocketOption(fd, SOL_SOCKET, SO_REUSEADDR, 1);
	return BindCallback(fd, addr);
}

File Connect(std::string address, std::string service)
{
	int const options = AI_V4MAPPED | AI_ADDRCONFIG;
	return Choose<ConnectCallback>(address, service, options);
}

File Bind(std::string address, std::string service, bool reuseaddr)
{
	int const options = AI_V4MAPPED | AI_ADDRCONFIG | AI_PASSIVE;
	if(reuseaddr)
		return Choose<BindCallback_RA>(address, service, options);
	else
		return Choose<BindCallback>(address, service, options);
}

void SetSocketOption(File &socket, int level, int optname, int value)
{
	SetSocketOption(socket, level, optname, &value, sizeof(value));
}

void SetSocketOption(File &socket, int level, int optname, void const *value, std::size_t length)
{
	syserror_throwif(setsockopt(socket.get(), level, optname, value, length), "Can't set socket option");
}
