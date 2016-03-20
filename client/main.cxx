#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "misc.hxx"
#include "reader.hxx"
#include "db/dbhelper.hxx"
#include "binary/network.hxx"
#include "bio.hxx"

char const *const PRE_Input = "> ";
char const *const PRE_Output = "< ";
char const *const PRE_Info = "# ";
char const *const PRE_Error = "! ";

class gai_category_t : public std::error_category
{
public:
	char const* name() const noexcept override
	{
		return "getaddrinfo";
	}

	std::string message(int rv) const override
	{
		return std::string(gai_strerror(rv));
	}
} gai_category;

struct gai_deleter
{
	void operator() (addrinfo *ai)
	{
		freeaddrinfo(ai);
	}
};

void Connect(std::string address, std::string service = std::to_string(zolden_port))
{
	addrinfo hint;
	addrinfo *dest = nullptr;
	std::unique_ptr<addrinfo, gai_deleter> list;

	std::memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG | AI_CANONNAME;

	int err = getaddrinfo(address.c_str(), service.c_str(), &hint, &dest);
	list.reset(dest);
	if(err)
		throw std::system_error(err, gai_category);

	for(addrinfo *dest = list.get(); dest; dest = dest->ai_next)
	{
		int s = socket(dest->ai_family, dest->ai_socktype, dest->ai_protocol);
		if(s < 0)
			continue;
		if(connect(s, dest->ai_addr, dest->ai_addrlen) == 0)
		{
			global_state.connection = s;
			break;
		}
		close(s);
	}
	if(global_state.connection < 0)
		throw std::system_error(errno, std::system_category(), "Can't connect to the server");
}

int main(int argc, char **argv)
{
	try
	{
		switch(argc)
		{
			case 1:
				Connect("127.0.0.1");
				break;
			case 2:
				Connect(argv[1]);
				break;
			case 3:
				Connect(argv[1], argv[2]);
				break;
			default:
				std::cout << "Usage:" << std::endl;
				std::cout << "\t" << argv[0] << " [ host [ port ] ]" << std::endl;
				return 1;
		}
		try
		{
			QueryReader rd(std::cin);
			std::cout << PRE_Info << "Welcome to the Zolden database client!" << std::endl;
			std::cout << PRE_Info << std::endl;
			std::cout << PRE_Info << "Enter queries ending with ;" << std::endl;
			std::cout << PRE_Info << "Type \"help;\" for more information" << std::endl;
			for(;;)
			{
				try
				{
					std::cout << std::endl;
					std::cout << PRE_Input;
					PQuery q(rd.readQuery());
					if(!q)
						break;
					q->perform();
				}
				catch(InvalidQueryError const &e)
				{
					std::cout << PRE_Error << "Query error: " << e.what() << std::endl;
					rd.skipLine();
				}
			}
		}
		catch(ExitException)
		{
			std::cout << std::endl << PRE_Info << "Good bye!" << std::endl;
		}
		catch(...)
		{
			std::cout << std::endl << PRE_Error;
			throw;
		}
	}
	catch(EofError const &e)
	{
		std::cout << "Unexpected end of input: " << e.what() << std::endl;
	}
	catch(ReaderError const &e)
	{
		std::cout << "Input read error: " << e.what() << std::endl;
	}
	catch(std::logic_error const &e)
	{
		std::cout << "Program logic error: " << e.what() << std::endl;
	}
	catch(std::runtime_error const &e)
	{
		std::cout << "Runtime error: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Invalid exception caught. Rethrowing to the standard library." << std::endl;
		throw;
	}
	return 0;
}
