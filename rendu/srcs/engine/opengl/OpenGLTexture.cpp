#include "OpenGLEngine.hpp"

void OpenGLEngine::_createTexture()
{
	OBJModel model = OBJModel::getModel(CUBE);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, model.getTexture().width, model.getTexture().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, model.getTexture().data);
	glGenerateMipmap(GL_TEXTURE_2D);
}
