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

struct Asset
{
	std::vector<Vertex> 	vertices;
	std::vector<uint32_t>	indices;
	AssetID					assetID;
	unsigned int			vbo;
	unsigned int			ibo;
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
