#pragma once

#include "AEngine.hpp"
#include "glad/gl.h"
#include <GL/glext.h>
#include "GLFW/glfw3.h"
#include <string>

#define OPENGL_SHADER_PATH "resources/shaders/glsl/"

struct GLValueConverter
{
	static constexpr GLenum getType(AttributeType type)
	{
		switch (static_cast<int>(type))
		{
			case FLOAT3:
			case FLOAT2: return GL_FLOAT;
			case INT: return GL_INT;
		}
		return 0;
	}
};

struct TextureBuffer
{
	GLuint		tbo;
	std::string	name;
};

struct PipelineLayout
{
	PipelineInfo											pipelineInfo;
	std::unordered_map<unsigned int, UniformBufferStream>	uniformBufferStreams;
	std::unordered_map<unsigned int, TextureBuffer>			textureBuffers;
};

struct AssetInfo
{
	Asset *								asset;
	std::vector<UniformBufferStream>	uniformBufferStreams;
};

class OpenGLEngine : public AEngine
{
	public:
		OpenGLEngine(GLFWwindow * window);
		~OpenGLEngine();

		void		load() override;
		void		beginFrame() override;
		AssetID		uploadAsset(Asset & asset, PipelineID pipelineID) override;
		void		unloadAsset(AssetID assetID) override;
		PipelineID	uploadPipeline(PipelineInfo & pipelineInfo) override;
		void		updateUniformBuffer(PipelineID pipelineID, unsigned int binding, void * data, size_t size) override;
		void		drawAsset(AssetID assetID, PipelineID pipelineID) override;
		void		endFrame() override;

		void		beginImGui() override;
	
	private:
		typedef std::unordered_map<PipelineID, PipelineLayout>	PipelineMap;
		typedef std::unordered_map<std::string, GLuint>			ShaderCache;
		typedef std::unordered_map<AssetID, AssetInfo>			AssetCache;

		PipelineMap	_pipelineMap;
		ShaderCache	_shaderCache;
		AssetCache	_assetCache;

		static void	_debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *);
		GLuint		_createShader(const std::string & vertexPath, const std::string & fragmentPath, PipelineInfo & pipelineInfo);
		void		_handleResize();
		void		_createTexture(TextureBuffer & textureBuffer, TextureInfo & textureInfo);
		void		_applyPipeline(PipelineID pipelineID);

		// ImGui
		void		_initImGui();
		void		_renderImGui();
		void		_shutdownImGui();

};
