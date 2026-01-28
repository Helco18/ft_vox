#include "OpenGLEngine.hpp"
#include "Logger.hpp"
#include <iostream>

void OpenGLEngine::_createTexture(TextureBuffer & textureBuffer, TextureInfo & textureInfo)
{
	glGenTextures(1, &textureBuffer.tbo);
	glBindTexture(GL_TEXTURE_2D, textureBuffer.tbo);

	// Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		textureInfo.width,
		textureInfo.height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		textureInfo.data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
