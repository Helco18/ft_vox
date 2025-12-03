#pragma once

#include <string>

class Texture
{
	public:
		Texture(const std::string & path);
		~Texture();

		unsigned char *	getData() const { return _data; }
		int				getWidth() const { return _width; }
		int				getHeight() const { return _height; }
		int				getChannels() const { return _channels; }

	private:
		unsigned char *	_data;
		int				_width;
		int				_height;
		int				_channels;
};
