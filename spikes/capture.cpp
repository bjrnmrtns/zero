#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <exception>
#define VPX_CODEC_DISABLE_COMPAT 1
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"
#define interface (vpx_codec_vp8_cx())
#define fourcc    0x30385056

#define IVF_FILE_HDR_SZ  (32)
#define IVF_FRAME_HDR_SZ (12)
#include <iostream>
namespace capture {
struct ivfheader
{
	char signature[4];
	unsigned short version;
	unsigned short lengthheader;
	unsigned int codec;
	unsigned short width;
	unsigned short height;
	unsigned int framerate;
	unsigned int timescale;
	unsigned int nrframes;
	unsigned int unused;
};

class pic
{
private:
	const unsigned char* buf;
	size_t width;
	size_t height;
public:
	pic(const unsigned char *buf, size_t width, size_t height)
	: buf(buf)
	, width(width)
	, height(height)
	{
	}
	const unsigned char* pix(int x, int y)
	{
		return buf + (y * width * 3) + (x * 3);
	}
	unsigned char* pixw(int x, int y)
	{
		return const_cast<unsigned char *>(pix(x, y));
	}
};


class encoder
{
private:
	vpx_image_t image;
	FILE* file;
	vpx_codec_enc_cfg_t cfg;
	vpx_codec_ctx_t codec;
	size_t framecount;
public:
	encoder(size_t width, size_t height, std::string filename)
	: framecount(0)
	{
		if(width < 16 || width % 2 != 0 || height < 16 || height % 2 != 0)
		{
			throw new std::exception();
		}
		if(!vpx_img_alloc(&image, VPX_IMG_FMT_I420, width, height, 1))
		{
			throw new std::exception();
		}
		if(!(file = fopen(filename.c_str(), "wb")))
		{
			throw new std::exception();
		}
                if(vpx_codec_enc_config_default(vpx_codec_vp8_cx(), &cfg, 0))
		{
			throw new std::exception();
		}
		cfg.rc_target_bitrate = width * height * cfg.rc_target_bitrate / cfg.g_w / cfg.g_h;
		cfg.g_w = width;
		cfg.g_h = height;
        	if(vpx_codec_enc_init(&codec, vpx_codec_vp8_cx(), &cfg, 0))
		{
			throw new std::exception();
		}
		writefileheader();
	}
	void convertrgbtoI420(const unsigned char* data, vpx_image_t *image)
	{
		unsigned char yuv[image->w * image->h * 3];
		pic source(data, image->w, image->h);
		pic dest(yuv, image->w, image->h);
		for(size_t y = 0; y < image->h; y++)
		{
			for(size_t x = 0; x < image->w; x++)
			{
				unsigned char r = *(source.pix(x,y));
				unsigned char g = *(source.pix(x,y) + 1);
				unsigned char b = *(source.pix(x,y) + 2);
				*(dest.pixw(x,y)) = 0.299 * r + 0.587 * g + 0.114 * b;
				*(dest.pixw(x,y)+1) = (-0.169 * r - 0.331 * g + 0.4998 * b) + 128;            
				*(dest.pixw(x,y)+2) = (0.4998 * r - 0.419 * g - 0.081 * b) + 128; 
			}
		}
		size_t i = 0;
		for(size_t y = 0; y < image->h; y++)
		{
			for(size_t x = 0; x < image->w; x++)
			{
				*(image->planes[0] + i++) = *(dest.pix(x,y));
			}
		}
		for(size_t y = 0; y < image->h; y+=2)
		{
			for(size_t x = 0; x < image->w; x+=2)
			{
				*(image->planes[0] + i++) = (*(dest.pix(x,y) + 1)
				                          + *(dest.pixw(x+1,y) + 1)
				                          + *(dest.pixw(x,y+1) + 1)
				                          + *(dest.pixw(x+1,y+1) + 1)) / 4;
			}
		}
		for(size_t y = 0; y < image->h; y+=2)
		{
			for(size_t x = 0; x < image->w; x+=2)
			{
				*(image->planes[0] + i++) = (*(dest.pix(x,y) + 2)
				                          + *(dest.pixw(x+1,y) + 2)
				                          + *(dest.pixw(x,y+1) + 2)
				                          + *(dest.pixw(x+1,y+1) + 2)) / 4;
			}
		}
	}
	void encode(const unsigned char* data)
	{
		vpx_codec_iter_t iter = 0;
		
		convertrgbtoI420(data, &image);
		if(VPX_CODEC_OK != vpx_codec_encode(&codec, &image, framecount++, 1, 0, VPX_DL_REALTIME))
		{
			throw new std::exception();
		}
		const vpx_codec_cx_pkt_t *pkt;
		while((pkt = vpx_codec_get_cx_data(&codec, &iter)))
		{
			switch(pkt->kind)
			{
			case VPX_CODEC_CX_FRAME_PKT:
				writeframeheader(pkt);
				fwrite(pkt->data.frame.buf, 1, pkt->data.frame.sz, file);
			break;
			default:
			break;
			}
		}

	}
	void writeframeheader(const vpx_codec_cx_pkt_t *pkt)
	{
		// only write the 4 least significant bytes of sz (sz is 8 bytes in size)
		fwrite(&(pkt->data.frame.sz), 1, 4, file);
		fwrite(&(pkt->data.frame.pts), 1, sizeof(pkt->data.frame.pts), file);
	}
	void writefileheader()
	{
		if(fseek(file, 0, SEEK_SET) != 0)
		{
			throw new std::exception();
		}
		capture::ivfheader h;
		h.signature[0] = 'D';
		h.signature[1] = 'K';
		h.signature[2] = 'I';
		h.signature[3] = 'F';
		h.version = 0;
		h.lengthheader = 32;
		h.codec = 0x30385056;
		h.width = cfg.g_w;
		h.height = cfg.g_h;
		h.framerate = cfg.g_timebase.den;
		h.timescale = cfg.g_timebase.num;
		h.nrframes = framecount;
		h.unused = 0;
		fwrite(&h, 1, sizeof(capture::ivfheader), file);
	}
	~encoder()
	{
		vpx_codec_destroy(&codec);
		writefileheader();
		fclose(file);
	}

};
}

int main()
{
	capture::encoder enc(1024, 768, "bla.ivf");
	unsigned char data[1024 * 768 * 3];
	for (size_t i = 0; i < 256; i++) {
		for (size_t j = 0; j < 768; j++) {
			for (size_t k = 0; k < 1024; k++) {
				data[(j*1024+k)*3+0] = j/3;
				data[(j*1024+k)*3+1] = k/4;
				data[(j*1024+k)*3+2] = i;
			}
		}
		enc.encode(data);
	}
	return 0;
}
