#ifndef __inputdev_h__
#define __inputdev_h__

#include "defines.h"

typedef struct inputdev_s {
	//read port
	u8 (*read)();

	//write port
	void (*write)(u8);

	//strobe
	void (*strobe)();

	//update controller info
	void (*update)();
} inputdev_t;

#endif
