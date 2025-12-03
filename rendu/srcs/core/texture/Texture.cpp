#include "Texture.hpp"
#include "CustomExceptions.hpp"
#include "stb/stb_image.h"

Texture::Texture(const std::string & path)
{
	_data = stbi_load(path.c_str(), &_width, &_height, &_channels, STBI_rgb_alpha);
	if (!_data)
		throw TextureException("Couldn't load image: " + path);
}

Texture::~Texture()
{
	stbi_image_free(_data);
}
