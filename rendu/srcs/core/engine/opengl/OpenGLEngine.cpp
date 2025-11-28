#include "OpenGLEngine.hpp"
#include "colors.hpp"
#include "OBJModel.hpp"
#include <iostream>

OpenGLEngine::OpenGLEngine(GLFWwindow * window, Camera * camera) : AEngine(window, camera) {}

OpenGLEngine::~OpenGLEngine() {}

AssetID OpenGLEngine::upload(Asset & asset)
{
	glGenVertexArrays(1, &asset.assetID);
	glBindVertexArray(asset.assetID);

	glGenBuffers(1, &asset.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, asset.vbo);
	glBufferData(GL_ARRAY_BUFFER, asset.vertices.size() * sizeof(Vertex), asset.vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));

	glGenBuffers(1, &asset.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, asset.indices.size() * sizeof(uint32_t), asset.indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	_assetMap.try_emplace(asset.assetID, asset);

	return asset.assetID;
}

void OpenGLEngine::load()
{
	glfwSwapInterval(0);

	_createShader("base.vert", "base.frag");
	glUseProgram(_shader);

	glGenBuffers(1, &_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo);

	_createTexture();

	glBindVertexArray(0);

	std::cout << GREEN << "[OK] OpenGL engine initialized successfully." << RESET << std::endl;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	_isInitalized = true;
}

void OpenGLEngine::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLEngine::drawAsset(AssetID assetID)
{
	if (_isFramebufferResized)
		_handleResize();

	AssetMap::iterator it = _assetMap.find(assetID);
	if (it == _assetMap.end())
		return;

	Asset asset = it->second;
	
	glBindVertexArray(assetID);
	_updateUniformBuffer();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	GLuint textureIndex = glGetUniformLocation(_shader, "texture_0");
	if (textureIndex == GL_INVALID_INDEX)
		throw std::runtime_error("Couldn't find texture uniform.");
	glUniform1i(textureIndex, 0);

	glDrawElements(GL_TRIANGLES, asset.indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void OpenGLEngine::endFrame()
{
	glfwSwapBuffers(_window);
}

void OpenGLEngine::_handleResize()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, _camera->getWidth(), _camera->getHeight());
	_isFramebufferResized = false;
}
