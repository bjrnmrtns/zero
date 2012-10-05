#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "shaderprogram.h"

class Texture
{
public:
	Texture(unsigned int width, unsigned int height, bool debug = false);
	Texture(std::string filename, bool debug = false);
	Texture(unsigned int size, int nrOfBlocks, bool debug = false);
	void Construct(unsigned int width, unsigned int height, void *data);
	static Texture& BlockTexture();
	void Bind(ShaderProgram& prog, const std::string& name);
	void SetAsAttachment(unsigned int count);
	~Texture();
	int CreateTextureUnitId();
private:
	unsigned int id;
	int textureUnitId;
public:
	bool debug;
};

#endif
