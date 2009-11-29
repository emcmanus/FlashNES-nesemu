#include "exp_arkanoid.h"
#include "system/system.h"

//static u8 data;

static u8 read_ark(u32 addr)
{
	int offset = 98;

	return(offset);
}

static void update_ark()
{
}

inputdev_t exp_arkanoid = {read_ark,0,0,update_ark};
