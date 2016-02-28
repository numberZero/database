#include <iostream>
#include "db/db.hxx"

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
	SelectionParams sp;
	sp.subject.value = "Algebra";
	sp.subject.do_check = true;
	for(Selection s(db, sp); s.isValid(); s.next())
	{
		RowReference r = s.getRow();
		std::cout << "*** Row ***\n";
		std::cout << "Day: " << r.getDay() << "\n";
		std::cout << "Lesson #" << r.getLesson() << "\n";
		std::cout << "Room #" << r.getRoom() << "\n";
		std::cout << "Subject: " << r.getSubject() << "\n";
		std::cout << "Teacher: " << r.getTeacher() << "\n";
		if(r.isMetaGroup())
			std::cout << "Metagroup #";
		else
			std::cout << "Group #";
		std::cout << r.getGroup() << "\n";
		std::cout << std::endl; // also flushes the stream
	}
	std::clog << "Done" << std::endl;
	return 0;
}
