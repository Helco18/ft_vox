#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include "CustomExceptions.hpp"
#include "OBJModel.hpp"
#include "Profiler.hpp"
#include "utils.hpp"
#include <iostream>

OpenGLEngine::OpenGLEngine(GLFWwindow * window, Camera * camera) : AEngine(window, camera) {}

OpenGLEngine::~OpenGLEngine()
{
	for (std::pair<AssetID, Asset *> assetPair : _assetMap)
	{
		Asset * asset = assetPair.second;
		glDeleteBuffers(1, &asset->vbo);
		glDeleteBuffers(1, &asset->ibo);
		glDeleteVertexArrays(1, &asset->assetID);
		for (UniformStream stream : asset->uniforms)
			glDeleteBuffers(1, &stream.ubo);
	}
	glDeleteTextures(1, &_texture);
	glDeleteBuffers(1, &_ubo);
	for (std::pair<std::string, GLuint> shaderPair : _shaderCache)
		glDeleteProgram(shaderPair.second);
}

AssetID OpenGLEngine::uploadAsset(Asset & asset, PipelineID pipelineID)
{
	PipelineMap::iterator it = _pipelineMap.find(pipelineID);
	if (it == _pipelineMap.end())
	{
		throw OpenGLException("Failed to upload asset because Pipeline ID: " +
			toString(pipelineID) + " doesn't exist.");
	}
	PipelineInfo & pipelineInfo = it->second;
	std::vector<Attribute> attributes = pipelineInfo.attributes;
	size_t offset = 0;

	glGenVertexArrays(1, &asset.assetID);
	glBindVertexArray(asset.assetID);

	glGenBuffers(1, &asset.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, asset.vbo);
	glBufferData(GL_ARRAY_BUFFER, asset.vertices.bytes.size(), asset.vertices.bytes.data(), GL_STATIC_DRAW);

	for (size_t i = 0; i < attributes.size(); ++i)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, attributes[i].count, GLValueConverter::getType(attributes[i].type), attributes[i].normalized,
		pipelineInfo.attributeSize, (void *)offset);
		offset += attributes[i].size;
	}

	for (UniformStream & futureUbo : asset.uniforms)
	{
		glGenBuffers(1, &futureUbo.ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, futureUbo.ubo);
		glBufferData(GL_UNIFORM_BUFFER, futureUbo.bytes.size(), futureUbo.bytes.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, futureUbo.binding, futureUbo.ubo);
	}

	glGenBuffers(1, &asset.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, asset.indices.size() * sizeof(uint32_t), asset.indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindVertexArray(0);

	_assetMap.try_emplace(asset.assetID, &asset);

	return asset.assetID;
}

void OpenGLEngine::load()
{
	Profiler p("OpenGLEngine::load");
	glfwSwapInterval(0);

	glGenBuffers(1, &_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo);

	_createTexture();

	if (g_debug)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(_debugCallback, this);
	}

	glEnable(GL_FRAMEBUFFER_SRGB);

	Logger::log(ENGINE_OPENGL, INFO, "OpenGL engine initialized successfully.");

	_isInitalized = true;
}

void OpenGLEngine::unloadAsset(AssetID assetID)
{
	AssetMap::iterator it = _assetMap.find(assetID);
	if (it != _assetMap.end())
	{
		Asset * asset = it->second;
		if (asset->vbo)
			glDeleteBuffers(1, &asset->vbo);
		if (asset->ibo)
			glDeleteBuffers(1, &asset->ibo);
		if (asset->assetID)
			glDeleteVertexArrays(1, &asset->assetID);
	}
}

PipelineID OpenGLEngine::uploadPipeline(PipelineInfo & pipelineInfo)
{
	int pipelineID = _pipelineMap.size();

	_pipelineMap.try_emplace(pipelineID, pipelineInfo);
	ShaderCache::iterator it = _shaderCache.find(pipelineInfo.shaderName);
	if (it == _shaderCache.end())
	{
		GLuint shader = _createShader(OPENGL_SHADER_PATH + pipelineInfo.shaderName + ".vert", OPENGL_SHADER_PATH + pipelineInfo.shaderName + ".frag");
		_shaderCache.try_emplace(pipelineInfo.shaderName, shader);
	}
	return pipelineID;
}

void OpenGLEngine::beginFrame()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_updateUniformBuffer();
}

void OpenGLEngine::_applyPipeline(PipelineID pipelineID)
{
	PipelineMap::iterator pipelineit = _pipelineMap.find(pipelineID);
	if (pipelineit == _pipelineMap.end())
	{
		Logger::log(ENGINE_OPENGL, WARNING, "Requested unknown pipeline with ID: " + toString(pipelineID));
		return;
	}
	PipelineInfo & pipelineInfo = pipelineit->second;

	// Polygon Mode
	switch (static_cast<int>(pipelineInfo.polygonMode))
	{
		case LINE: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
	}

	// Cull Mode
	switch (static_cast<int>(pipelineInfo.cullMode))
	{
		case BACK: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
		case OFF: glDisable(GL_CULL_FACE); break;
	}

	pipelineInfo.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	if (pipelineInfo.blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);

	ShaderCache::iterator shaderit = _shaderCache.find(pipelineInfo.shaderName);
	if (shaderit != _shaderCache.end())
		glUseProgram(shaderit->second);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	GLuint textureIndex = glGetUniformLocation(shaderit->second, "texture_0");
	if (textureIndex == GL_INVALID_INDEX)
		throw OpenGLException("Couldn't find texture uniform.");
	glUniform1i(textureIndex, 0);
}

void OpenGLEngine::drawAsset(AssetID assetID, PipelineID pipelineID)
{
	if (_isFramebufferResized)
		_handleResize();

	AssetMap::iterator it = _assetMap.find(assetID);
	if (it == _assetMap.end())
		return;

	Asset * asset = it->second;
	if (asset->vertices.bytes.empty())
		return;

	glBindVertexArray(assetID);

	for (UniformStream & stream : asset->uniforms)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, stream.ubo);
		glBindBufferBase(GL_UNIFORM_BUFFER, stream.binding, stream.ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, stream.bytes.size(), stream.bytes.data());
	}

	_applyPipeline(pipelineID);
	glDrawElements(GL_TRIANGLES, asset->indices.size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindVertexArray(0);
}

void OpenGLEngine::endFrame()
{
	glfwSwapBuffers(_window);
}

void OpenGLEngine::_handleResize()
{
	glViewport(0, 0, _camera->getWidth(), _camera->getHeight());
	_isFramebufferResized = false;
}
