#include "OpenGLEngine.hpp"
#include "OBJModel.hpp"
#include "Logger.hpp"
#include "TextureAtlas.hpp"
#include <iostream>

void OpenGLEngine::_createTexture()
{
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		TextureAtlas::getWidth(),
		TextureAtlas::getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		TextureAtlas::getData());
	glGenerateMipmap(GL_TEXTURE_2D);

	Logger::log(ENGINE_OPENGL, INFO, "Created Texture.");
}
