#include <iostream>
#include "db/db.hxx"
#include "client/query.hxx"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << "\t" << argv[0] << " [ data-file-name ]" << std::endl;
		std::cerr << std::endl;
		return -1;
	}
	std::clog << "Database manipulation program" << std::endl;
	Database db;
	db.readText(argv[1]);
	Client client(&db);
	client.run(std::cin, std::cout);
/*
	SelectionParams sp;
	sp.subject.value = "Algebra";
	sp.subject.do_check = true;
	sp.lesson.min = 1;
	sp.lesson.max = 1;
	sp.lesson.do_check = true;
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
*/
	return 0;
}
