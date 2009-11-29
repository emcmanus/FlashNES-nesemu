#ifndef __sachen_8259_h__
#define __sachen_8259_h__

#define SACHEN_8259A		0
#define SACHEN_8259B		1
#define SACHEN_8259C		2
#define SACHEN_8259D		3

void sachen_8259_init(int r);
void sachen_8259_state(int mode,u8 *data);

#endif
