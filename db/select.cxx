#include <cassert>
#include "db.hxx"
#include "select.hxx"

/********* Param *********/

/*** StringParam ***/

void StringParam::refine(StringParam const& b)
{
	do_return |= b.do_return;
	if(!b.do_check)
		return;
	if(do_check)
	{
		if(value != b.value)
			is_valid = false;
	}
	else
	{
		do_check = true;
		value = b.value;
	}
}

bool StringParam::check(char const* data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return value == data;
}

/*** IntegerParam ***/

void IntegerParam::refine(IntegerParam const& b)
{
	do_return |= b.do_return;
	if(!b.do_check)
		return;
	if(do_check)
	{
		min = std::max(min, b.min);
		max = std::min(max, b.max);
	}
	else
	{
		do_check = true;
		min = b.min;
		max = b.max;
	}
	is_valid = min <= max;
}

bool IntegerParam::check(long int data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return (min <= data) && (data <= max);
}

/*** BooleanParam ***/

void BooleanParam::refine(BooleanParam const& b)
{
	do_return |= b.do_return;
	if(!b.do_check)
		return;
	if(do_check)
		is_valid = value == b.value;
	do_check = true;
	value = b.value;
}

bool BooleanParam::check(bool data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return value == data;
}

/*** SelectionParams ***/

void SelectionParams::refine(SelectionParams const& b)
{
	teacher.refine(b.teacher);
	subject.refine(b.subject);
	room.refine(b.room);
	group.refine(b.group);
	meta.refine(b.meta);
	day.refine(b.day);
	lesson.refine(b.lesson);
}

bool SelectionParams::check(RowReference row) const
{
	return
		teacher.check(row.getTeacher()) &&
		subject.check(row.getSubject()) &&
		room.check(row.getRoom()) &&
		group.check(row.getGroup()) &&
		meta.check(row.isMetaGroup()) &&
		day.check(row.getDay()) &&
		lesson.check(row.getLesson());
}

bool SelectionParams::isValid() const
{
	return
		teacher.is_valid &&
		subject.is_valid &&
		room.is_valid &&
		group.is_valid &&
		meta.is_valid &&
		day.is_valid &&
		lesson.is_valid;
}

/********* PreSelection *********/

/*** PreSelection_Full ***/

PreSelection_Full::PreSelection_Full(Table<Row>& table) :
	rows(table),
	index(0)
{
}

bool PreSelection_Full::isValid()
{
	return index < rows.count;
}

Row* PreSelection_Full::getRow()
{
	return &rows[index];
}

void PreSelection_Full::next()
{
	++index;
}

/*** PreSelection_SimpleKey ***/

bool PreSelection_SimpleKey::isValid()
{
	return index < rows->count;
}

Row *PreSelection_SimpleKey::getRow()
{
	assert(isValid());
	return node->rows[index % RowRefList::node_capacity];
}

void PreSelection_SimpleKey::next()
{
	assert(isValid());
	++index;
	std::size_t shift = index % RowRefList::node_capacity;
	if(!shift)
		node = node->next;
}

/********* Selection *********/

Selection::Selection() :
	db(nullptr),
	s(nullptr)
{
}

Selection::Selection(Database& database, SelectionParams const& params) :
	db(&database),
	p(params),
	s(nullptr)
{
	if(!params.isValid())
		return;
	s = new PreSelection_Full(db->rows); // slow but always works
	reach();
}

Selection::Selection(Selection&& b)
{
	reset(b);
}

Selection::~Selection()
{
	drop();
}

Selection& Selection::operator=(Selection&& b)
{
	drop();
	reset(b);
	return *this;
}

void Selection::drop()
{
	if(s)
		delete s;
}

void Selection::reset(Selection& b)
{
	db = b.db;
	p = b.p;
	s = b.s;
	b.s = nullptr;
}

bool Selection::reach()
{
	while(s->isValid())
	{
		if(p.check(getRow()))
			return true;
		s->next();
	}
	return false;
}

bool Selection::isValid()
{
	return s && s->isValid();
}

RowReference Selection::getRow()
{
	if(!isValid())
		throw DatabaseLogicError("Selection::getRow() called on an invalid selection");
	return RowReference(db, s->getRow());
}

void Selection::next()
{
	if(!isValid())
		throw DatabaseLogicError("Selection::next() called on an invalid selection");
	s->next();
	reach();
}
