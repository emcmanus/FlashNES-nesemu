#include "exp_barcode.h"
#include "system/system.h"

//static u8 data;

static u8 read_bar(u32 addr)
{
	return(0);
}

static void update_bar()
{
}

inputdev_t exp_barcode = {read_bar,0,0,update_bar};
