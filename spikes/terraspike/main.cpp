#include <cstdio>
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include "GeneralException.cpp"

class File
{
public:
	FILE* file;
	File(const std::string name, const char* mode)
	: file(fopen(name.c_str(), mode))
	{
		if(file == 0) throw GeneralException("could not open file");
	}
	template<typename T>
	int read(T* data)
	{
		return ::fread(data, sizeof(T), 1, file);
	}
	template<typename T>
	int read(T* data, size_t length)
	{
		return ::fread(data, sizeof(T), length, file);
	}
	~File()
	{
		fclose(file);
	}
};

namespace terra
{
struct rect
{
	int32_t left, right, top, bottom;
};

class worldheader
{
public:
	void read(File& file)
	{
		file.read(&mapversion);
		int8_t worldnamelength;
		file.read(&worldnamelength);
		char worldnamedata[128];
		int length = file.read(worldnamedata, worldnamelength);
		worldname = std::string(worldnamedata, length);
		file.read(&worldid);
		file.read(&worldbounds);
		file.read(&worldheightintiles);
		file.read(&worldwidthintiles);
		file.read(&spawnx);
		file.read(&spawny);
		file.read(&worldsurfaceY);
		file.read(&rocklayerY);
		file.read(&gametime);
	}
	void log()
	{
		std::cout << "mapversion: " << mapversion << std::endl;
		std::cout << "worldname: " << worldname << std::endl;
	}
private:
	int32_t mapversion;
	std::string worldname;
	int32_t worldid;
	rect worldbounds;
	int32_t worldheightintiles;
	int32_t worldwidthintiles;
	int32_t spawnx;
	int32_t spawny;
	double worldsurfaceY;
	double rocklayerY;
	double gametime;
	bool day;
	int32_t moonphase;
	bool bloodmoon;
};

}
int main()
{
	File worldfile("world1.wld", "r");
	terra::worldheader header;
	header.read(worldfile);
	header.log();
	return 0;
}
