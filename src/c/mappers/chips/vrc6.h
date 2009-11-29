#ifndef __vrc6_h__
#define __vrc6_h__

#define KONAMI_VRC6A		0
#define KONAMI_VRC6B		1

void vrc6_init(int revision);
void vrc6_line(int line,int pcycles);
void vrc6_state(int mode,u8 *data);

#endif
