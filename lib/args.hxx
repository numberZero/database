#pragma once
#include <cstdint>
#include <string>

namespace arguments
{
	void read(int argc, char **argv);

	inline namespace named
	{
		bool present(std::string const &name);
		std::string get(std::string const &name, std::string const &def = "");
	}

	inline namespace positional
	{
		std::string get(std::size_t index);
		std::size_t count();
	}
}
