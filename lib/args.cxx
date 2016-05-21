#include "args.hxx"
#include <vector>
#include <map>

static std::map<std::string, std::string> nargs;
static std::vector<std::string> pargs;

void arguments::read(int argc, char **argv)
{
	int k;
	pargs.reserve(argc);
	std::string key;
	for(k = 1; k != argc; ++k)
	{
		std::string value(argv[k]);
		if(value.substr(0, 2) == "--")
		{
			if(value == "--")
				break;
			std::size_t pos = value.find('=', 2);
			if(pos == std::string::npos)
			{
				key = value.substr(2);
				nargs.emplace(key, "");
			}
			else
			{
				key.clear();
				nargs.emplace(value.substr(2, pos), value.substr(pos + 1));
			}
			continue;
		}
		if(!key.empty())
			nargs.at(key) = std::move(value);
		else
			pargs.push_back(std::move(value));
		key.clear();
	}
	pargs.shrink_to_fit();
}

bool arguments::named::present(std::string const &name)
{
	auto iter = nargs.find(name);
	return iter != nargs.end();
}

std::string arguments::named::get(std::string const &name, std::string const &def)
{
	auto iter = nargs.find(name);
	if(iter != nargs.end())
		return iter->second;
	return def;
}

std::size_t arguments::positional::count()
{
	return pargs.size();
}

std::string arguments::positional::get(std::size_t index)
{
	return pargs.at(index);
}
