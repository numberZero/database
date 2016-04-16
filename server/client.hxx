#pragma once
#include "net.hxx"

class Client
{
	Socket socket;

	void sendMessage(int errcode, std::string message);
	void sendAnswerHeader(bool success = true, int mc = 0);
	void sendAnswerHeader(int errcode, std::string message);
	void select(SelectionParams const &sp);

public:
	Client(Socket &&s);
	void operator() ();
};
