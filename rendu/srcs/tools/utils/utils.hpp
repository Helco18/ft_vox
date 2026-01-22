#pragma once

#include <sstream>
#include <vector>
#include <cstring>

#define WIDTH	800
#define HEIGHT	600

/* ************************************************************************** */
/*   general_tools                                                            */
/* ************************************************************************** */

/* * Converts a value to a string.
 * This is a utility function that can be used to convert any type that supports
 * the stream insertion operator (<<) to a string.
 */
template <typename T>
const std::string			toString(const T & value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

const std::vector<char>		readFile(const std::string & filename);
std::vector<std::string>	ft_split(const std::string & str, char delimiter);
const std::string			getFileAsString(const char * name);
void						generateImGui();
