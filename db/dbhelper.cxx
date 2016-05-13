#include "dbhelper.hxx"
#include "dbcommon.hxx"
#include "db.hxx"

/*** RowRefList ***/
/*
RowRefList::~RowRefList()
{
	Node *node = head;
	while(node)
	{
		Node *next = node->next;
		delete node;
		node = next;
	}
}

void RowRefList::addRow(Id row)
{
	std::size_t idx = count % node_capacity;
	if(!idx)
	{
		Node *node = new Node;
		node->next = head;
		head = node;
	}
	head->rows.get(idx) = row;
	++count;
}
*/
/*** RowReference ***/

RowReference::RowReference(Database const *database, Row const *prow) :
	db(database),
	row(prow)
{
}

char const *RowReference::getTeacher() const
{
	return db->Teachers::data.get(row->teacher).data.name;
}

char const *RowReference::getSubject() const
{
	return db->Subjects::data.get(row->subject).data.name;
}

unsigned RowReference::getRoom() const
{
	return db->Rooms::data.get(row->room).data.number;
}

unsigned RowReference::getGroup() const
{
	return db->Groups::data.get(row->group).data.number;
}

unsigned RowReference::getDay() const
{
	return db->Times::data.get(row->time).data.day;
}

unsigned RowReference::getLesson() const
{
	return db->Times::data.get(row->time).data.lesson;
}

RowData RowReference::getData() const
{
	return {
		getTeacher(),
		getSubject(),
		getRoom(),
		getGroup(),
		getDay(),
		getLesson()
	};
}

bool RowReference::check(SelectionParams const &sp) const
{
	return
		row &&
		sp.teacher.check(getTeacher()) &&
		sp.subject.check(getSubject()) &&
		sp.room.check(getRoom()) &&
		sp.group.check(getGroup()) &&
		sp.day.check(getDay()) &&
		sp.lesson.check(getLesson());
}

/*** getKey ***/

char const *getKey(Teacher const &object)
{
	return object.name;
}

char const *getKey(Subject const &object)
{
	return object.name;
}

uint_fast32_t getKey(Room const &object)
{
	return getKey(object.number);
}

uint_fast32_t getKey(Group const &object)
{
	return getKey(object.number);
}

uint_fast32_t getKey(Time const &object)
{
	return getKey(object.day, object.lesson);
}

char const *getKey(std::string name)
{
	return name.c_str();
}

std::uint_fast32_t getKey(std::uint16_t number)
{
	return number;
}

std::uint_fast32_t getKey(std::uint16_t day, std::uint16_t lesson)
{
	return (day << 16) | lesson;
}
