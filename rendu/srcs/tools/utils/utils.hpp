/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scraeyme <scraeyme@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 19:33:36 by gcannaud          #+#    #+#             */
/*   Updated: 2026/01/14 11:54:04 by scraeyme         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sstream>
#include <vector>
#include <cstring>

#define WIDTH	800
#define HEIGHT	600

/* ************************************************************************** */
/*   general_tools                                                            */
/* ************************************************************************** */

/* * Converts a value to a string.
 * This is a utility function that can be used to convert any type that supports
 * the stream insertion operator (<<) to a string.
 */
template <typename T>
const std::string			toString(const T & value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

template <typename T>
std::vector<std::byte>		valueToBytes(const T & value)
{
	std::vector<std::byte> bytes(sizeof(T));
	memcpy(bytes.data(), &value, sizeof(T));
	return bytes;
}

template <typename T>
std::vector<std::byte>		vectorToBytes(const std::vector<T> & value)
{
	return { reinterpret_cast<const std::byte *>(value.data()),
		reinterpret_cast<const std::byte *>(value.data()) + value.size() * sizeof(T)};
}

const std::vector<char>		readFile(const std::string & filename);
std::vector<std::string>	ft_split(const std::string & str, char delimiter);
const std::string			getFileAsString(const char * name);
