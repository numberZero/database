#include <cassert>
#include "db.hxx"
#include "select.hxx"

/********* Param *********/

/*** StringParam ***/

void StringParam::refine(StringParam const &b)
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

bool StringParam::check(char const *data) const
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return value == data;
}

void StringParam::set(std::string const &_value)
{
	do_check = true;
	is_valid = true;
	value = _value;
}

/*** IntegerParam ***/

void IntegerParam::refine(IntegerParam const &b)
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

void IntegerParam::set(long _value)
{
	set(_value, _value);
}

void IntegerParam::set(long _min, long _max)
{
	do_check = true;
	is_valid = true;
	min = _min;
	max = _max;
}

void IntegerParam::set(std::string const &_value)
{
	std::size_t pos = _value.find('-');
	if(pos == std::string::npos)
		set(std::stol(_value));
	else
		set(std::stol(_value.substr(0, pos)), std::stol(_value.substr(pos + 1)));
}

/*** BooleanParam ***/

void BooleanParam::refine(BooleanParam const &b)
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

void BooleanParam::set(bool _value)
{
	do_check = true;
	is_valid = true;
	value = _value;
}

void BooleanParam::set(std::string const &_value)
{
	set(parseBoolean(_value));
}

/*** SelectionParams ***/

void SelectionParams::refine(SelectionParams const &b)
{
	teacher.refine(b.teacher);
	subject.refine(b.subject);
	room.refine(b.room);
	group.refine(b.group);
	day.refine(b.day);
	lesson.refine(b.lesson);
}

void SelectionParams::clearReturn()
{
	teacher.do_return = false;
	subject.do_return = false;
	room.do_return = false;
	group.do_return = false;
	day.do_return = false;
	lesson.do_return = false;
}

bool SelectionParams::check(RowReference row) const
{
	return
		teacher.check(row.getTeacher()) &&
		subject.check(row.getSubject()) &&
		room.check(row.getRoom()) &&
		group.check(row.getGroup()) &&
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
		day.is_valid &&
		lesson.is_valid;
}

/********* PreSelection *********/

/*** PreSelection_Real ***/

PreSelection_Real::PreSelection_Real(Rows const &db) :
	rows(db)
{
}

Row const *PreSelection_Real::getRow()
{
	return &rows[getRowId()];
}

/*** PreSelection_Full ***/

PreSelection_Full::PreSelection_Full(Rows const &table) :
	PreSelection_Real(table),
	index(0)
{
}

Id PreSelection_Full::getRowId()
{
	return index;
}

bool PreSelection_Full::isValid()
{
	return index < rows.count;
}

void PreSelection_Full::next()
{
	++index;
}

/*** PreSelection_SimpleKey ***/

Id PreSelection_SimpleKey::getRowId()
{
	assert(isValid());
	return node->rows[index % RowRefList::node_capacity];
}

bool PreSelection_SimpleKey::isValid()
{
	return index < rows->count;
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

Selection::Selection(Database &database, SelectionParams const &params, std::unique_ptr<PreSelection> &&preselect) :
	db(&database),
	p(params),
	s(std::move(preselect))
{
	reach();
}

Selection::Selection(Selection &&b)
{
	reset(b);
}

Selection &Selection::operator=(Selection &&b)
{
	reset(b);
	return *this;
}

void Selection::reset(Selection &b)
{
	db = b.db;
	p = b.p;
	s = std::move(b.s);
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
