#include <cctype>
#include <string>
#include <sstream>
#include "db/dbcommon.hxx"
#include "misc.hxx"
#include "query.hxx"

IntegerParam parseRange(std::string const& range)
{
	IntegerParam p;
	p.do_check = true;
	std::string::size_type k = range.find_first_of('-');
	if(k == std::string::npos)
	{
		p.min = p.max = std::stoi(range);
	}
	else
	{
		p.min = std::stoi(range.substr(0, k));
		p.max = std::stoi(range.substr(k + 1));
	}
	return std::move(p);
}

SelectionParams parseParams(std::istream& in)
{
	SelectionParams p;
	for(;;)
	{
		SelectionParams p2;
		std::string key;
		std::string value;
		in >> key;
		upcase_it(key);
		if(key == "END")
			break;
		std::getline(in, value);
		trim_it(value);
		if(key == "TEACHER")
		{
			p2.teacher.do_check = true;
			p2.teacher.value = value;
		}
		else if(key == "SUBJECT")
		{
			p2.subject.do_check = true;
			p2.subject.value = value;
		}
		else if(key == "ROOM")
		{
			p2.room = parseRange(value);
		}
		else if(key == "GROUP")
		{
			std::string::size_type l = value.find_first_of(':');
			if(l == std::string::npos)
			{
				p2.group = parseRange(value);
				p2.meta.do_check = true;
				p2.meta.value = false;
			}
			else
			{
				std::string num = value.substr(0, l);
				p2.meta.do_check = true;
				std::string meta = value.substr(l + 1);
				upcase_it(meta);
				if((meta == "ANY") || (meta == "*") || (meta == ""))
					p2.meta.do_check = false;
				else if((meta == "NORMAL") || (meta == "GROUP"))
					p2.meta.value = false;
				else if((meta == "META") || (meta == "METAGROUP"))
					p2.meta.value = true;
				else
					throw DatabaseFileError("Invalid group suffix");
			}
		}
		else if(key == "META")
		{
			p2.meta.do_check = true;
			p2.meta.value = parseBoolean(value);
		}
		else if(key == "TIME")
		{
			std::string::size_type l = value.find_first_of(':');
			if(l == std::string::npos)
				throw DatabaseFileError("Invalid time value");
			p2.day = parseRange(value.substr(0, l));
			p2.lesson = parseRange(value.substr(l + 1));
		}
		else if(key == "DAY")
		{
			p2.day = parseRange(value);
		}
		else if(key == "LESSON")
		{
			p2.lesson = parseRange(value);
		}
		p.refine(p2);
	}
	return std::move(p);
}

Client::Query *Client::Query::parse(std::string const& text)
{
	std::istringstream s(text);
	return read(s);
}

Client::Query *Client::Query::read(std::istream& source)
{
	std::string key;
	source >> key;
	upcase_it(key);
	if(key == "PRINT")
		return new QueryPrint(source);
	if(key == "SELECT")
		return new QuerySelect(source, false);
	if(key == "RESELECT")
		return new QuerySelect(source, true);
	if(key == "INSERT")
		return new QueryInsert(source);
	if(key == "REMOVE")
		return new QueryRemove(source);
	throw DatabaseError("Invalid query");
}

Client::QuerySelect::QuerySelect(std::istream& in, bool reselect) :
	params(parseParams(in)),
	re(reselect)
{
}

Client::Query::Result Client::QuerySelect::perform(Client& client, Database& db)
{
	return Result::Error;
}

Client::QueryInsert::QueryInsert(std::istream& in)
{
}

Client::Query::Result Client::QueryInsert::perform(Client& client, Database& db)
{
	return Result::Error;
}

Client::QueryRemove::QueryRemove(std::istream& in):
	params(parseParams(in))
{
}

Client::Query::Result Client::QueryRemove::perform(Client& client, Database& db)
{
	return Result::Error;
}

Client::QueryPrint::QueryPrint(std::istream& in)
{
}

Client::Query::Result Client::QueryPrint::perform(Client& client, Database& db)
{
	return Result::Error;
}

Client::Client(Database* database) :
	db(database)
{
}

void Client::signle_query(std::istream& in, std::ostream& out)
{
	std::ios::iostate old_emask = in.exceptions();
	in.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
	Query *q = Query::read(in);
	q->perform(*this, *db);
	in.exceptions(old_emask);
}

void Client::run(std::istream& in, std::ostream& out)
{
	try
	{
		for(;;)
			signle_query(in, out);
	}
	catch(std::ios_base::failure const& e)
	{
// 		if(in.eof())
// 			return; // silently ignore EOF
// 		throw;
	}
}
