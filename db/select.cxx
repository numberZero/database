#include <cassert>
#include "db.hxx"
#include "select.hxx"

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
		if(getRow().check(p))
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
