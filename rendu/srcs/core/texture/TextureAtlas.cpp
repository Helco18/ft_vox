#include "TextureAtlas.hpp"
#include "Logger.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "utils.hpp"
#include <cstring>

TextureAtlas::TextureMap TextureAtlas::_textureMap;
TextureAtlas::AtlasData TextureAtlas::_atlasData;
int TextureAtlas::_width = 0;
int TextureAtlas::_height = 0;

void TextureAtlas::createAtlas()
{
	int offsetX = 0.0f;

	if (_width == 0 || _height == 0 || _textureMap.empty())
	{
		Logger::log(TEXTURE, FATAL, "Texture Map is empty.");
		return;
	}
	_atlasData.resize(_width * _height * 4, 0);
	for (std::pair<const std::string, Texture *> & textures : _textureMap)
	{
		Texture * texture = textures.second;
		texture->uvMin.x = static_cast<float>(offsetX) / _width;
		texture->uvMin.y = 0.0f;
		texture->uvMax.x = texture->uvMin.x + static_cast<float>(texture->width) / _width;
		texture->uvMax.y = texture->uvMin.y + static_cast<float>(texture->height) / _height;

		for (int y = 0; y < texture->height; ++y)
		{
			for (int x = 0; x < texture->width; ++x)
			{
				int atlasIndex = ((y * _width) + (x + offsetX)) * 4;
				int texIndex = (y * texture->width + x) * 4;
				memcpy(&_atlasData[atlasIndex], &texture->data[texIndex], 4);
			}
		}
		offsetX += texture->width;
	}
	#ifdef DEBUG
	stbi_write_png("atlas_debug.png", _width, _height, 4, _atlasData.data(), _width * 4);
	#endif
	Logger::log(TEXTURE, INFO, "Texture Atlas created.");
}

void TextureAtlas::pushTexture(const std::string & texturePath)
{
	TextureMap::iterator it = _textureMap.find(texturePath);
	if (it != _textureMap.end())
	{
		Logger::log(TEXTURE, WARNING, "Texture already pushed: " + texturePath);
		return;
	}

	Texture * texture = new Texture;

	texture->data = stbi_load(texturePath.c_str(), &texture->width, &texture->height, &texture->colorChannels, STBI_rgb_alpha);
	if (!texture->data)
	{
		Logger::log(TEXTURE, ERROR, "Couldn't open texture file at: " + texturePath);
		delete texture;
		return;
	}
	_textureMap.emplace(texturePath, texture);
	if (_height < texture->height)
		_height = texture->height;
	_width += texture->width;
	Logger::log(TEXTURE, DEBUG, "Loaded texture at: " + texturePath);
}

Texture * TextureAtlas::getTexture(const std::string & texturePath)
{
	TextureMap::iterator it = _textureMap.find(texturePath);
	if (it == _textureMap.end())
	{
		Logger::log(TEXTURE, ERROR, "Couldn't find texture at: " + texturePath);
		return nullptr;
	}
	return it->second;
}

void TextureAtlas::destroy()
{
	for (std::pair<const std::string, Texture *> & textures : _textureMap)
	{
		stbi_image_free(textures.second->data);
		delete textures.second;
	}
}
