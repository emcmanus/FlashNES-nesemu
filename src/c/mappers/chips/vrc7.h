#ifndef __vrc7_h__
#define __vrc7_h__

#define KONAMI_VRC7A		0
#define KONAMI_VRC7B		1

void vrc7_init(int revision);
void vrc7_line(int line,int pcycles);
void vrc7_state(int mode,u8 *data);

#endif
