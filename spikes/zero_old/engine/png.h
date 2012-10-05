#ifndef PNG_H_
#define PNG_H_

#include <stdlib.h>
#include "pnglite.h"
#include <string>

class Png
{
public:
	Png(std::string filename);
	~Png();
	void* GetData();
public:
	int fmt;
	unsigned int width;
	unsigned int height;
private:
	png_t pnginfo;
	void *data;
};

#endif
