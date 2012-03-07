#include <cstdio>
#include <string>
#include <iostream>
#include <memory>
#include <cassert>

class File
{
public:
	struct Lump
	{
		size_t size;
		std::unique_ptr<unsigned char> buf;
	};
	FILE* file;
	File(const std::string name, const char* mode)
	: file(fopen(name.c_str(), mode))
	{
		if(file == 0) throw std::exception();
	}
	~File()
	{
		fclose(file);
	}
	static std::unique_ptr<Lump> read(const std::string name)
	{
		File file(name, "r");
		fseek(file.file, 0L, SEEK_END);
		std::unique_ptr<Lump> lump(new Lump);
		lump->size = ftell(file.file);
		rewind(file.file);
		unsigned char* buf = (unsigned char*)malloc(lump->size);
		lump->buf.reset(buf);
		size_t done = 0;
		do {
			size_t read = fread(buf+done, 1, lump->size - done, file.file);
			assert(read != 0);
			done += read;
		} while(done != lump->size);
		return lump;
	}
	static void write(const std::string name, const Lump& lump)
	{
		File file(name, "w");
		size_t done = 0;
		do {
			size_t written = fwrite(lump.buf.get()+done, 1, lump.size - done, file.file);
			assert(written != 0);
			done += written;
		} while(done != lump.size);
	}
};

int main()
{
	std::unique_ptr<File::Lump> lump = File::read("RenderPipeline");
	File::write("Copyfile", *(lump.get()));
	return 0;
}
