#include "utils.hpp"
#include "CustomExceptions.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>

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
		throw GeneralException("Failed to open file: " + filename);

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

const std::string getFileAsString(const char * name)
{
	std::ifstream	file;
	std::string		buffer;
	std::string		content;
	std::string		filename;
	
	filename = name;
	file.open(filename, std::ios::in);
	if (file.fail())
		throw GeneralException("Couldn't find file: " + filename + ".");
	while (std::getline(file, buffer))
		content += buffer + '\n';
	file.close();
	if (content.empty())
		throw GeneralException("File " + filename + " is empty.");
	return (content);
}
