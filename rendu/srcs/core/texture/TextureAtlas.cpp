#include "TextureAtlas.hpp"
#include "AEngine.hpp"
#include "Logger.hpp"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "utils.hpp"

TextureAtlas::TextureMap TextureAtlas::_textureMap;
TextureAtlas::AtlasData TextureAtlas::_atlasData;
int TextureAtlas::_width = 0;
int TextureAtlas::_height = 0;
int TextureAtlas::_colorChannels = TARGET_COLOR_CHANNELS;

void TextureAtlas::createAtlas()
{
	int offsetX = 0;

	if (_width == 0 || _height == 0 || _textureMap.empty())
	{
		Logger::log(TEXTURE, FATAL, "Texture Map is empty.");
		return;
	}
	_atlasData.resize(_width * _height * _colorChannels, 255);
	const float normWidth = 1.0f / static_cast<float>(_width);
	const float normHeight = 1.0f / static_cast<float>(_height);
	for (const std::pair<const std::string, std::unique_ptr<Texture>> & textures : _textureMap)
	{
		const std::unique_ptr<Texture> & texture = textures.second;
		texture->uvMin.x = (offsetX + 0.5f) * normWidth;
		texture->uvMin.y = 0.5f * normHeight;
		texture->uvMax.x = (texture->width + offsetX - 0.5f) * normWidth;
		texture->uvMax.y = (texture->height - 0.5f) * normHeight;

		for (int y = 0; y < texture->height; ++y)
		{
			int atlasIndex = ((y * _width) + offsetX) * _colorChannels;
			int texIndex = y * texture->width * _colorChannels;
			memcpy(&_atlasData[atlasIndex], &texture->data[texIndex], texture->width * _colorChannels);
		}
		offsetX += texture->width;
	}
	if (g_debug == DebugLevel::DEBUG_MESSAGES)
		stbi_write_png("atlas_debug.png", _width, _height, _colorChannels, _atlasData.data(), _width * _colorChannels);
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

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	stbi_set_flip_vertically_on_load(true);
	texture->data = stbi_load(texturePath.c_str(), &texture->width, &texture->height, &texture->colorChannels, STBI_rgb_alpha);
	if (!texture->data)
	{
		Logger::log(TEXTURE, ERROR, "Couldn't open texture file at: " + texturePath);
		return;
	}
	if (_height < texture->height)
		_height = texture->height;
	_width += texture->width;
	_textureMap.emplace(texturePath, std::move(texture));
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
	return it->second.get();
}

void TextureAtlas::destroy()
{
	for (const std::pair<const std::string, std::unique_ptr<Texture>> & textures : _textureMap)
	{
		if (textures.second)
			stbi_image_free(textures.second->data);
	}
}
