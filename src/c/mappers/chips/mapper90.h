#ifndef __mapper90_h__
#define __mapper90_h__

#define MAPPER90A	0
#define MAPPER90B	1

void mapper90_line(int line,int pcycles);
void mapper90_init(int r);
void mapper90_state(int mode,u8 *data);

#endif
