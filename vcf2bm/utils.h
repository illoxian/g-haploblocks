#pragma once
#include <vector>
#include <string>
#include <algorithm>

/* Splits input string at each occurence of char delim. */
std::vector<std::string> split_string(const std::string& input, char delim, unsigned int expectedSize = 1);

/* Checks whether arguments contain given option. */
bool has_cmd_option(char **begin, char **end, std::string option);

/* Returns argument for option if it exists. */
char* get_cmd_option(char **begin, char **end, std::string option);
