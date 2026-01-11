#include "OpenGLEngine.hpp"
#include "utils.hpp"
#include "Logger.hpp"
#include "CustomExceptions.hpp"
#include <iostream>

static GLuint compileShader(GLenum type, const std::string & filepath)
{
	GLuint shader;
	int result;
	std::string shaderSrc;
	char * vertexStr;

	shader = glCreateShader(type);
	shaderSrc = getFileAsString(filepath.c_str());
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
		// Delete the shader to avoid leaks.
		glDeleteShader(shader);
		throw OpenGLException((type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") +
			std::string("shader failed to compile: ") + error_message + ".");
	}
	return shader;
}

GLuint OpenGLEngine::_createShader(const std::string & vertexPath, const std::string & fragmentPath)
{
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
	GLuint shader = glCreateProgram();
	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint uboIndex = glGetUniformBlockIndex(shader, "block_UniformBuffer_0");
	if (uboIndex == GL_INVALID_INDEX)
		uboIndex = glGetUniformBlockIndex(shader, "block_UniformBuffer_std140_0");
	if (uboIndex == GL_INVALID_INDEX)
		throw OpenGLException("Couldn't find ubo index.");
	glUniformBlockBinding(shader, uboIndex, 0);

	Logger::log(ENGINE_OPENGL, INFO, "Created Shader ID: " + toString(shader) + ".");
	return shader;
}
