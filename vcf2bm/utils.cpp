#include "utils.h"
#include <sys/stat.h>

/* Splits input string at each occurence of char delim. */
std::vector<std::string> split_string(const std::string& input, const char delim, unsigned int expectedSize)
{
	std::string buffer{ "" };
	std::vector<std::string> result;
	result.reserve(expectedSize);
	for (auto c : input)
	{
		if (c != delim)
			buffer += c;
		else
		{
			result.push_back(buffer);
			buffer.clear();
		}
	}
	if (!buffer.empty()) // don't forget last part of string
		result.push_back(buffer);
	return result;
}


// taken from https://stackoverflow.com/a/868894
/* Checks whether arguments contain given option. */
bool has_cmd_option(char **begin, char **end, std::string option)
{
	return std::find(begin, end, option) != end;
}
/* Returns argument for option if it exists. */
char* get_cmd_option(char **begin, char **end, std::string option)
{
	char **itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
		return *itr;
	else
		return 0;
}
