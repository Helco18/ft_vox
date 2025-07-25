/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 14:48:18 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/25 18:11:38 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <glm/glm.hpp>
#include "vulkan/VulkanEngine.hpp"
#include "colors.hpp"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GLFWimage decodeOneStep(const char * filename)
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

static void setIcone(GLFWwindow * window)
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
	catch(const std::exception& e)
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

static GLFWwindow * createWindow()
{
	GLFWwindow * window = glfwCreateWindow(800, 600, "ft_vox", nullptr, nullptr);
	if (glfwVulkanSupported() == GLFW_FALSE)
		throw std::runtime_error("Vulkan is not supported by this system");
	if (!window)
		throw std::runtime_error("Failed to create GLFW window");

	# ifdef DEBUG
	glfwSetWindowTitle(window, "ft_vox [DEBUG]");
	# endif

	return window;
}

GLFWwindow * getWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow * window = createWindow();
	glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);
	setIcone(window);

	return (window);
}

int main()
{
	try
	{
		GLFWwindow * window = getWindow();
		VulkanEngine engine(window);
		engine.init();

		std::cout << GREEN << "Vulkan initialized" << RESET << std::endl;
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			glfwSwapBuffers(window);
		}
		engine.destroy();
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	catch (const std::exception & e)
	{
		std::cerr << RED << "[ERROR] " << e.what() << RESET << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
