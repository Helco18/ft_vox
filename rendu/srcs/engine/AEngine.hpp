#pragma once

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "Camera.hpp"
#include <iterator>
#include <limits>
#include <chrono>
#include <valgrind/valgrind.h>
#include "OBJModel.hpp"
#include "colors.hpp"
#include "utils.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define ENGINE_NAME(x) x == VULKAN ? "Vulkan" : "OpenGL"

struct UniformBuffer
{
	glm::mat4 view;
	glm::mat4 proj;
};

class AEngine
{
	public:
		AEngine(GLFWwindow * window, Camera * camera) : _window(window), _camera(camera), _isFramebufferResized(false), _isInitalized(false) {};
		virtual ~AEngine() {}

		virtual void	load() = 0;
		virtual void	drawFrame() = 0;
		
		Camera *		getCamera() const { return _camera; }
		bool			getFramebufferResized() const { return _isFramebufferResized; }
		bool			isInitialized() const { return _isInitalized; }

		void			setFramebufferResized(bool framebufferResized) { _isFramebufferResized = framebufferResized; }
	protected:
		GLFWwindow *	_window;
		Camera *		_camera;
		bool			_isFramebufferResized;
		bool			_isInitalized;
};
