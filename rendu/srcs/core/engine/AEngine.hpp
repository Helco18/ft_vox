#pragma once

#include <memory>
#include "Camera.hpp"
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

struct UniformStream
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
	AssetID						assetID;
	VertexStream				vertices;
	std::vector<uint32_t>		indices;
	std::vector<UniformStream>	uniforms;
	unsigned int				vbo;
	unsigned int				ibo;
};

struct UniformBuffer
{
	glm::mat4 view;
	glm::mat4 proj;
};

class AEngine
{
	public:
		AEngine(GLFWwindow * window, Camera * camera) :
			_window(window), _camera(camera), _isFramebufferResized(false), _isInitalized(false) {};
		virtual ~AEngine() {}

		virtual void		load() = 0;
		virtual void		beginFrame() = 0;
		virtual AssetID		uploadAsset(Asset & asset, PipelineID pipelineID) = 0;
		virtual void		unloadAsset(AssetID assetID) = 0;
		virtual PipelineID	uploadPipeline(PipelineInfo & pipelineInfo) = 0;
		virtual void		drawAsset(AssetID assetID, PipelineID pipelineID) = 0;
		virtual void		endFrame() = 0;

		GLFWwindow *		getWindow() const { return _window; }
		Camera *			getCamera() const { return _camera; }
		bool				getFramebufferResized() const { return _isFramebufferResized; }
		bool				isInitialized() const { return _isInitalized; }

		void				setFramebufferResized(bool framebufferResized) { _isFramebufferResized = framebufferResized; }

	protected:
		typedef std::unordered_map<AssetID, Asset *>	AssetMap;

		AssetMap			_assetMap;
		GLFWwindow *		_window;
		Camera *			_camera;
		bool				_isFramebufferResized;
		bool				_isInitalized;
};
