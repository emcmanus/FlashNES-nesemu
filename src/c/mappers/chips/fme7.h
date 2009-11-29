#ifndef __fme7_h__
#define __fme7_h__

void fme7_sync();
void fme7_line(int line,int pcycles);
void fme7_init(int hard);
void fme7_state(int mode,u8 *data);

#endif
