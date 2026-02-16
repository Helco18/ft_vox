#include "OpenGLEngine.hpp"
#include "Logger.hpp"

void OpenGLEngine::_createTexture(TextureBuffer & textureBuffer, TextureInfo & textureInfo)
{
	glGenTextures(1, &textureBuffer.tbo);
	glBindTexture(GL_TEXTURE_2D, textureBuffer.tbo);

	// Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLValueConverter::getFiltering(textureInfo.filtering));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLValueConverter::getFiltering(textureInfo.filtering));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		textureInfo.width,
		textureInfo.height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		textureInfo.data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
