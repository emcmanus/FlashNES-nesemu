#ifndef __vrc2_h__
#define __vrc2_h__

#define KONAMI_VRC2A		0
#define KONAMI_VRC2B		1

void vrc2_init(int r);
void vrc2_line(int line,int pcycles);
void vrc2_state(int mode,u8 *data);

#endif
