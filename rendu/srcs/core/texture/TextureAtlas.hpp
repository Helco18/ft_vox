#pragma once

#include <string>
#include <memory>
#include "glm/glm.hpp"

#define TARGET_COLOR_CHANNELS 4

struct Texture
{
	int				width;
	int				height;
	int				colorChannels;
	unsigned char *	data;
	glm::vec2		uvMin;
	glm::vec2		uvMax;
};

class TextureAtlas
{
	public:

		TextureAtlas() = delete;
		~TextureAtlas() = delete;
	
		static void				pushTexture(const std::string & texturePath);
		static void				pushFolder(const std::string & folderPath);
		static void				createAtlas();

		static Texture *		getTexture(const std::string & texturePath);
		static unsigned char *	getData() { return _atlasData.data(); };
		static int				getWidth() { return _width; };
		static int				getHeight() { return _height; };
		static int				getColorChannels() { return _colorChannels; };

		static void				destroy();

	private:
		typedef std::unordered_map<std::string, std::unique_ptr<Texture>>	TextureMap;
		typedef std::vector<unsigned char>									AtlasData;

		static TextureMap		_textureMap;
		static AtlasData		_atlasData;
		static int				_width;
		static int				_height;
		static int				_colorChannels;

};
