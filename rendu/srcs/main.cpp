/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gcannaud <gcannaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 14:48:18 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/23 18:27:39 by gcannaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <glm/glm.hpp>
#include "vulkan/VulkanEngine.hpp"
#include "colors.hpp"
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

GLFWimage decodeOneStep(const char* filename) {
	GLFWimage image;
	int width, height, channels;
	unsigned char* pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels) {
		std::cerr << RED << "[ERROR] Failed to load image: " << filename << RESET << std::endl;
		exit(1);
	}
	image.width = width;
	image.height = height;
	image.pixels = pixels;

    return image;
}

GLFWwindow * getWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow * window = glfwCreateWindow(800, 600, "ft_vox", nullptr, nullptr);
	# ifdef DEBUG
	glfwSetWindowTitle(window, "ft_vox [DEBUG]");
	# endif
	if (glfwVulkanSupported() == GLFW_FALSE)
	{
		std::cerr << RED << "[ERROR] Vulkan is not supported" << RESET << std::endl;
		exit(1);
	}
	if (!window)
	{
		std::cerr << RED << "[ERROR] Failed to create GLFW window" << RESET << std::endl;
		exit(1);
	}

	glfwSetWindowSizeLimits(window, 400, 300, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// Set window icon
	GLFWimage images[2];
	images[0] = decodeOneStep("assets/icon.png");
	images[1] = decodeOneStep("assets/icon_small.png");
	glfwSetWindowIcon(window, 2, images);
	// Free the image data after setting the icon
	stbi_image_free(images[0].pixels);
	stbi_image_free(images[1].pixels);

	return (window);
}

int main()
{
	try
	{
		GLFWwindow * window = getWindow();
		if (!window)
		{
			std::cerr << RED << "[ERROR] Failed to create GLFW window" << RESET << std::endl;
			return -1;
		}
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
	}
}
