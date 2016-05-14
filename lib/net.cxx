#include <cassert>
#include <cstdio>
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

AddressIPv4 GetSocketAddressIPv4(File &socket)
{
	AddressIPv4 result;
	sockaddr_in addr;
	socklen_t len(sizeof(addr));
	getsockname(socket.get(), reinterpret_cast<sockaddr *>(&addr), &len);
	if(addr.sin_family != AF_INET)
		throw std::invalid_argument("The socket given is not an IPv4 socket");
	result.addr.data = addr.sin_addr.s_addr;
	result.port = ntohs(addr.sin_port);
	return result;
}

namespace std
{
	string to_string(IP4addr const &a)
	{
		char buf[16]; // enough
		int len = snprintf(buf, 16, "%hhu.%hhu.%hhu.%hhu", a.bytes[0], a.bytes[1], a.bytes[2], a.bytes[3]);
		assert(len < 16);
		return string(buf, len);
	}

	string to_string(AddressIPv4 const &a)
	{
		char buf[22]; // enough
		int len = snprintf(buf, 22, "%hhu.%hhu.%hhu.%hhu:%hu", a.addr.bytes[0], a.addr.bytes[1], a.addr.bytes[2], a.addr.bytes[3], a.port);
		assert(len < 22);
		return string(buf, len);
	}
};
