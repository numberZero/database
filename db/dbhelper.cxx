#include "dbhelper.hxx"
#include "dbcommon.hxx"
#include "db.hxx"

/*** RowRefList ***/

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
	head->rows[idx] = row;
	++count;
}

/*** RowReference ***/

RowReference::RowReference(Database const *database, Row const *prow) :
	db(database),
	row(prow)
{
}

char const *RowReference::getTeacher() const
{
	return db->teachers[row->teacher].data.name;
}

char const *RowReference::getSubject() const
{
	return db->subjects[row->subject].data.name;
}

unsigned RowReference::getRoom() const
{
	return db->rooms[row->room].data.number;
}

unsigned RowReference::getGroup() const
{
	return db->groups[row->group].data.number;
}

unsigned RowReference::getDay() const
{
	return db->times[row->time].data.day;
}

unsigned RowReference::getLesson() const
{
	return db->times[row->time].data.lesson;
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
	return getRoomKey(object.number);
}

uint_fast32_t getKey(Group const &object)
{
	return getGroupKey(object.number);
}

uint_fast32_t getKey(Time const &object)
{
	return getTimeKey(object.day, object.lesson);
}

uint_fast32_t getRoomKey(std::uint16_t number)
{
	return number;
}

uint_fast32_t getGroupKey(std::uint16_t number)
{
	return number;
}

uint_fast32_t getTimeKey(std::uint16_t day, std::uint16_t lesson)
{
	return (day << 16) | lesson;
}
