#include "utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

static GLFWimage decodeOneStep(const char * filename)
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

static void setIcon(GLFWwindow * window)
{
	GLFWimage images[2];

	try
	{
		images[0] = decodeOneStep("assets/icon.png");
		images[1] = decodeOneStep("assets/icon_small.png");
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

static void	glfwErrorCallback(int error, const char* description)
{
    std::cerr << RED << "[GLFW ERROR] (" << error << ") " << description << RESET << std::endl;
}

static GLFWwindow * createWindow()
{
    // Width, height, titre de la fenêtre, écran sur lequel afficher (GLFWMonitor, nullptr pour défaut)
    // Le dernier paramètre est pour OpenGL, on met donc nullptr.
    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "scop", nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Failed to instantiate GLFW window.");
    if (glfwVulkanSupported() == GLFW_FALSE)
		throw std::runtime_error("Vulkan is not supported by this system");

	# ifdef DEBUG
	glfwSetWindowTitle(window, "scop [DEBUG]");
	# endif

    return window;
}

GLFWwindow * getWindow()
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (glfwInit() == GLFW_FALSE)
        throw std::runtime_error("Failed to create GLFW context.");

    // Vu que GLFW créé un contexte OpenGL par défaut, on lui précise de ne pas le faire.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow * window = nullptr;

	try
	{
		window = createWindow();
		glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);
		setIcon(window);
	}
	catch(const std::exception& e)
	{
		glfwTerminate();
		throw;
	}
	

	return (window);
}
