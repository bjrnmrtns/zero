#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <exception>
#include <string>
#include <string.h>
#include <cassert>
#include <iostream>


class Binary
{
public:
	Binary(const std::string filename)
	{
		if((fd = open(filename.c_str(), O_RDONLY, 0644 )) == -1) perror("open");
		struct stat file_info;
		if(fstat(fd, &file_info) == -1) perror("fstat");
		size_ = file_info.st_size;

		void *rawdata = mmap(NULL, file_info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
		if (rawdata == MAP_FAILED) perror("mmap");

		data = (uint8_t*)rawdata;
	}
	~Binary()
	{
		munmap(rawdata, size_);
		close(fd);
	}
	uint8_t operator[](unsigned int index)
	{
		assert(index < size_);
		return data[index];
	}
	size_t size() { return size_; }
private:
	void *rawdata;
	uint8_t *data;
	size_t size_;
	int fd;
};

class z80
{
public:
	z80(Binary& binary)
	: binary(binary)
	{
		unsigned int pc = 0;
	}
private:
	Binary& binary;
};

int main()
{
	Binary binary("test.bin");
	for(size_t i=0; i < binary.size(); i++)
	{
		std::cout << binary[i];
	}
	z80 cpu(binary);
}

