#pragma once

#include <memory>
#include "OBJModel.hpp"
#include "PipelineManager.hpp"

#define ENGINE_NAME(x) x == VULKAN ? "Vulkan" : "OpenGL"

inline bool g_debug = false;

enum EngineType
{
	VULKAN,
	OPENGL
};

typedef unsigned int AssetID;

class GLFWwindow;

struct UniformBufferStream
{
	void *					data;
	size_t					size;
	int						binding;
	unsigned int			ubo;
};

struct VertexStream
{
	void *					data;
	uint32_t				vertexCount;
	uint32_t				stride;
	uint32_t				size;
};

struct Asset
{
	AssetID								assetID;
	VertexStream						vertices;
	std::vector<uint32_t>				indices;
	unsigned int						vbo;
	unsigned int						ibo;
	std::vector<UniformBufferStream>	uniformBuffers;
	void *								uniforms = nullptr;
};

struct UniformBuffer
{
	glm::mat4 view;
	glm::mat4 proj;
};

class AEngine
{
	public:
		AEngine(GLFWwindow * window) :
			_window(window), _isFramebufferResized(false), _isInitalized(false) {};
		virtual ~AEngine() {}

		virtual void		load() = 0;
		virtual void		beginFrame() = 0;
		virtual AssetID		uploadAsset(Asset & asset, PipelineID pipelineID) = 0;
		virtual void		unloadAsset(AssetID assetID) = 0;
		virtual PipelineID	uploadPipeline(PipelineInfo & pipelineInfo) = 0;
		virtual void		updateUniformBuffer(PipelineID pipelineID, void * data, size_t size) = 0;
		virtual void		drawAsset(AssetID assetID, PipelineID pipelineID) = 0;
		virtual void		endFrame() = 0;

		EngineType			getEngineType() const { return _engineType; }
		GLFWwindow *		getWindow() const { return _window; }
		bool				getFramebufferResized() const { return _isFramebufferResized; }
		bool				isInitialized() const { return _isInitalized; }

		void				setFramebufferResized(bool framebufferResized) { _isFramebufferResized = framebufferResized; }

	protected:
		typedef std::unordered_map<AssetID, Asset *>	AssetMap;

		AssetMap			_assetMap;
		EngineType			_engineType;
		GLFWwindow *		_window;
		bool				_isFramebufferResized;
		bool				_isInitalized;
};
