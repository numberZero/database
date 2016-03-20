#pragma once
#include <iostream>
#include <memory>
#include "db/select.hxx"

enum class Command
{
	Exit,
	Help,
};

struct ExitException {};

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

struct QueryInsert: Query
{
	RowData row;
	void perform() override;
	char const *name() const override;
};

struct QueryRemove: Query
{
	SelectionParams params;
	void perform() override;
	char const *name() const override;
};

struct QueryPrint: Query
{
	SelectionParams params;
	void perform() override;
	char const *name() const override;
};

struct QueryCommand: Query
{
	Command const command;
	QueryCommand(Command cmd): command(cmd) {}
};

struct QueryHelp: QueryCommand
{
	QueryHelp() : QueryCommand(Command::Help) {}
	void perform() override;
	char const *name() const override;
};

struct QueryExit: QueryCommand
{
	QueryExit() : QueryCommand(Command::Help) {}
	void perform() override;
	char const *name() const override;
};

struct QueryMachineState
{
	int connection; // socket
	std::istream& cin;
	std::ostream& cout;
	SelectionParams params;
};

extern QueryMachineState global_state;
