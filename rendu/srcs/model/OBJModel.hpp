#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

#include <filesystem>

enum ModelType
{
	TRIANGLE,
	TEST
};

struct Vertex
{
    glm::vec3	position;
    glm::vec3	normal;
    glm::vec2	texCoord;
    int			originalPositionIndex; 

};

struct Material
{
    std::string name;
    glm::vec3	diffuseColor = {1.0f, 1.0f, 1.0f};
    glm::vec3	ambientColor = {0.0f, 0.0f, 0.0f};
    glm::vec3	specularColor = {0.0f, 0.0f, 0.0f};
    float		shininess = 0.0f;
    std::string	diffuseTexturePath = "";
};

struct Mesh
{
    std::string				material;
    std::vector<uint16_t>	indices;
};

struct Texture
{
    int				width;
    int				height;
    int				channels;
    unsigned char *	data;
};

class OBJModel
{
    public:
		OBJModel() {};
        OBJModel(ModelType type);
        ~OBJModel();

        bool												load();
        const std::vector<Vertex> &							getVertices() const { return _vertices; }
        const std::vector<uint16_t> &						getIndices() const { return _indices; }
        const std::unordered_map<std::string, Mesh> &		getMeshes() const { return _meshes; }
        const std::unordered_map<std::string, Material> &	getMaterials() const { return _materials; }
		const Texture &										getTexture() const;

		static OBJModel										getModel(ModelType type);
    private:
		static std::unordered_map<ModelType, OBJModel>		_modelCache;	

        ModelType											_type;
        std::string											_filepath;
        std::vector<Vertex>									_vertices;
        std::vector<uint16_t>								_indices;

        std::unordered_map<std::string, uint16_t>			_uniqueVertexMap;

        std::vector<glm::vec3>								_temp_positions;
        std::vector<glm::vec3>								_temp_normals;
        std::vector<glm::vec2>								_temp_texcoords;

        std::unordered_map<std::string, Material>			_materials;
        std::string											_currentMaterialName;
        std::vector<std::string>							_materialForFace;

        std::unordered_map<std::string, Mesh>				_meshes;

        std::unordered_map<ModelType, Texture>				_loadedTextures;

        void												_parseLine(const std::string & line);
        uint16_t											_parseVertexIndex(const std::string & token);
        void												_loadMTL(const std::string & filename);
};
