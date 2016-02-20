#include <iostream>
#include "db.hxx"

int main()//int argc, char **argv)
{
	std::clog << "Database manipulation program" << std::endl;
	Database db;
	std::clog << "Reading DB from stdin" << std::endl;
	db.readText(std::cin);
	std::clog << "Printing human-readable DB to stderr" << std::endl;
	db.printDB(std::clog);
	std::clog << "Writing computer-readable DB to stdout" << std::endl;
	db.writeText(std::cout);
	std::clog << "Done" << std::endl;
	return 0;
}
