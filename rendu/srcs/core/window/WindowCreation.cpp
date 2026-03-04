#include "OpenGLEngine.hpp"
#include "WindowManager.hpp"
#include "CustomExceptions.hpp"
#include "stb/stb_image.h"
#include "Logger.hpp"

GLFWimage WindowManager::_decodeOneStep(const char * filename)
{
	GLFWimage image;
	int width, height, channels;
	unsigned char * pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

	if (!pixels)
	{
		image.pixels = nullptr;
		return image;
	}
	image.width = width;
	image.height = height;
	image.pixels = pixels;

	return image;
}

void WindowManager::_setIcon(GLFWwindow * window)
{
	GLFWimage images[2];

	stbi_set_flip_vertically_on_load(false);
	images[0] = _decodeOneStep("resources/assets/icon/icon.png");
	if (!images[0].pixels || images[0].height != images[0].width)
		throw WindowException("Invalid icon. It must be a valid file with matching height/width.");
	images[1] = _decodeOneStep("resources/assets/icon/icon_small.png");
	if (!images[1].pixels || images[1].height != images[1].width)
	{
		stbi_image_free(images[0].pixels);
		throw WindowException("Invalid icon. It must be a valid file with matching height/width.");
	}

	glfwSetWindowIcon(window, 2, images);

	stbi_image_free(images[0].pixels);
	stbi_image_free(images[1].pixels);
}

static void glfwErrorCallback(int error, const char * description)
{
	Logger::log(ENGINE_OPENGL, ERROR, "(" + toString(error) + ")" + description + ".");
}

GLFWwindow * WindowManager::_createWindow()
{
	if (glfwInit() == GLFW_FALSE)
		throw WindowException("Failed to create GLFW context.");

	if (glfwVulkanSupported() == GLFW_FALSE)
	{
		Logger::log(WINDOW, WARNING, "Vulkan not supported on this computer. Defaulting to OpenGL.");
		_engineType = OPENGL;
	}

	// Vu que GLFW créé un contexte OpenGL par défaut, on lui précise de ne pas le faire.
	if (_engineType == VULKAN)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	else
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	std::stringstream sstream;
	sstream << "[" << (ENGINE_NAME(_engineType)) << "]";
	GLFWwindow * window = glfwCreateWindow(_width, _height, sstream.str().c_str(), nullptr, nullptr);

	if (!window)
		throw WindowException("Failed to instantiate GLFW window.");

	if (_engineType == OPENGL)
	{
		glfwMakeContextCurrent(window);
		if (!gladLoadGL(glfwGetProcAddress))
			throw WindowException("Couldn't initialize GLAD.");
	}
	glfwSetWindowSizeLimits(window, WIDTH / 2, HEIGHT / 2, GLFW_DONT_CARE, GLFW_DONT_CARE);

	glfwSetErrorCallback(glfwErrorCallback);
	return (window);
}
