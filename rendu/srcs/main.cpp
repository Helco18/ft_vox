/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/22 14:48:18 by scraeyme          #+#    #+#             */
/*   Updated: 2025/07/22 19:18:28 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <glm/glm.hpp>
#include "vulkan/VulkanEngine.hpp"
#include "colors.hpp"

GLFWwindow * getWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow * window = glfwCreateWindow(800, 600, "ft_vox", nullptr, nullptr);
	if (!window)
		exit(1); // log a faire
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
	}
}
