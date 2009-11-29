#ifndef __vrc4_h__
#define __vrc4_h__

#define KONAMI_VRC4A		0
#define KONAMI_VRC4B		1
#define KONAMI_VRC4C		2
#define KONAMI_VRC4D		3
#define KONAMI_VRC4E		4

void vrc4_init(int r);
void vrc4_line(int line,int pcycles);
void vrc4_state(int mode,u8 *data);

#endif
