#include "select.hxx"
#include "db.hxx"

/*** Param ***/

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

bool StringParam::check(char const* data)
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return value == data;
}

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

bool IntegerParam::check(long int data)
{
	if(!do_check)
		return true;
	if(!is_valid)
		return false;
	return (min <= data) && (data <= max);
}

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

bool BooleanParam::check(bool data)
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

bool SelectionParams::check(RowReference row)
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

/*** PreSelection ***/

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

bool PreSelection_SimpleKey::isValid()
{
	return index < rows->count;
}

Row *PreSelection_SimpleKey::getRow()
{
	if(!isValid())
		throw DatabaseError("PreSelection_SimpleKey::getRow() called on an invalid selection");
	return node->rows[index % RowRefList::node_capacity];
}

void PreSelection_SimpleKey::next()
{
	if(!isValid())
		throw DatabaseError("PreSelection_SimpleKey::next() called on an invalid selection");
	++index;
	std::size_t shift = index % RowRefList::node_capacity;
	if(!shift)
		node = node->next;
}

/*** Selection ***/

Selection::Selection(Database *database, SelectionParams const& params) :
	db(database),
	p(params),
	s(nullptr)
{
}

Selection::~Selection()
{
	if(s)
		delete s;
}

void Selection::perform()
{
	if(s)
		throw std::logic_error("Selection::perform() is called second time");
	s = new PreSelection_Full(db->rows); // slow but always works
// 	throw std::logic_error("PreSelection_SimpleKey::perform() is not implemented");
}

bool Selection::isValid()
{
	return s->isValid();
}

RowReference Selection::getRow()
{
	return RowReference(db, s->getRow());
}

void Selection::next()
{
	if(!isValid())
		throw DatabaseError("Selection::next() called on an invalid selection");
	for(;;)
	{
		s->next();
		if(!isValid())
			break; // no more rows
		if(p.check(getRow()))
			break; // next row found
	}
}
