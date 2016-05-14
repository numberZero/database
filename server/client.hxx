#pragma once
#include <string>
#include "file.hxx"
#include "struct.hxx"

class Client
{
	File socket;

	void sendMessage(int errcode, std::string message);
	void sendAnswerHeader(bool success = true, int mc = 0);
	void sendAnswerHeader(int errcode, std::string message);
	void flush();

	void select(SelectionParams const &sp);
	void insert(RowData const &row);
	void remove(SelectionParams const &rp);

public:
	Client(File &&s);
	void operator() ();
};
