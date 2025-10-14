#include "Model.hpp"

Model::ModelMap Model::_modelMap;

Model::Model(const std::string & path)
{
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	std::string tmp;
	std::ifstream file(path);
	std::vector<std::string> splitted;

	if (file.fail())
		throw std::runtime_error("No file found at given path: " + path);

	while (std::getline(file, tmp))
	{
		splitted = ft_split(tmp, ',');
		if (splitted.empty() || strncmp(splitted[0].c_str(), "//", 2) == 0)
			continue;
		if (strncmp(splitted[0].c_str(), "v ", 2) == 0 && splitted.size() >= 5)
		{
			glm::vec2 position(std::atof(&(splitted[0].c_str())[2]), std::atof(splitted[1].c_str()));
			glm::vec3 color(std::atof(splitted[2].c_str()), std::atof(splitted[3].c_str()), std::atof(splitted[4].c_str()));
			vertices.push_back({ position, color });
		}
		else if (strncmp(splitted[0].c_str(), "f ", 2) == 0 && splitted.size() >= 3)
		{
			splitted[0] = &splitted[0][2];
			for (size_t i = 0; i < splitted.size(); ++i)
				indices.push_back(std::atoi(splitted[i].c_str()));
		}
		else
		{
			file.close();
			throw std::runtime_error("Invalid line at: " + tmp);
		}
	}
	file.close();
	_vertices = vertices;
	_indices = indices;
}

const std::vector<Vertex> & Model::getVertices() const
{
	return _vertices;
}

const std::vector<uint16_t> & Model::getIndices() const
{
	return _indices;
}

Model Model::getModel(ModelType type)
{
	if (_modelMap.find(type) == _modelMap.end())
		throw std::runtime_error("Invalid ModelType: " + toString(type));
	return _modelMap[type];
}

void Model::loadModels()
{
	_modelMap[TRIANGLE] = Model("assets/models/triangle.obj");
}
