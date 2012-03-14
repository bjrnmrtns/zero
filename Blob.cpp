#ifndef BLOB_
#define BLOB_
#include <memory>
struct Blob
{
	size_t size;
	std::shared_ptr<unsigned char> buf;
};
#endif

