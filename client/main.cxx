#include <csignal>
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include "args.hxx"
#include "config.hxx"
#include "net.hxx"
#include "reader.hxx"
#include "struct.hxx"

char const *const PRE_Input = "> ";
char const *const PRE_Output = "< ";
char const *const PRE_Info = "# ";
char const *const PRE_Error = "! ";

class BatchModeRunner
{
private:
	typedef std::unique_ptr<NetworkQuery> PNQuery;
	bool stopping;
	std::mutex mtx;
	std::condition_variable cv;
	std::thread back;
	std::list<PNQuery> queries_in_progress;
	QueryReader rd;

	void run2()
	{
		for(;;)
		{
			std::unique_lock<std::mutex> guard(mtx);
			while(queries_in_progress.empty())
			{
				if(stopping)
					return;
				cv.wait(guard);
			}
			PNQuery q(std::move(queries_in_progress.front()));
			queries_in_progress.pop_front();
			guard.unlock();
			q->recv();
		}
	}

	void stop()
	{
		std::unique_lock<std::mutex> guard(mtx);
		stopping = true;
		guard.unlock();
		cv.notify_all();
	}

public:
	BatchModeRunner() :
		stopping(false),
		rd(std::cin),
		back(&BatchModeRunner::run2, this)
	{
	}

	~BatchModeRunner()
	{
		stop();
		back.join();
	}

	void run()
	{
		try
		{
			for(;;)
			{
				PQuery q(rd.readQuery());
				if(!q)
				{
					std::clog << "Read error" << std::endl;
					return;
				}
				std::clog << "Query: " << q->name() << std::endl;
				NetworkQuery *nq = dynamic_cast<NetworkQuery *>(q.get());
				if(nq)
				{
				// transfer the pointer (noexcept block)
					PNQuery pnq(nq);
					q.release();
				// end of noexcept block

					pnq->send();
					std::unique_lock<std::mutex> guard(mtx);
					queries_in_progress.emplace_back(std::move(pnq));
					guard.unlock();
					cv.notify_all();
				}
				else
				{
					q->perform();
				}
			}
		}
		catch(ExitException)
		{
			std::clog << "Exit" << std::endl;
			stop();
		}
	}
};

int main(int argc, char **argv)
{
	std::signal(SIGPIPE, SIG_IGN);
	arguments::read(argc, argv);
	try
	{
		bool batch = arguments::present("batch");
		std::string addr = arguments::get("addr", zolden_addr);
		std::string port = arguments::get("port", std::to_string(zolden_port));
		global_state.connection = Connect(addr, port);
		if(batch)
		{
			std::clog << "Connected to " << addr << ":" << port << std::endl;
			BatchModeRunner runner;
			runner.run();
			return 0;
		}
		try
		{
			std::cout << "Connected to " << addr << ":" << port << std::endl;
			QueryReader rd(std::cin);
			std::cout << std::endl;
			std::cout << PRE_Info << "Welcome to the Zolden database client!" << std::endl;
			std::cout << PRE_Info << std::endl;
			std::cout << PRE_Info << "Enter queries ending with ;" << std::endl;
			std::cout << PRE_Info << "Type \"help;\" for more information" << std::endl;
			for(;;)
			{
				try
				{
					std::cout << std::endl;
					std::cout << PRE_Input;
					PQuery q(rd.readQuery());
					if(!q)
						break;
					q->perform();
				}
				catch(InvalidQueryError const &e)
				{
					std::cout << PRE_Error << "Query error: " << e.what() << std::endl;
					rd.skipLine();
				}
				catch(RemoteError const &e)
				{
					std::cout << PRE_Error << "Server reported error: " << e.what() << std::endl;
					rd.skipLine();
				}
			}
		}
		catch(ExitException)
		{
			std::cout << std::endl << PRE_Info << "Good bye!" << std::endl;
		}
		catch(...)
		{
			std::cout << std::endl << PRE_Error;
			throw;
		}
	}
	catch(EofError const &e)
	{
		std::cout << "Unexpected end of input: " << e.what() << std::endl;
	}
	catch(ReaderError const &e)
	{
		std::cout << "Input read error: " << e.what() << std::endl;
	}
	catch(std::logic_error const &e)
	{
		std::cout << "Program logic error: " << e.what() << std::endl;
	}
	catch(std::runtime_error const &e)
	{
		std::cout << "Runtime error: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << "Invalid exception caught. Rethrowing to the standard library." << std::endl;
		throw;
	}
	return 0;
}
