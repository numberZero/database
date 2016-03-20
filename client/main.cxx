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
#include "net.hxx"

char const *const PRE_Input = "> ";
char const *const PRE_Output = "< ";
char const *const PRE_Info = "# ";
char const *const PRE_Error = "! ";

int main(int argc, char **argv)
{
	try
	{
		switch(argc)
		{
			case 1:
				global_state.connection = Connect("127.0.0.1", std::to_string(zolden_port));
				break;
			case 2:
				global_state.connection = Connect(argv[1], std::to_string(zolden_port));
				break;
			case 3:
				global_state.connection = Connect(argv[1], argv[2]);
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
