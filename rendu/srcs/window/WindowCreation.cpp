#include "WindowManager.hpp"
#include "stb/stb_image.h"

GLFWimage WindowManager::_decodeOneStep(const char * filename)
{
	GLFWimage image;
	int width, height, channels;
	unsigned char* pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

	if (!pixels)
		throw std::runtime_error("Failed to load image : " + std::string(filename));
	image.width = width;
	image.height = height;
	image.pixels = pixels;

	return image;
}

void WindowManager::_setIcon(GLFWwindow * window)
{
	GLFWimage images[2];

	try
	{
		images[0] = _decodeOneStep("assets/icon/icon.png");
		images[1] = _decodeOneStep("assets/icon/icon_small.png");
		if (images[0].height != images[0].width || images[1].height != images[1].width)
			throw std::runtime_error("Icone size mismatch");

		glfwSetWindowIcon(window, 2, images);
	}
	catch(const std::exception & e)
	{
		if (images[0].pixels)
			stbi_image_free(images[0].pixels);
		if (images[1].pixels)
			stbi_image_free(images[1].pixels);
		throw;
	}

	stbi_image_free(images[0].pixels);
	stbi_image_free(images[1].pixels);
}

static void glfwErrorCallback(int error, const char * description)
{
	std::cerr << RED << "[GLFW ERROR] (" << error << ") " << description << RESET << std::endl;
}

GLFWwindow * WindowManager::_createWindow()
{
	if (glfwInit() == GLFW_FALSE)
		throw std::runtime_error("Failed to create GLFW context.");

	if (glfwVulkanSupported() == GLFW_FALSE)
	{
		std::cout << YELLOW << "[Warning] Vulkan not supported on this computer. Defaulting to OpenGL." << RESET << std::endl;
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
	}

	std::stringstream sstream;
	sstream << "[" << (ENGINE_NAME(_engineType)) << "]";
	GLFWwindow * window = glfwCreateWindow(_width, _height, sstream.str().c_str(), nullptr, nullptr);

	try
	{
		if (!window)
			throw std::runtime_error("Failed to instantiate GLFW window.");

		if (_engineType == OPENGL)
		{
			glfwMakeContextCurrent(window);
			if (glewInit() != GLEW_OK)
				throw std::runtime_error("Couldn't initialize GLEW.");
		}
		glfwSetWindowSizeLimits(window, WIDTH / 2, HEIGHT / 2, GLFW_DONT_CARE, GLFW_DONT_CARE);
		_setIcon(window);
	}
	catch(const std::exception & e)
	{
		glfwTerminate();
		throw;
	}

	glfwSetErrorCallback(glfwErrorCallback);
	return (window);
}
