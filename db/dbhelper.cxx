#include "dbhelper.hxx"
#include "dbcommon.hxx"
#include "db.hxx"

RowReference::RowReference(Database const *database, Row const *prow) :
	db(database),
	row(prow)
{
}

char const *RowReference::getTeacher() const
{
	return db->Teachers::data.get(row->teacher).name;
}

char const *RowReference::getSubject() const
{
	return db->Subjects::data.get(row->subject).name;
}

unsigned RowReference::getRoom() const
{
	return db->Rooms::data.get(row->room).number;
}

unsigned RowReference::getGroup() const
{
	return db->Groups::data.get(row->group).number;
}

unsigned RowReference::getDay() const
{
	return db->Times::data.get(row->time).day;
}

unsigned RowReference::getLesson() const
{
	return db->Times::data.get(row->time).lesson;
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
