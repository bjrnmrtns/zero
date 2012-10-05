#include "png.h"
#include <string>
#include "filenotfoundexception.h"
#include <GL/glew.h>

Png::Png(std::string filename)
{
	png_init(0, 0);
	if(png_open_file_read(&pnginfo, filename.c_str()) != 0)
	{
		throw FileNotFoundException(filename);
	}
	data = malloc(pnginfo.width * pnginfo.height * pnginfo.bpp);
	png_get_data(&pnginfo, (unsigned char*)data);
	width = pnginfo.width;
	height = pnginfo.height;
	switch (pnginfo.color_type)
	{
		case PNG_GREYSCALE:
			fmt = GL_LUMINANCE;
			break;
		case PNG_TRUECOLOR:
			fmt = GL_RGB;
			break;
		case PNG_GREYSCALE_ALPHA:
			fmt = GL_LUMINANCE_ALPHA;
			break;
		case PNG_TRUECOLOR_ALPHA:
			fmt = GL_RGBA;
			break;
		default:
			fmt = GL_RGBA;
			break;
	}
}

Png::~Png()
{
	free(data);
	png_close_file(&pnginfo);
}

void* Png::GetData()
{
	return data;
}
