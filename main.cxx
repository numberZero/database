#include <iostream>
#include "db.hxx"

int main(int argc, char **argv)
{
	switch(argc)
	{
		case 1:
			break;
		case 2:
			if(0 == strcmp("-", argv[1]))
				argc = 1;
			break;
		default:
			std::cerr << "Usage:" << std::endl;
			std::cerr << "\t" << argv[0] << " [ data-file-name ]" << std::endl;
			std::cerr << std::endl;
			return -1;
	}
	std::clog << "Database manipulation program" << std::endl;
	Database db;
	switch(argc)
	{
		case 1:
			std::clog << "Reading DB from stdin" << std::endl;
			db.readText(std::cin);
			break;
		case 2:
			std::clog << "Reading DB from file: " << argv[1] << std::endl;
			db.readText(argv[1]);
	}
	std::clog << "Printing human-readable DB to stderr" << std::endl;
	db.printDB(std::clog);
	std::clog << "Writing computer-readable DB to stdout" << std::endl;
	db.writeText(std::cout);
	std::clog << "Done" << std::endl;
	return 0;
}
