#pragma once

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "Camera.hpp"
#include "OBJModel.hpp"

#define ENGINE_NAME(x) x == VULKAN ? "Vulkan" : "OpenGL"
#define SHADER_PATH "srcs/core/shaders/"

typedef unsigned int AssetID;
typedef unsigned int PipelineID;

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

enum PolygonMode
{
	FILL,
	LINE
};

enum CullMode
{
	BACK,
	OFF
};

struct PipelineInfo
{
	std::string		shaderPath;
	PolygonMode		polygonMode = FILL;
	CullMode		cullMode = BACK;
	int				width = -1;
	int				height = -1;
	bool			blend = true;
	bool			depthTest = true;
};

class AEngine
{
	public:
		AEngine(GLFWwindow * window, Camera * camera) :
			_window(window), _camera(camera), _isFramebufferResized(false), _isInitalized(false) {};
		virtual ~AEngine() {}

		virtual void		load() = 0;
		virtual void		beginFrame() = 0;
		virtual AssetID		uploadAsset(Asset & asset) = 0;
		virtual PipelineID	uploadPipeline(PipelineInfo & pipelineInfo) = 0;
		virtual void		drawAsset(AssetID assetID, PipelineID pipelineID) = 0;
		virtual void		endFrame() = 0;

		Camera *			getCamera() const { return _camera; }
		bool				getFramebufferResized() const { return _isFramebufferResized; }
		bool				isInitialized() const { return _isInitalized; }

		void				setFramebufferResized(bool framebufferResized) { _isFramebufferResized = framebufferResized; }

	protected:
		typedef std::unordered_map<AssetID, Asset> AssetMap;

		AssetMap			_assetMap;
		GLFWwindow *		_window;
		Camera *			_camera;
		bool				_isFramebufferResized;
		bool				_isInitalized;
};
