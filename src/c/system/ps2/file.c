/***************************************************************************
 *   Copyright (C) 2006-2009 by Dead_Body   *
 *   jamesholodnak@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <tamtypes.h>
#include <sys/fcntl.h>
#include <fileXio_rpc.h>
#include "system/file.h"

#define FILE_MC		0x01
#define FILE_HDD		0x02
#define FILE_GZ		0x40
#define FILE_TEMP		0x80

#define CHUNK 0x8000

/*
the method used for reading gzipped files from the hard drive
is very bad.  currently, it decompresses the rom to a temporary
file in the root of the roms partition, then deletes it when
the file is closed.

*/

typedef struct file_s {
	gzFile gzfd;
	int fd;
	FILE *fp;
	u32 flags;
	long length;
	char name[512];
} file_t;

static file_t files[16];
static int numfiles = 0;

static void make_temp_fn(char *fn)
{
	int i,n;

	for(i=0;i<8;i++) {
		n = rand() % (26 * 2);
		if(n < 26)
			fn[i] = n + 'A';
		else
			fn[i] = n + 'a' - 26;
	}
	memcpy(&fn[i],".tmp\0",5);
}

static int decompress(char *infn,char *outfn)
{
/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */

    int ret;
    unsigned have;
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];
	int source,dest;
	int srclen,srcptr = 0;

	if((source = fileXioOpen(infn,O_RDONLY,0)) < 0) {
		printf("decompress: error opening '%s'\n",infn);
		return(1);
	}
	if((dest = fileXioOpen(outfn,O_CREAT | O_WRONLY,0)) < 0) {
		printf("decompress: error opening '%s'\n",outfn);
		return(1);
	}

	fileXioLseek(source,0,SEEK_END);
	srclen = fileXioLseek(source,0,SEEK_CUR);
	fileXioLseek(source,0,SEEK_SET);
    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK) {
			printf("decompress: inflateInit failed\n");
        return ret;
    }

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fileXioRead(source, in, CHUNK);
        srcptr += strm.avail_in;
		  printf("decompress: read %d bytes...\n",strm.avail_in);
		  printf("decompress: $%02X $%02X $%02X $%02X-$%02X $%02X $%02X $%02X\n",
in[0],
in[1],
in[2],
in[3],
in[4],
in[5],
in[6],
in[7]);
//        if (ferror(source)) {
        if (srcptr >= srclen) {
		  printf("decompress: reached end of input (%d >= %d)\n",srcptr,srclen);
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
//            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
					printf("decompress: inflate returned Z_NEED_DICT\n");
                ret = Z_DATA_ERROR;     /* and fall through */
                (void)inflateEnd(&strm);
					fileXioClose(source);
					fileXioClose(dest);
                return ret;
            case Z_DATA_ERROR:
					printf("decompress: inflate returned Z_DATA_ERROR\n");
                (void)inflateEnd(&strm);
					fileXioClose(source);
					fileXioClose(dest);
                return ret;
            case Z_MEM_ERROR:
					printf("decompress: inflate returned Z_MEM_ERROR\n");
                (void)inflateEnd(&strm);
					fileXioClose(source);
					fileXioClose(dest);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fileXioWrite(dest, out, have) != have) {// || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);

    fileXioClose(source);
    fileXioClose(dest);

    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

void file_init()
{
	memset(files,0,sizeof(file_t) * 16);
	numfiles = 0;
}

void file_kill()
{
}

int file_open(char *filename,char *oldmode)
{
	int hdd = 0,gzip = 1;
	int fd = 0;
	char ch,mode[8] = "\0\0\0\0\0\0\0\0",*p = mode;

	//detect if the file is located on the hard drive
	if(strncmp(filename,"pfs0:",5) == 0)
		hdd = 1;

	//detect if file is gzipped
	if(strncmp(filename + strlen(filename) - 3,".gz",3) == 0)
		gzip = 1;

	while((ch = *oldmode++)) {
		if(ch == 'u') {
			gzip = 0;
			continue;
		}
		if(ch == 'z') {
			gzip = 1;
			continue;
		}
		*p++ = ch;
	}
	printf("opening '%s' in mode '%s' (gzip=%d)\n",filename,mode,gzip);
	for(fd=0;fd<16;fd++) {
		if(files[fd].flags == 0) {
			if(hdd) {
				int flags;
				char *m;

				for(flags=0,m=mode;*m;m++) {
					switch(*m) {
						case 'r': flags = O_RDONLY; break;
						case 'w': flags = O_WRONLY; break;
						case 'a': flags = O_RDWR; break;
					}
				}
				//only support reading gzipped files
				if(gzip && (flags != O_RDONLY)) {
					printf("only reading gzipped files from hdd is supported\n");
					return(-1);
				}
				if(gzip && (flags == O_RDONLY)) {
					char fn[512];
					char tmp[512];

					sprintf(fn,"decompress");
					make_temp_fn(fn);
					sprintf(tmp,"pfs0:/%s",fn);
					printf("temp filename is %s\n",tmp);
					if(decompress(filename,tmp) != Z_OK) {
						printf("error decompressing rom '%s' to temp file '%s'\n",filename,tmp);
						return(-1);
					}
					if((files[fd].fd = fileXioOpen(tmp,flags,0)) >= 0) {
						files[fd].gzfd = 0;
						files[fd].flags = FILE_HDD;
						file_seek(fd,0,SEEK_END);
						files[fd].length = file_tell(fd);
						file_seek(fd,0,SEEK_SET);
						strcpy(files[fd].name,tmp);
						return(fd);
					}
					return(-1);
				}
				if((files[fd].fd = fileXioOpen(filename,flags,0)) >= 0) {
					files[fd].gzfd = 0;
					files[fd].flags = FILE_HDD;
					file_seek(fd,0,SEEK_END);
					files[fd].length = file_tell(fd);
					file_seek(fd,0,SEEK_SET);
					return(fd);
				}
				else {
					printf("error opening file '%s'\n",filename);
					return(-1);
				}
			}
			else {
				if(gzip && (files[fd].gzfd = gzopen(filename,mode))) {
					printf("opening file with gzopen'%s'\n",filename);
					files[fd].fd = 0;
					files[fd].fp = 0;
					files[fd].flags = FILE_MC;
					return(fd);
				}
				else if((files[fd].fp = fopen(filename,mode))) {
					printf("opening file with fopen'%s'\n",filename);
					files[fd].gzfd = 0;
					files[fd].fd = 0;
					files[fd].flags = FILE_MC;
					return(fd);
				}
				else {
					printf("error opening file '%s'\n",filename);
					return(-1);
				}
			}
		}
	}
	return(-1);
}

void file_close(int fd)
{
	if(files[fd].flags & FILE_HDD)
		fileXioClose(files[fd].fd);
	else if(files[fd].fp)
		fclose(files[fd].fp);
	else
		gzclose(files[fd].gzfd);
	if(files[fd].flags & FILE_TEMP) {
		printf("removing temp file '%s'\n",files[fd].name);
		fileXioRemove(files[fd].name);
	}
	memset(&files[fd],0,sizeof(file_t));
}

int file_gets(int fd,char *buf,int maxlen)
{
	u8 ch;

	*buf = 0;
	for(;;) {
		int read = file_read(fd,&ch,1);

		if(read != 1 || ch == '\n' || ch == 0) {
			*buf = 0;
			return(strlen(buf));
		}
		*buf++ = ch;
	}
	return(0);
}

int file_read(int fd,void *buf,int len)
{
	if(files[fd].flags & FILE_HDD)
		return(fileXioRead(files[fd].fd,buf,len));
	else if(files[fd].fp)
		return(fread(buf,1,len,files[fd].fp));
	return(gzread(files[fd].gzfd,buf,len));
}

int file_write(int fd,void *buf,int len)
{
	if(files[fd].flags & FILE_HDD)
		return(fileXioWrite(files[fd].fd,buf,len));
	else if(files[fd].fp)
		return(fwrite(buf,1,len,files[fd].fp));
	return(gzwrite(files[fd].gzfd,buf,len));
}

long file_seek(int fd,long pos,int whence)
{
	if(files[fd].flags & FILE_HDD)
		return(fileXioLseek(files[fd].fd,pos,whence));
	else if(files[fd].fp)
		return(fseek(files[fd].fp,pos,whence));
	return(gzseek(files[fd].gzfd,pos,whence));
}

long file_tell(int fd)
{
	return(file_seek(fd,0,SEEK_CUR));
}

int file_eof(int fd)
{
	if(files[fd].flags & FILE_HDD) {
		if(file_tell(fd) >= files[fd].length)
			return(0);
		return(1);
	}
	else if(files[fd].fp)
		return(feof(files[fd].fp));
	return(gzeof(files[fd].gzfd));
}
