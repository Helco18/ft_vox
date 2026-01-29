#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include "CustomExceptions.hpp"
#include "Profiler.hpp"
#include "WindowManager.hpp"
#include "utils.hpp"
#include <iostream>

OpenGLEngine::OpenGLEngine(GLFWwindow * window) : AEngine(window) { _engineType = OPENGL; }

OpenGLEngine::~OpenGLEngine()
{
	for (std::pair<AssetID, AssetInfo> assetPair : _assetCache)
	{
		AssetInfo & assetInfo = assetPair.second;
		Asset * asset = assetInfo.asset;
		glDeleteBuffers(1, &asset->vbo);
		glDeleteBuffers(1, &asset->ibo);
		glDeleteVertexArrays(1, &asset->assetID);
		for (UniformBufferStream & uniformInfo : assetInfo.uniformBufferStreams)
			glDeleteBuffers(1, &uniformInfo.ubo);
	}
	for (std::pair<std::string, GLuint> shaderPair : _shaderCache)
		glDeleteProgram(shaderPair.second);
	for (std::pair<PipelineID, PipelineLayout> pipelinePair : _pipelineMap)
	{
		for (std::pair<const unsigned int, UniformBufferStream> & uniformPair : pipelinePair.second.uniformBufferStreams)
			glDeleteBuffers(1, &uniformPair.second.ubo);
		for (std::pair<const unsigned int, TextureBuffer> & texturePair : pipelinePair.second.textureBuffers)
			glDeleteTextures(1, &texturePair.second.tbo);
	}
	_shutdownImGui();
	_isInitalized.store(false);
}

void OpenGLEngine::load()
{
	Profiler p("OpenGLEngine::load");
	glfwSwapInterval(0);

	if (g_debug)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(_debugCallback, this);
	}
	glEnable(GL_MULTISAMPLE);

	_initImGui();

	_isInitalized = true;
	Logger::log(ENGINE_OPENGL, INFO, "OpenGL engine initialized successfully.");
}

void OpenGLEngine::beginFrame()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_pipelineAssetMap.clear();
}

void OpenGLEngine::endFrame()
{
	for (std::pair<const PipelineID, std::vector<Asset *>> & pipelinePair : _pipelineAssetMap)
	{
		PipelineMap::iterator pipelineit = _pipelineMap.find(pipelinePair.first);
		if (pipelineit == _pipelineMap.end())
			throw OpenGLException("Requested unknown pipeline with ID: " + toString(pipelinePair.first));

		PipelineLayout & pipelineLayout = pipelineit->second;
		_applyPipeline(pipelineLayout);

		for (Asset * asset : pipelinePair.second)
		{
			if (!asset->vertices.data)
				return;

			glBindVertexArray(asset->assetID);

			AssetInfo & assetInfo = _assetCache[asset->assetID];
			for (UniformBufferStream & uniformInfo : assetInfo.uniformBufferStreams)
			{
				glBindBuffer(GL_UNIFORM_BUFFER, uniformInfo.ubo);
				glBindBufferBase(GL_UNIFORM_BUFFER, uniformInfo.binding, uniformInfo.ubo);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, uniformInfo.size, uniformInfo.data);
			}

			GLenum polygonMode = GLValueConverter::getDrawMode(pipelineLayout.pipelineInfo.drawMode);
			if (!asset->indices.empty())
				glDrawElements(polygonMode, asset->indices.size(), GL_UNSIGNED_INT, 0);
			else
				glDrawArrays(polygonMode, 0, asset->vertices.vertexCount);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindVertexArray(0);
		}
	}
	_renderImGui();
	glfwSwapBuffers(_window);
}

void OpenGLEngine::_handleResize()
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(_window));
	glViewport(0, 0, windowManager->getWidth(), windowManager->getHeight());
	_isFramebufferResized = false;
}
