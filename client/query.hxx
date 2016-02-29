#pragma once
#include <iostream>
#include "db/select.hxx"

class Database;

class Client
{
private:
	Database *db;
	SelectionParams sp;
	Selection sel;

public:
	enum class Result
	{
		Error = -1,
		NoData = 0,
		Success = 1,
	};

	class Query
	{
	protected:
		Query() = default;

	public:
		virtual ~Query() = default;
		virtual Result perform(Client& client, Database& db) = 0;

		static Query *parse(std::string const& text);
		static Query *read(std::istream& source);
	};

	class QuerySelect: public Query
	{
	private:
		SelectionParams params;
		bool re;
	public:
		QuerySelect(std::istream& in, bool reselect);
		Result perform(Client& client, Database& db) override;
	};

	class QueryPrint: public Query
	{
	private:
		SelectionParams params;
	public:
		QueryPrint(std::istream& in);
		Result perform(Client& client, Database& db) override;
	};

	class QueryInsert: public Query
	{
	private:
		RowData row;
	public:
		QueryInsert(std::istream& in);
		Result perform(Client& client, Database& db) override;
	};

	class QueryRemove: public Query
	{
	private:
		SelectionParams params;
	public:
		QueryRemove(std::istream& in);
		Result perform(Client& client, Database& db) override;
	};

	Client(Database *database);
	bool signle_query(std::istream& in, std::ostream& out);
	void run(std::istream& in, std::ostream& out);
};
