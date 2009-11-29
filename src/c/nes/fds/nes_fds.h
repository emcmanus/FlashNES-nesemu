#ifndef __fds_h__
#define __fds_h__

#include "mappers/mapper.h"

extern u8 disknum;

u8 fds_read(u32 addr);
void fds_write(u32 addr,u8 data);
void fds_line(int line,int pcycles);
void fds_reset(int hard);
void fds_state(int mode,u8 *data);

extern mapper_ines_t mapper_fds;

#endif
