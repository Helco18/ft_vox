#pragma once

#include "glm/glm.hpp"
#include <string>
#include <cstring>
#include <exception>
#include "utils.hpp"

enum ModelType
{
	TRIANGLE
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;
};

class Model
{
	public:
		Model() {};
		Model(const std::string & path);

		const std::vector<Vertex> &		getVertices() const;
		const std::vector<uint16_t> &	getIndices() const;

		static void						loadModels();
		static Model					getModel(ModelType type);
	private:
		typedef std::unordered_map<ModelType, Model> ModelMap;

		std::vector<Vertex>				_vertices;
		std::vector<uint16_t>			_indices;

		static ModelMap					_modelMap;
};
