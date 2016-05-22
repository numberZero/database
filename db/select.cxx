#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif
#include "db.hxx"
#include "hashtable.hxx"
#include "select.hxx"

/*** PreSelection ***/

PreSelection::PreSelection(Table<Row> const &db) :
	rows(db)
{
}

Row const *PreSelection::getRow()
{
	return &rows.get(getRowId());
}

/*** PreSelection_Full ***/

PreSelection_Full::PreSelection_Full(Table<Row> const &rows) :
	PreSelection(rows),
	index(0)
{
#ifndef NDEBUG
	std::clog << "Using full preselection\n";
#endif
	if(!rows.first(index))
		index = invalid_index;
}

Id PreSelection_Full::getRowId()
{
	return index;
}

bool PreSelection_Full::isValid()
{
	return index != invalid_index;
}

void PreSelection_Full::next()
{
	if(!rows.next(index))
		index = invalid_index;
}

/*** PreSelection_SimpleKey ***/

PreSelection_SimpleKey::PreSelection_SimpleKey(Table<Row> const &rows, HashTable::RowIterator iter):
	PreSelection(rows),
	iterator(iter)
{
#ifndef NDEBUG
	std::clog << "Using simple key preselection\n";
#endif
}

Id PreSelection_SimpleKey::getRowId()
{
	assert(isValid());
	return *iterator;
}

bool PreSelection_SimpleKey::isValid()
{
	return !!iterator;
}

void PreSelection_SimpleKey::next()
{
	++iterator;
}

/********* Selection *********/

Selection::Selection() :
	db(nullptr),
	s(nullptr)
{
}

Selection::Selection(Database &database, SelectionParams const &params, std::unique_ptr<PreSelection> &&preselect, std::unique_ptr<SRXW_ReadLockGuard> &&lguard) :
	db(&database),
	p(params),
	s(std::move(preselect)),
	guard(std::move(lguard))
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
	guard = std::move(b.guard);
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
