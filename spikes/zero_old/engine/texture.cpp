#include "texture.h"
#include <GL/glew.h>
#include <string>
#include "png.h"

Texture::Texture(unsigned int width, unsigned int height, bool debug)
: debug(debug)
{
	Construct(width, height, (void*)0);
}

Texture::Texture(std::string filename, bool debug)
: debug(debug)
{
	Png png(filename);
	Construct(png.width, png.height, png.GetData());
}

Texture::Texture(unsigned int size, int nrOfBlocks, bool debug)
: debug(debug)
{
	struct
	{
		unsigned char R, G, B;
	} data[size][size];

	unsigned int freq = size / nrOfBlocks;
	for(unsigned int x = 0; x < size; x++)
	{
		for(unsigned int y = 0; y < size; y++)
		{
			if( ((x / freq) + (y / freq)) % 2)
			{
				data[x][y].R = 255;
				data[x][y].G = 255;
				data[x][y].B = 255;
			}
			else
			{
				data[x][y].R = 0;
				data[x][y].G = 0;
				data[x][y].B = 0;
			}
		}
	}
	Construct(size, size, (void*)data);
}
void Texture::Construct(unsigned int width, unsigned int height, void *data)
{
	textureUnitId = CreateTextureUnitId();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

}

Texture& Texture::BlockTexture()
{
	static Texture blocktexture(512, (int)8);
	return blocktexture;
}

void Texture::Bind(ShaderProgram& prog, const std::string& name)
{
	int error;
	error = glGetError(); if (error) { printf("%d\n", error); abort(); }
	if (prog.SetTexture(name.c_str(), textureUnitId))
	{
	error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		glActiveTexture(GL_TEXTURE0+textureUnitId);
	error = glGetError(); if (error) { printf("%d\n", error); abort(); }
		glBindTexture(GL_TEXTURE_2D, id);
	error = glGetError(); if (error) { printf("%d\n", error); abort(); }
	}
}

void Texture::SetAsAttachment(unsigned int count)
{
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + count, id, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

int Texture::CreateTextureUnitId()
{
	static int curtexUnit = -1;
	// TODO: max textureunits not necesseraly 16.
	return (++curtexUnit % 16);
}
