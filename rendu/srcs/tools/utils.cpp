#include "utils.hpp"

std::vector<std::string> ft_split(const std::string & str, char delimiter)
{
	std::vector<std::string> tab;
	std::stringstream ss(str);
	std::string tmp;

	while (std::getline(ss, tmp, delimiter))
	{
		if (!tmp.empty())
			tab.push_back(tmp);
	}
	return tab;
}

const std::vector<char> readFile(const std::string & filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("Failed to open file!");

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}
