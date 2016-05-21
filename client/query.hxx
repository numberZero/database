#pragma once
#include <iostream>
#include <memory>
#include "misc.hxx"
#include "file.hxx"
#include "struct.hxx"

enum class Command
{
	Exit,
	Help,
};

struct ExitException {};

NEW_ERROR_CLASS(ProtocolError, runtime_error, std);
NEW_ERROR_CLASS(RemoteError, runtime_error, std);

struct Query
{
	Query() = default;
	virtual ~Query() = default;
	virtual void perform() = 0;
	virtual char const *name() const = 0;
};

typedef std::unique_ptr<Query> PQuery;

struct QuerySelect: Query
{
	SelectionParams params;
	bool re;
	void perform() override;
	char const *name() const override;
};

struct NetworkQuery: Query
{
	void perform() override;
	virtual void send() = 0;
	virtual void recv() = 0;
};

struct QueryInsert: NetworkQuery
{
	RowData row;
	char const *name() const override;
	void send() override;
	void recv() override;
};

struct QueryRemove: NetworkQuery
{
	SelectionParams params;
	char const *name() const override;
	void send() override;
	void recv() override;
};

struct QueryPrint: NetworkQuery
{
	SelectionParams params;
	char const *name() const override;
	void send() override;
	void recv() override;
};

struct CommandQuery: Query
{
	Command const command;
	CommandQuery(Command cmd): command(cmd) {}
};

struct QueryHelp: CommandQuery
{
	QueryHelp() : CommandQuery(Command::Help) {}
	void perform() override;
	char const *name() const override;
};

struct QueryExit: CommandQuery
{
	QueryExit() : CommandQuery(Command::Help) {}
	void perform() override;
	char const *name() const override;
};

struct QueryMachineState
{
	File connection;
	std::istream &cin;
	std::ostream &cout;
	SelectionParams params;
};

extern QueryMachineState global_state;
