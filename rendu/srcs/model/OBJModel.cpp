#include "OBJModel.hpp"
#include "utils.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

#include "stb/stb_image.h"

std::unordered_map<ModelType, OBJModel> OBJModel::_modelCache;
std::unordered_map<ModelType, Texture> OBJModel::_loadedTextures;

OBJModel::OBJModel(const std::string & filepath, ModelType type) : _filepath(filepath), _type(type)
{}

OBJModel::~OBJModel() {}

OBJModel OBJModel::getModel(ModelType type)
{
	if (_modelCache.find(type) == _modelCache.end())
		throw std::runtime_error("Invalid ModelType: " + toString(type));
	return _modelCache[type];
}

bool OBJModel::load()
{
	_uniqueVertexMap.clear();
	_indices.clear();

	_vertices.clear();
	_temp_positions.clear();
	_temp_texcoords.clear();
	_temp_normals.clear();
	_meshes.clear();
	_materials.clear();
	std::ifstream file(_filepath);
	if (!file.is_open())
	{
		std::cerr << "Erreur ouverture OBJ : " << _filepath << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line))
		_parseLine(line);

	_uniqueVertexMap.clear();

	std::vector<glm::vec3> temp_calculated_normals;
	temp_calculated_normals.resize(_temp_positions.size(), {0.0f, 0.0f, 0.0f});

	for (const auto& [matName, mesh] : _meshes)
	{
		const auto& _indices = mesh.indices;

		for (size_t i = 0; i < _indices.size(); i += 3)
		{
			size_t index1 = _indices[i];
			size_t index2 = _indices[i + 1];
			size_t index3 = _indices[i + 2];

			size_t posIndex1 = _vertices[index1].originalPositionIndex;
			size_t posIndex2 = _vertices[index2].originalPositionIndex;
			size_t posIndex3 = _vertices[index3].originalPositionIndex;
			if (posIndex1 >= _temp_positions.size() || posIndex2 >= _temp_positions.size() || posIndex3 >= _temp_positions.size())
			{
				std::cerr << "Erreur: Indice de position d'origine invalide lors du calcul de la normale" << std::endl;
				continue;
			}
			const glm::vec3& p1 = _temp_positions[posIndex1];
			const glm::vec3& p2 = _temp_positions[posIndex2];
			const glm::vec3& p3 = _temp_positions[posIndex3];

			glm::vec3 v1 = p2 - p1;
			glm::vec3 v2 = p3 - p1;

			glm::vec3 faceNormal = cross(v1, v2);

			temp_calculated_normals[posIndex1] = temp_calculated_normals[posIndex1] + faceNormal;
			temp_calculated_normals[posIndex2] = temp_calculated_normals[posIndex2] + faceNormal;
			temp_calculated_normals[posIndex3] = temp_calculated_normals[posIndex3] + faceNormal;
		}
	}

	for (glm::vec3& normal : temp_calculated_normals)
	{
		normal = normalize(normal);
		// if (normal.z < 0)
		//     normal = -normal;
	}
	for (size_t i = 0; i < _vertices.size(); ++i)
	{
		int posIndex = _vertices[i].originalPositionIndex;
		if (posIndex >= 0 && (size_t)posIndex < temp_calculated_normals.size())
		{
			_vertices[i].normal = temp_calculated_normals[posIndex];
		}
	}

	_modelCache[_type] = *this;
	return true;
}

void OBJModel::_parseLine(const std::string & line)
{
	std::istringstream iss(line);
	std::string prefix;
	iss >> prefix;

	if (prefix == "v")
	{
		glm::vec3 pos;
		iss >> pos.x >> pos.y >> pos.z;
		_temp_positions.push_back(pos);
	}
	else if (prefix == "vt")
	{
		glm::vec2 tex;
		iss >> tex.x >> tex.y;
		_temp_texcoords.push_back(tex);
	}
	else if (prefix == "vn")
	{
		glm::vec3 norm;
		iss >> norm.x >> norm.y >> norm.z;
		_temp_normals.push_back(norm);
	}
	else if (prefix == "f")
	{
		std::vector<uint32_t> face_indices;
		std::string token;
		while (iss >> token)
		{
			face_indices.push_back(_parseVertexIndex(token));
		}
		for (size_t i = 1; i + 1 < face_indices.size(); ++i)
		{
			_indices.push_back(face_indices[0]);
			_indices.push_back(face_indices[i]);
			_indices.push_back(face_indices[i + 1]);

			Mesh & mesh = _meshes[_currentMaterialName];
			mesh.material = _currentMaterialName;
			mesh.indices.push_back(face_indices[0]);
			mesh.indices.push_back(face_indices[i]);
			mesh.indices.push_back(face_indices[i + 1]);
		}
	}
	else if (prefix == "mtllib")
	{
		std::string mtlFile;
		iss >> mtlFile;
		_loadMTL(mtlFile);
	}
	else if (prefix == "usemtl")
	{
		iss >> _currentMaterialName;
	}
	else if (prefix == "g" || prefix == "o")
	{
		std::cout << "Ignoring line: " << line << std::endl;
	}
	else if (line.empty() || line[0] == '#')
	{
		// Ignore empty lines and comments
	}
}

uint32_t OBJModel::_parseVertexIndex(const std::string & token)
{
	auto it = _uniqueVertexMap.find(token);
	if (it != _uniqueVertexMap.end())
		return it->second;

	Vertex v{};
	std::istringstream ss(token);
	std::string indexStr;
	int idx[3] = {0, 0, 0};
	int i = 0;

	while (std::getline(ss, indexStr, '/'))
	{
		if (!indexStr.empty())
			idx[i] = std::stoi(indexStr);
		++i;
	}

	int posIndex = (idx[0] > 0) ? idx[0] - 1 : _temp_positions.size() + idx[0];
	int texIndex = (idx[1] > 0) ? idx[1] - 1 : _temp_texcoords.size() + idx[1];
	int normIndex = (idx[2] > 0) ? idx[2] - 1 : _temp_normals.size() + idx[2];

	v.position = (posIndex >= 0 && (size_t)posIndex < _temp_positions.size()) ? _temp_positions[posIndex] : glm::vec3{0.0f, 0.0f, 0.0f};
	v.texCoord = (texIndex >= 0 && (size_t)texIndex < _temp_texcoords.size()) ? _temp_texcoords[texIndex] : glm::vec2{0.0f, 0.0f};
	v.normal = (normIndex >= 0 && (size_t)normIndex < _temp_normals.size()) ? _temp_normals[normIndex] : glm::vec3{0.0f, 0.0f, 0.0f};
	v.originalPositionIndex = posIndex; 

	uint32_t newIndex = static_cast<uint32_t>(_vertices.size());
	_vertices.push_back(v);
	_uniqueVertexMap[token] = newIndex;

	// std::cout << "Nombre de matériaux utilisés : " << _meshes.size() << std::endl;
	// for (const auto& [name, mesh] : _meshes)
	// {
	//     std::cout << "  Matériau : " << name << ", triangles : " << mesh.indices.size() / 3 << std::endl;
	// }
	
	return newIndex;
}

void OBJModel::_loadMTL(const std::string & filename)
{
	std::filesystem::path objDir = std::filesystem::path(_filepath).parent_path();
	std::filesystem::path fullPath = objDir / filename;
	std::ifstream file(fullPath);

	if (!file.is_open())
	{
		std::cerr << "Erreur ouverture MTL : " << filename << std::endl;
		return;
	}
	std::cout << "Chargement du fichier MTL : " << filename << std::endl;

	std::string line;
	Material * current = nullptr;

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "newmtl")
		{
			std::string name;
			iss >> name;
			_materials[name] = Material{name};
			current = &_materials[name];
		}
		else if (prefix == "Kd" && current)
		{
			iss >> current->diffuseColor.x >> current->diffuseColor.y >> current->diffuseColor.z;
		}
		else if (prefix == "Ka" && current)
		{
			iss >> current->ambientColor.x >> current->ambientColor.y >> current->ambientColor.z;
		}
		else if (prefix == "Ks" && current)
		{
			iss >> current->specularColor.x >> current->specularColor.y >> current->specularColor.z;
		}
		else if (prefix == "Ns" && current)
		{
			iss >> current->shininess;
		}
		else if ((prefix == "map_Kd" || prefix == "map_Ks") && current)
		{
			iss >> current->diffuseTexturePath;

			std::filesystem::path fullTexturePath = objDir / current->diffuseTexturePath;

			if (_loadedTextures.find(_type) == _loadedTextures.end()) {
				Texture newTexture = {};
				stbi_set_flip_vertically_on_load(true);
				newTexture.data = stbi_load(fullTexturePath.string().c_str(), &newTexture.width, &newTexture.height, &newTexture.channels, STBI_rgb_alpha);

				if (newTexture.data)
				{
					std::cout << "Texture '" << current->diffuseTexturePath << "' chargee avec succes." << std::endl;
					_loadedTextures[_type] = newTexture;
				}
				else
					std::cerr << "Erreur de chargement de la texture : " << fullTexturePath << std::endl;
			}
		}
	}
}

const Texture & OBJModel::getTexture() const
{
	std::unordered_map<ModelType, Texture>::const_iterator it = _loadedTextures.find(_type);
	if (it != _loadedTextures.end()) {
		return it->second;
	}
	const static Texture emptyTexture = {0, 0, 0, nullptr};
	return emptyTexture;
}

bool OBJModel::loadModels()
{
	_modelCache[CUBE] = OBJModel("assets/models/cube.obj", CUBE);

	for (std::unordered_map<ModelType, OBJModel>::iterator it = _modelCache.begin(); it != _modelCache.end(); ++it)
	{
		if (!it->second.load())
			return false;
	}
	return true;
}

void OBJModel::deleteModels()
{
	for (std::unordered_map<ModelType, Texture>::const_iterator it = _loadedTextures.begin(); it != _loadedTextures.end(); ++it)
	{
		if (it->second.data)
			stbi_image_free(it->second.data);
	}
}
