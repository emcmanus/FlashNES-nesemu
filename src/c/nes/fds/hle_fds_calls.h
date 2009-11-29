#ifndef __hle_fds_calls_h__
#define __hle_fds_calls_h__

#include "defines.h"

typedef struct fds_disk_header_s {
	u8 blockid;
	u8 ident[14];
	u8 manufacturer_code;
	u8 name[4];
	u8 version;
	u8 diskside;
	u8 disknum;
	u8 extra[2];
	u8 bootid;
	u8 data[31];
	u8 numfiles;
} fds_disk_header_t;

typedef struct fds_file_header_s {
	u8 unk;
	u8 blockid;
	u8 index;
//start of file header on disk
	u8 fileid;
	u8 name[8];
	u16 loadaddr;
	u16 filesize;
	u8 loadarea;
//end of file header on disk
} fds_file_header_t;

typedef struct fds_file_header2_s {
	u8 fileid;
	u8 name[8];
	u16 loadaddr;
	u16 filesize;
	u8 loadarea;
	u16 srcaddr;
	u8 srcarea;
} fds_file_header2_t;

typedef void (*hle_call_t)();

extern hle_call_t hle_calls[];

#endif
