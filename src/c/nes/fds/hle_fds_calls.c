#include <string.h> //for memcpy
#include "nes/fds/hle_fds_calls.h"
#include "nes/fds/nes_fds.h"
#include "nes/nes.h"
#include "nes/ppu/ppu.h"

//returns error code (to be put in accumulator)
static void hle_loadfiles()
{
	fds_disk_header_t disk_header;
	fds_file_header_t file_header;
	u8 loadedfiles = 0;
	u32 tmp,retaddr,addr1,addr2;
	u8 diskid;
	int pos,i,j,k;
	u8 fileidlist[0x20];

	tmp = 0x100 + nes->cpu.s;
	retaddr = dead6502_read(tmp + 1) << 0;
	retaddr |= dead6502_read(tmp + 2) << 8;

	addr1 = dead6502_read(retaddr + 1);
	addr1 |= dead6502_read(retaddr + 2) << 8;

	addr2 = dead6502_read(retaddr + 3);
	addr2 |= dead6502_read(retaddr + 4) << 8;

	log_message("loadfiles: addr1,2 = $%04X, $%04X\n",addr1,addr2);

	//get diskid
	diskid = dead6502_read(addr1);

	//load file id list
	for(i=0;i<20;i++) {
		fileidlist[i] = dead6502_read(addr2 + i);
		log_message("loadfiles: id list[%d] = $%02X\n",i,fileidlist[i]);
		if(fileidlist[i] == 0xFF)
			break;
	}

	log_message("loadfiles: disk id required = %d, disknum = %d\n",diskid,disknum);
	//read in disk header
	pos = disknum * 65500;
	memcpy(&disk_header,nes->rom->diskdata + pos,sizeof(fds_disk_header_t));
//	log_message("loading all files with bootid less than %d\n",disk_header.bootid);
	pos += 57;

	//$FF indicates to load system boot files
	if(fileidlist[0] == 0xFF) {

		//leet hax
		log_message("loadfiles: hack!  fileidlist[0]==$FF!  resetting disknum to 0!!!@!@@121112\n");
		disknum = 0;

		//load boot files
		for(i=0;i<disk_header.numfiles;i++) {
			char fn[9] = "        \0";

			memcpy(&file_header,nes->rom->diskdata + pos,sizeof(fds_file_header_t));
			pos += 17;
			if(file_header.fileid <= disk_header.bootid) {
				memcpy(fn,file_header.name,8);
				log_message("loadfiles: loading boot file '%s', id %d, size $%X, load addr $%04X\n",fn,file_header.fileid,file_header.filesize,file_header.loadaddr);
				loadedfiles++;

				//load into cpu
				if(file_header.loadarea == 0) {
					for(j=0;j<file_header.filesize;j++)
						dead6502_write(j + file_header.loadaddr,nes->rom->diskdata[pos + j + 1]);
				}

				//load into ppu
				else {
					for(j=0;j<file_header.filesize;j++)
						ppumem_write(j + file_header.loadaddr,nes->rom->diskdata[pos + j + 1]);
				}
			}
			pos += file_header.filesize;
		}
	}
	else {
		//load files
		for(i=0;i<disk_header.numfiles;i++) {
			char fn[9] = "        \0";

			memcpy(&file_header,nes->rom->diskdata + pos,sizeof(fds_file_header_t));
			pos += 17;                 
			memcpy(fn,file_header.name,8);
			log_message("loadfiles: checking file '%s', id $%X, size $%X, load addr $%04X\n",fn,file_header.fileid,file_header.filesize,file_header.loadaddr);
			for(k=0;fileidlist[k] != 0xFF && k < 20;k++) {
				if(file_header.fileid == fileidlist[k]) {
					log_message("loadfiles: loading file '%s', id $%X, size $%X, load addr $%04X\n",fn,file_header.fileid,file_header.filesize,file_header.loadaddr);
					loadedfiles++;       

					//load into cpu
					if(file_header.loadarea == 0) {
						for(j=0;j<file_header.filesize;j++)
							dead6502_write(j + file_header.loadaddr,nes->rom->diskdata[pos + j + 1]);
					}

					//load into ppu
					else {
						for(j=0;j<file_header.filesize;j++)
							ppumem_write(j + file_header.loadaddr,nes->rom->diskdata[pos + j + 1]);
					}
				}
			}
			pos += file_header.filesize;
		}
	}

	retaddr += 4;
	dead6502_write(tmp + 1,retaddr & 0xFF);
	dead6502_write(tmp + 2,(retaddr >> 8) & 0xFF);

	log_message("loadfiles: loaded %d files\n",loadedfiles);

	//put loaded files into y register
	nes->cpu.y = loadedfiles;

	//put error code in accumulator
	nes->cpu.a = 0;
}

static void hle_writefile()
{
	int i;
	char fn[9] = "        \0";
	u32 tmp,retaddr,addr1,addr2;
	u32 pos;
	fds_file_header_t file_header;
	fds_file_header2_t file_header2;
	fds_disk_header_t disk_header;

	if(disknum > 3) {
		log_warning("hle_writefile: disknum > 3, correcting with mask of $%X\n",3);
		disknum &= 3;
	}
	tmp = 0x100 + nes->cpu.s;
	retaddr = dead6502_read(tmp + 1) << 0;
	retaddr |= dead6502_read(tmp + 2) << 8;
	log_message("writefile: retaddr = $%04X\n",retaddr);

	addr1 = dead6502_read(retaddr + 1);
	addr1 |= dead6502_read(retaddr + 2) << 8;
	log_message("writefile: diskid addr = $%04X\n",addr1);

	addr2 = dead6502_read(retaddr + 3);
	addr2 |= dead6502_read(retaddr + 4) << 8;
	log_message("writefile: fileheader list addr = $%04X\n",addr2);

	retaddr += 4;
	dead6502_write(tmp + 1,retaddr & 0xFF);
	dead6502_write(tmp + 2,(retaddr >> 8) & 0xFF);

	pos = disknum * 65500;

	memcpy(&disk_header,nes->rom->diskdata + pos,sizeof(fds_disk_header_t));

	pos += 57;

	if(nes->cpu.y != 0xFF)
		disk_header.numfiles = nes->cpu.y;

	dead6502_write(6,disk_header.numfiles);

	//skip thru files to find the position to write the file
	for(i=0;i<disk_header.numfiles;i++) {
		memcpy(&file_header,nes->rom->diskdata + pos,sizeof(fds_file_header_t));
		pos += 17 + file_header.filesize;
	}

	log_message("hle writefile: disk data pos = %d\n",pos);

	//write the file
	//get the file write info
	pos += 3;

	for(i=0;i<17;i++)
		((u8*)&file_header2)[i] = dead6502_read(addr2 + i);
	//write header
	memcpy(nes->rom->diskdata + pos,&file_header2,sizeof(fds_file_header2_t));
	pos += 17;
	//write data
	for(i=0;i<file_header2.filesize;i++) {
		if(file_header2.srcarea == 0)
			nes->rom->diskdata[pos++ - 2] = dead6502_read(file_header2.srcaddr+i);
		else
			nes->rom->diskdata[pos++ - 2] = ppumem_read(file_header2.srcaddr+i);
	}
	disk_header.numfiles++;

	//write new disk header
	pos = disknum * 65500;
	memcpy(nes->rom->diskdata + pos,&disk_header,57);

	dead6502_write(0x6,disk_header.numfiles);

	pos += 57;
		for(i=0;i<disk_header.numfiles;i++) {
			char fn[9] = "        \0";

			memcpy(&file_header,nes->rom->diskdata + pos,sizeof(fds_file_header_t));
			pos += 17;                 
			memcpy(fn,file_header.name,8);
			log_message("hle_writefile: checking file '%s', id $%X, size $%X, load addr $%04X\n",fn,file_header.fileid,file_header.filesize,file_header.loadaddr);
			pos += file_header.filesize;
		}


	dead6502_write(0x05,0xFF);
	nes->cpu.a = 0;
	nes->cpu.x = 0;
	nes->cpu.f &= ~0x80;
	nes->cpu.f |= 0x02;
}

static void hle_vramfill()
{
	//read in parameters
	u8 vramaddrhi = nes->cpu.a;
	u8 fillvalue = nes->cpu.x;
	int i,j,loopcount = nes->cpu.y;

	log_message("hle vramfill: a = %X, x = %X, y = %X\n",nes->cpu.a,nes->cpu.x,nes->cpu.y);

	//store variables
	dead6502_write(0x00,nes->cpu.a);
	dead6502_write(0x01,nes->cpu.x);
	dead6502_write(0x02,nes->cpu.y);

	//reset ppu toggle
	ppu_read(0x2002);

	//write new ppu ctrl 1 reg save
	dead6502_write(0xFF,dead6502_read(0xFF) & 0xFB);

	//write to ppu ctrl 1 reg
	ppu_write(0x2000,dead6502_read(0xFF));

	//write vramaddr to ppu registers
	ppu_write(0x2006,vramaddrhi);
	ppu_write(0x2006,0x00);

	//adjust loop count for nametables
	if(vramaddrhi >= 0x20)
		loopcount = 4;

	//fill ppu data
	for(i=loopcount;i;i--) {
		for(j=0;j<256;j++)
			ppu_write(0x2007,fillvalue);
	}

	//reset ppu toggle
	ppu_read(0x2002);

	//write to ppu ctrl 1 reg
	ppu_write(0x2000,dead6502_read(0xFF));

	//attribute byte fill
	if(vramaddrhi >= 0x20) {
		ppu_write(0x2006,vramaddrhi+3);
		ppu_write(0x2006,0xC0);
		for(i=0;i<0x40;i++)
			ppu_write(0x2007,fillvalue);
	}

	nes->cpu.x = 1;
}

static void hle_memfill()
{
	u8 fillvalue = nes->cpu.a;
	u8 firstpage = nes->cpu.x;
	u8 lastpage = nes->cpu.y;
	int i;

	log_message("hle memfill: x = %X, y = %X\n",nes->cpu.x,nes->cpu.y);
	for(i=firstpage*0x100;i<((lastpage+1)*0x100);i++)
		dead6502_write(i,fillvalue);
}

static void hle_ppudataproc()
{
/*
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;PPU data 
processorлллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
;this routine treats a string of bytes as custom instructions. These strings
;are stored in a mannar similar	to regular CPU instructions, in	that the
;instructions are stored & processed sequentially, and instruction length is
;dynamic. The instructions haved been designed to allow easy filling/copying
;of data to random places in the PPU memory map. Full random access to PPU
;memory is supported, and it is	even possible to call subroutines. All data
;written to PPU	memory is stored in the actual instructions (up	to 64
;sequential bytes of data can be stored in 1 instruction).

;the 16-bit immediate which follows the 'JSR PPUdataPrsr' opcode is a 
pointer
;to the first PPU data string to be processed.

;the PPU data processor's opcodes are layed out as follows.


;+---------------+
;|special opcodes|
;+---------------+
; $4C:	call subroutine. 16-bit call address follows. This opcode is
;	the equivelant of a 'JMP $xxxx'	6502 mnemonic.

; $60:	end subroutine.	returns to the instruction after the call.
;	This opcode is the equivelant of a 'RTS' 6502 mnemonic.

; $80..$FF:	end program. processing will not stop until this opcode is
;	encountered.


;+---------------------------------+
;|move/fill data instruction format|
;+---------------------------------+

; byte 0
; ------
;  high byte of	destination PPU	address. cannot	be equal to $60, $4C or
;  greater than	$7F.

; byte 1
; ------
;  low byte of destination PPU address.

; byte 2 bit description
; ----------------------
;  7:	 increment PPU address by 1/32 (0/1)
;  6:	 copy/fill data	to PPU mem (0/1)
;  5-0:	 byte xfer count (0 indicates 64 bytes)

; bytes 3..n
; ----------
;  - if the fill bit is set, there is only one byte here, which	is the value
;  to fill the PPU memory with,	for the specified byte xfer count.
;  - if copy mode is set instead, this data contains the bytes to be copied
;  to PPU memory. The number of	bytes appearing	here is equal to the byte
;  xfer count.
*/
	u32 tmp,tmp1;
	u32 i,len,addr,ppuaddr;
	u8 data,ctrl;

	//get data pointer addr
	tmp = 0x100 + nes->cpu.s;
	tmp1 = dead6502_read(tmp + 1) << 0;
	tmp1 |= dead6502_read(tmp + 2) << 8;

//	log_message("hle ppudataprsr: ptr dataaddr = $%04X\n",tmp1);

	//get data addr
	addr = dead6502_read(tmp1 + 1) << 0;
	addr |= dead6502_read(tmp1 + 2) << 8;

//	log_message("hle ppudataprsr: dataaddr = $%04X\n",addr);

	//fixup return address
	tmp1 += 2;
	dead6502_write(tmp + 1,tmp1 & 0xFF);
	dead6502_write(tmp + 2,(tmp1 >> 8) & 0xFF);

	for(;;) {
		ppu_read(0x2002);
		data = dead6502_read(addr++);
		//end of data
		if(data >= 0x80) {
			log_message("hle ppudataprsr: end opcode @ $%04X = $%02X\n",addr-1,data);		
			break;
		}
		//jump to subroutine
		else if(data == 0x4C) {
			log_message("ppu data jsr\n");
			tmp1 = addr;
			addr = dead6502_read(tmp1 + 0);
			addr |= dead6502_read(tmp1 + 1) << 8;
			tmp1 += 2;
			dead6502_push(tmp1 >> 8);
			dead6502_push(tmp1);
			log_message("jsr: old addr, new addr = $%04X, $%04X\n",tmp1-2,addr);
		}
		//return from subroutine
		else if(data == 0x60) {
			log_message("ppu data rts\n");
			addr = dead6502_pop();
			addr |= dead6502_pop() << 8;
			dead6502_write(0x00,addr);
			dead6502_write(0x01,addr >> 8);
		}
		//opcode stream
		else {
			log_message("hle ppudataprsr: opcode @ $%04X = $%02X\n",addr-1,data);		
			ppuaddr = (data << 8) | dead6502_read(addr++);
			data = dead6502_read(addr++);
			log_message("hle ppudataprsr: %s data (addr=$%04X,data=$%02X)\n",(data & 0x40) ? "filling" : "copying",ppuaddr,data);
			len = (data & 0x3F) == 0 ? 64 : (data & 0x3F);
			ctrl = dead6502_read(0xFF) | 4;
			if((data & 0x80) == 0)
				ctrl &= 0xFB;
			ppu_write(0x2000,ctrl);
			dead6502_write(0xFF,ctrl);
			ppu_write(0x2006,ppuaddr >> 8);
			ppu_write(0x2006,ppuaddr >> 0);
			for(i=0;i<len;i++) {
				ppu_write(0x2007,dead6502_read(addr));
				if((data & 0x40) == 0)	//copy or fill?
					addr++;					//if copy, increment address pointer
			}
			if((data & 0x40) != 0)	//fill
				addr++;					//increment address pointer
		}
	}
}

static void hle_ppurestore()
{
	ppu_read(0x2002);
	ppu_write(0x2005,dead6502_read(0xFD));
	ppu_write(0x2005,dead6502_read(0xFC));
	ppu_write(0x2000,dead6502_read(0xFF));
}

static void hle_rand()
{
	nes->cpu.a = rand() & 0xFF;
}

static void hle_readctrlrs()
{
	int i;
	u8 tmp1[2],tmp2[2],data;

	dead6502_write(0x4016,0xFC);
	dead6502_write(0x4016,0xFB);
	tmp1[0] = tmp1[1] = tmp2[0] = tmp2[1] = 0;
	for(i=7;i>=0;i--) {
		data = dead6502_read(0x4016);
		tmp1[0] |= (data & 1) << i;
		tmp1[1] |= ((data >> 1) & 1) << i;
		data = dead6502_read(0x4017);
		tmp2[1] |= (data & 1) << i;
		tmp2[1] |= ((data >> 1) & 1) << i;
	}
	dead6502_write(0xF5,tmp1[0]);
	dead6502_write(0xF6,tmp2[0]);
	dead6502_write(0x00,tmp1[1]);
	dead6502_write(0x01,tmp2[1]);
}

static void hle_cputoppucpy()
{
	u8 units = nes->cpu.x;
	u16 ppuaddr = (nes->cpu.y << 8) | (nes->cpu.a & 0xF0);
	u8 mode = nes->cpu.a;
	u8 fill;

	u32 i,tmp,tmp1,addr;

	//get data pointer addr
	tmp = 0x100 + nes->cpu.s;
	tmp1 = dead6502_read(tmp + 1) << 0;
	tmp1 |= dead6502_read(tmp + 2) << 8;

	log_message("hle cputoppucpy: tmp1 = $%04X\n",tmp1);
	//get data addr
	addr = dead6502_read(tmp1 + 1) << 0;
	addr |= dead6502_read(tmp1 + 2) << 8;

//	addr --; 
	//fixup return address
	tmp1 += 2;
	dead6502_write(tmp + 1,tmp1 & 0xFF);
	dead6502_write(tmp + 2,(tmp1 >> 8) & 0xFF);
	log_message("hle cputoppucpy: cpuaddr = $%04X, units = %d\n",addr,units);

	//read from ppu
	if(mode & 2) {
		log_message("hle cputoppucpy: read from ppu not supported\n");
	}

/*	if(mode & 0xC)
		addr &= ~0xF;

	//write to ppu
	else */{
		while(units--) {
			log_message("hle cputoppucpy: ppu,cpu addr = $%04X,$%04X, mode = %X\n",ppuaddr,addr,mode & 0xC);
			fill = (mode & 1) ? 0xFF : 0x00;
			switch(mode & 0xC) {
				case 0x0:
					for(i=0;i<16;i++)	ppumem_write(ppuaddr++,dead6502_read(addr++));
					break;
				case 0x4:
					for(i=0;i<8;i++)	ppumem_write(ppuaddr++,dead6502_read(addr++));
					for(i=0;i<8;i++)	ppumem_write(ppuaddr++,fill);
					break;
				case 0x8:
					for(i=0;i<8;i++)	ppumem_write(ppuaddr++,fill);
					for(i=0;i<8;i++)	ppumem_write(ppuaddr++,dead6502_read(addr++));
					break;
				case 0xC:
					for(i=0;i<8;i++,ppuaddr++) {
						u8 data = dead6502_read(addr++);

						ppumem_write(ppuaddr,data ^ fill);
						ppumem_write(ppuaddr+8,data);
					}
					ppuaddr += 8;
					break;
			}
		}
	}
}

static void hle_error()
{
	log_message("hle_error!\n");
}

static void hle_getdiskinfo()
{
	fds_disk_header_t disk_header;
	fds_file_header_t file_header;
	u32 tmp,tmp1;
	u32 i,j,addr,pos;

	//get data pointer addr
	tmp = 0x100 + nes->cpu.s;
	tmp1 = dead6502_read(tmp + 1) << 0;
	tmp1 |= dead6502_read(tmp + 2) << 8;

	log_message("hle_getdiskinfo: ptr dataaddr = $%04X\n",tmp1);

	//get data addr
	addr = dead6502_read(tmp1 + 1) << 0;
	addr |= dead6502_read(tmp1 + 2) << 8;

	log_message("hle_getdiskinfo: dataaddr = $%04X\n",addr);

	//fixup return address
	tmp1 += 2;
	dead6502_write(tmp + 1,tmp1 & 0xFF);
	dead6502_write(tmp + 2,(tmp1 >> 8) & 0xFF);

	pos = disknum * 65500;
	memcpy(&disk_header,nes->rom->diskdata + pos,sizeof(fds_disk_header_t));
	pos += 57;

	dead6502_write(addr++,disk_header.manufacturer_code);
	dead6502_write(addr++,disk_header.name[0]);
	dead6502_write(addr++,disk_header.name[1]);
	dead6502_write(addr++,disk_header.name[2]);
	dead6502_write(addr++,disk_header.name[3]);
	dead6502_write(addr++,disk_header.version);
	dead6502_write(addr++,disk_header.diskside);
	dead6502_write(addr++,disk_header.disknum == 0 ? 1 : 0);
	dead6502_write(addr++,disk_header.disknum == 1 ? 1 : 0);
	dead6502_write(addr++,disk_header.disknum == 2 ? 1 : 0);
	dead6502_write(addr++,disk_header.numfiles);

	//skip thru files to find the position to write the file
	for(i=0;i<disk_header.numfiles;i++) {
		memcpy(&file_header,nes->rom->diskdata + pos,sizeof(fds_file_header_t));
		pos += 17 + file_header.filesize;
		dead6502_write(addr++,file_header.fileid);
		for(j=0;j<8;j++)
			dead6502_write(addr++,file_header.name[j]);
	}

	dead6502_write(addr++,pos >> 8);
	dead6502_write(addr++,pos >> 0);

	nes->cpu.a = 0;
}

static void hle_xferdone()
{
/*
;dispatched when transfer is to	be terminated. returns error # in A.
XferDone:	LDX #$00;	no error
$E77A	BEQ $E786
---
XferFailOnCy:	BCS XferFail
$E77E	RTS
XferFailOnNEQ:	BEQ $E77E
XferFail:	TXA
$E782	LDX $04
$E784	TXS;	restore PC to original caller's address
$E785	TAX
---
$E786	LDA $FA
$E788	AND #$09
$E78A	ORA #$26
$E78C	STA $FA
$E78E	STA $4025
$E791	TXA
$E792	CLI
$E793	RTS
*/
	u8 data = dead6502_read(0xFA);

	data &= 9;
	data |= 0x26;
	dead6502_write(0xFA,data);
	dead6502_write(0x4025,data);
	nes->cpu.a = 0;
	nes->cpu.x = 0;
}

static void hle_intro()
{
	int i;

	//intro state
	static u8 state = 0;

	switch(state) {
		//init state
		case 0:
			log_message("intro inited\n");
			dead6502_write(0x2000,0x00);
			dead6502_write(0x2006,0x3F);
			dead6502_write(0x2006,0x00);
			dead6502_write(0x2007,0x1A);
			for(i=1;i<16;i++)
				dead6502_write(0x2007,0x0F);
			state = 1;
			break;

		//startup state
		case 1:
			log_message("intro starting up\n");
			//skip to load files state
			state = 3;
			break;

		//main intro loop
		case 2:
			break;

		//begin loading files from disk
		case 3:
			dead6502_push(1);
			return;
	}
	dead6502_push(0);
}

//reimplemented in the 6502 part of the hle bios
//xfre byte
/*void hle_xferbyte()
{
	u8 tmp = dead6502_read(0x4031);

	dead6502_write(0x4024,nes->cpu.a);
	nes->cpu.a = tmp;
}*/

hle_call_t hle_calls[] = {
	//$0
	hle_loadfiles,
	hle_writefile,
	0,//hle_adjfilecount,
	0,//hle_checkfilecount,
	hle_getdiskinfo,
	0,//hle_setfilecount1,
	0,//hle_setfilecount2,
	0,//hle_xferbyte,

	//$8
	0,//hle_checkblktype,
	0,//hle_writeblktype,
	0,//hle_endofblkread,
	0,//hle_endofblkwrite,
	hle_ppurestore,
	hle_ppudataproc,
	hle_memfill,
	hle_vramfill,

	//$10
	hle_rand,
	hle_cputoppucpy,
	hle_readctrlrs,
	hle_error,
	hle_xferdone,
	0,
	0,
	0,

	//$18
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	hle_intro,
};
