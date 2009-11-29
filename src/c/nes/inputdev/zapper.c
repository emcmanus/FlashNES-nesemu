#include "zapper.h"
#include "system/system.h"
#include "nes/ppu/palette.h"

static u8 data;

static u8 read_zapper(u32 addr)
{
	return(data);
}

static void update_zapper()
{
	data = joyzap;
	if(video_getpixel(joyx,joyy) == video_getpalette(0x10))
		data |= 8;
}

inputdev_t dev_zapper = {read_zapper,0,0,update_zapper};
