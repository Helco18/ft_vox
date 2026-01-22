#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include "CustomExceptions.hpp"
#include "OBJModel.hpp"
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
	glDeleteTextures(1, &_texture);
	for (std::pair<std::string, GLuint> shaderPair : _shaderCache)
		glDeleteProgram(shaderPair.second);
}

void OpenGLEngine::load()
{
	Profiler p("OpenGLEngine::load");
	glfwSwapInterval(0);

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

void OpenGLEngine::beginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void OpenGLEngine::endFrame()
{
	glfwSwapBuffers(_window);
}

void OpenGLEngine::_handleResize()
{
	WindowManager * windowManager = reinterpret_cast<WindowManager *>(glfwGetWindowUserPointer(_window));
	glViewport(0, 0, windowManager->getWidth(), windowManager->getHeight());
	_isFramebufferResized = false;
}
