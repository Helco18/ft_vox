/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 19:33:36 by gcannaud          #+#    #+#             */
/*   Updated: 2025/10/08 17:53:18 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "GLFW/glfw3.h"
#include "colors.hpp"

#define WIDTH 1280
#define HEIGHT 720

/* ************************************************************************** */
/*   general_tools                                                            */
/* ************************************************************************** */

/* * Converts a value to a string.
 * This is a utility function that can be used to convert any type that supports
 * the stream insertion operator (<<) to a string.
 */
template <typename T>
const std::string		toString(const T & value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

const std::vector<char>	readFile(const std::string & filename);

/* ************************************************************************** */
/*   GLFW tools                                                               */
/* ************************************************************************** */

/* * Initializes GLFW and creates a window.
 * This function sets up GLFW, configures window hints, and creates a window.
 * It also sets the window icon and handles any errors that may occur during
 * initialization or window creation.
 *
 * @return A pointer to the created GLFWwindow.
 * @throws std::runtime_error if GLFW initialization or window creation fails.
 */
GLFWwindow *			getWindow();
