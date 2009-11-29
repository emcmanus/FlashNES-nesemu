#ifndef __mmc6_h__
#define __mmc6_h__

#include "defines.h"

u8 mmc6_getprgbank(int n);
u8 mmc6_getchrbank(int n);
void mmc6_syncprg();
void mmc6_syncchr();
void mmc6_syncvram();
void mmc6_syncsram();
void mmc6_syncmirror();
void mmc6_init(void (*s)());
void mmc6_write(u32 addr,u8 data);
void mmc6_line(int line,int pcycles);
void mmc6_load(u8 *data);
void mmc6_save(u8 *data);

#endif
