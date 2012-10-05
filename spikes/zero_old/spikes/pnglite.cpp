#include <cstddef>
#include <pnglite.h>
#include <string>

int main()
{
	std::string filename("../data/crap.png");
	int err = png_init(0, 0);
	png_t pnginfo;
	err = png_open_file_read(&pnginfo, filename.c_str());
	if(err != 0)
		printf("open png failed");
	png_print_info(&pnginfo);
	printf("bpp %d\n", pnginfo.bpp);
	unsigned char *data = (unsigned char *)malloc(pnginfo.width * pnginfo.height * pnginfo.bpp);

	if (err != 0)
		printf("png_get_data error = %d\n", err);

	free(data);
	err = png_close_file(&pnginfo);

	return 0;
}
