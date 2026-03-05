#include "utils.hpp"
#include "CustomExceptions.hpp"
#include <fstream>
#include <ostream>
#include <filesystem>

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

float getDistanceInterval(float min, float max, float value)
{
	if (value < min)
		return min - value;
	else if (value > max)
		return value - max;
	return 0;
}

uint64_t alignTo(uint64_t offset, uint64_t alignment)
{
	uint64_t remainder = offset % alignment;
	if (remainder == 0)
		return offset;
	return offset + (alignment - remainder);
}

bool fileExists(const std::string & path)
{
	try
	{
		if (!std::filesystem::exists(path) || (std::filesystem::status(path).permissions() & std::filesystem::perms::owner_read) == std::filesystem::perms::none)
			return false;
	} catch (const std::filesystem::filesystem_error & e)
	{
		return false;
	}
	return true;
}
