#pragma once
#include <string>
#include "net.hxx"
#include "struct.hxx"

class Client
{
	Socket socket;

	void sendMessage(int errcode, std::string message);
	void sendAnswerHeader(bool success = true, int mc = 0);
	void sendAnswerHeader(int errcode, std::string message);
	void select(SelectionParams const &sp);
	void insert(RowData const &row);
	void remove(SelectionParams const &rp);

public:
	Client(Socket &&s);
	void operator() ();
};
