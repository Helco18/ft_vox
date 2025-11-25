#include "OpenGLEngine.hpp"
#include "OBJModel.hpp"
#include "colors.hpp"
#include <iostream>

void OpenGLEngine::_createTexture()
{
	OBJModel model = OBJModel::getModel(CUBE);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	// Wrapping mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, model.getTexture().width, model.getTexture().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, model.getTexture().data);
	glGenerateMipmap(GL_TEXTURE_2D);

	std::cout << GREEN << "[OK] Created Texture" << RESET << std::endl;
}
