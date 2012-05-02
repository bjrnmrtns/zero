#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define VPX_CODEC_DISABLE_COMPAT 1
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"
#define interface (vpx_codec_vp8_cx())
#define fourcc    0x30385056

#define IVF_FILE_HDR_SZ  (32)
#define IVF_FRAME_HDR_SZ (12)

namespace capture {
struct ivfheader
{
	char signature[4];
	unsigned short version;
	unsigned short lenghtheader;
	unsigned int codec;
	unsigned short width;
	unsigned short height;
	unsigned int framerate;
	unsigned int timescale;
	unsigned int nrframes;
	unsigned int unused;
};
}


