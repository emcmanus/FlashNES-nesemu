#include "powerpad.h"
#include "system/system.h"

static u16 paddata;
static int padread;

static u8 read_powerpad(u32 addr)
{
	u8 ret = 0;

	ret |= ((paddata >> (padread + 0)) & 1) << 3;
	ret |= ((paddata >> (padread + 8)) & 1) << 4;
	padread++;
	return(ret);
}

static void strobe_powerpad()
{
	//last four bits always 1
	paddata = 0xF000;

	//top four buttons
	if(joykeys[config.pad_keys[1]])  paddata |= 0x001;
	if(joykeys[config.pad_keys[0]])  paddata |= 0x002;
	if(joykeys[config.pad_keys[4]])  paddata |= 0x004;
	if(joykeys[config.pad_keys[8]])  paddata |= 0x008;

	//middle four buttons
	if(joykeys[config.pad_keys[5]])  paddata |= 0x010;
	if(joykeys[config.pad_keys[9]])  paddata |= 0x020;
	if(joykeys[config.pad_keys[10]]) paddata |= 0x040;
	if(joykeys[config.pad_keys[6]])  paddata |= 0x080;

	//bottom four buttons
	if(joykeys[config.pad_keys[3]])  paddata |= 0x100;
	if(joykeys[config.pad_keys[2]])  paddata |= 0x200;
	if(joykeys[config.pad_keys[11]]) paddata |= 0x400;
	if(joykeys[config.pad_keys[7]])  paddata |= 0x800;
}

inputdev_t dev_powerpad = {read_powerpad,0,strobe_powerpad,0};
