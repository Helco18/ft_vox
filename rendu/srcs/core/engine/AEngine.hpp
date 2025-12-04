#pragma once

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "Camera.hpp"
#include "OBJModel.hpp"

#define ENGINE_NAME(x) x == VULKAN ? "Vulkan" : "OpenGL"
#define SHADER_PATH "srcs/core/shaders/"

typedef unsigned int AssetID;

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
		AEngine(GLFWwindow * window, Camera * camera, bool isWireframeEnabled) :
			_window(window), _camera(camera), _isFramebufferResized(false), _isInitalized(false), _isWireframeEnabled(isWireframeEnabled) {};
		virtual ~AEngine() {}

		virtual void	load() = 0;
		virtual void	beginFrame() = 0;
		virtual AssetID	uploadAsset(Asset & asset) = 0;
		virtual void	uploadTexture(OBJModel & model) = 0;
		virtual void	uploadTexture(Texture & texture) = 0;
		virtual void	drawAsset(AssetID assetID) = 0;
		virtual void	endFrame() = 0;

		Camera *		getCamera() const { return _camera; }
		bool			getFramebufferResized() const { return _isFramebufferResized; }
		bool			isInitialized() const { return _isInitalized; }
		bool			isWireframeEnabled() const { return _isWireframeEnabled; }

		void			toggleWireframe() { _isWireframeEnabled = !_isWireframeEnabled; }

		void			setFramebufferResized(bool framebufferResized) { _isFramebufferResized = framebufferResized; }

	protected:
		typedef std::unordered_map<AssetID, Asset> AssetMap;

		AssetMap		_assetMap;
		GLFWwindow *	_window;
		Camera *		_camera;
		bool			_isFramebufferResized;
		bool			_isInitalized;
		bool			_isWireframeEnabled = false;
};
