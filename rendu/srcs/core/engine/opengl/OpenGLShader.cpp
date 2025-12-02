#include "OpenGLEngine.hpp"
#include "utils.hpp"
#include "colors.hpp"
#include "Logger.hpp"
#include <iostream>

static const std::string getShaderAsString(std::string path)
{
	path = SHADER_PATH"glsl/" + path;
	return getFileAsString(path.c_str());
}

static GLuint compileShader(GLenum type, const std::string & filepath)
{
	GLuint shader;
	int result;
	std::string shaderSrc;
	char * vertexStr;

	shader = glCreateShader(type);
	shaderSrc = getShaderAsString(filepath);
	vertexStr = (char *)shaderSrc.c_str();
	glShaderSource(shader, 1, &vertexStr, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		int error_length;
		char *error_message;
		// Get the log message's length in order to alloca the string to store it.
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);
		// Alloca is a malloc that frees itself once the function ends.
		error_message = (char *)alloca(sizeof(char) * error_length);
		// This will store the log of the program into the error_message string.
		glGetShaderInfoLog(shader, error_length, &error_length, error_message);
		Logger::log(ENGINE_OPENGL, CRITICAL, (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") + std::string("shader failed to compile: ") + error_message);
		// Delete the shader to avoid leaks.
		glDeleteShader(shader);
		return (0);
	}
	return shader;
}

void OpenGLEngine::_createShader(const std::string & vertexPath, const std::string & fragmentPath)
{
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
	_shader = glCreateProgram();
	glAttachShader(_shader, vertexShader);
	glAttachShader(_shader, fragmentShader);
	glLinkProgram(_shader);
	glValidateProgram(_shader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint uboIndex = glGetUniformBlockIndex(_shader, "block_UniformBuffer_0");
	if (uboIndex == GL_INVALID_INDEX)
		uboIndex = glGetUniformBlockIndex(_shader, "block_UniformBuffer_std140_0");
	if (uboIndex == GL_INVALID_INDEX)
		throw std::runtime_error("Couldn't find ubo index.");
	glUniformBlockBinding(_shader, uboIndex, 0);

	std::cout << GREEN << "[OK] Created Shaders" << RESET << std::endl;
}
