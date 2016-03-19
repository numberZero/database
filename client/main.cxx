#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>
#include "misc.hxx"
#include "reader.hxx"
#include "db/dbhelper.hxx"
#include "binary/network.hxx"

char const *const PRE_Input = "> ";
char const *const PRE_Output = "< ";
char const *const PRE_Info = "# ";
char const *const PRE_Error = "! ";

// int main(int argc, char **argv)
int main(void)
{
	try
	{
		try
		{
			QueryReader rd(std::cin);
			std::cout << PRE_Info << "Welcome to the Zolden database client!" << std::endl;
			std::cout << PRE_Info << std::endl;
			std::cout << PRE_Info << "Type queries ending with ;" << std::endl;
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
					SelectionParams sp;
					NetworkType<SelectionParams>::serialize(std::cout, sp);
				}
				catch(InvalidQueryError const& e)
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
	catch(EofError const& e)
	{
		std::cout << "Unexpected end of input: " << e.what() << std::endl;
	}
	catch(ReaderError const& e)
	{
		std::cout << "Input read erorr: " << e.what() << std::endl;
	}
	catch(std::logic_error const& e)
	{
		std::cout << "Program logic erorr: " << e.what() << std::endl;
	}
	catch(std::runtime_error const& e)
	{
		std::cout << "Runtime erorr: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Invalid exception caught. Rethrowing to the standard library." << std::endl;
		throw;
	}
	return 0;
}
