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

//#include <stdio.h> //for log_message
#include <string.h>
#include "nes/nes.h"
#include "mappers/mapper.h"
#include "system/system.h"

#define BIOS_IDLE		0x00
#define BIOS_INIT		0x01
#define BIOS_PLAY		0x02

extern u8 nsfbios_rom[];

static u8 prg[8],song;
static u8 disabled,bios_cmd;
static readfunc_t readF;
static writefunc_t write4;
static u8 *readptrF = 0;
static u8 read_bios(u32 addr);
static u8 read_vectors(u32 addr);

static void sync()
{
	int i;

	if(memcmp(nes->rom->nsf.bankswitch,"\0\0\0\0\0\0\0\0",8) == 0)
		return;

	//sync nsf prg banks
	for(i=8;i<0x10;i++) {
		mem_setsram4(i,prg[i & 7] + 2);
		mem_setwriteptr(i,0);
	}

	readptrF = mem_getreadptr(0xF);
	mem_setreadptr(0xF,0);
	mem_setread(0xF,read_vectors);
}

static u8 read_bios(u32 addr)
{
	u8 ret;
	int i;

	//bios registers
	if(addr < 0x3100) {
/*		if(addr <= 0x3004 && addr >= 0x3000)
			log_message("bios reading regs at $%04X\n",addr);
		else
			log_message("bios reading ??? at $%04X\n",addr);
		log_message("loadaddr = $%04X\n",nes->rom->nsf.loadaddr);
		log_message("initaddr = $%04X\n",nes->rom->nsf.initaddr);
		log_message("playaddr = $%04X\n",nes->rom->nsf.playaddr);
*/		switch(addr & 0xFF) {
			case 0: return(nes->rom->nsf.initaddr & 0xFF);
			case 1: return(nes->rom->nsf.initaddr >> 8);
			case 2: return(nes->rom->nsf.playaddr & 0xFF);
			case 3: return(nes->rom->nsf.playaddr >> 8);
			case 4: ret = bios_cmd; bios_cmd = 0; return(ret);
			case 5: return(0);
			case 0x10: return(song);
			case 0x80:	//bios init
				printf("bios init\n");
				memset(nes->ram,0,0x800);
				memset(nes->rom->sram,0,0x2000);
				for(i=0x4000;i<=0x4013;i++)
					dead6502_write(i,(i == 0x4010) ? 0x10 : 0x00);
				dead6502_write(0x4015,0xF);
				return(0);
		}
		log_message("bios reading regs at $%04X\n",addr);
		return(0);
	}
	return(nsfbios_rom[addr & 0xFFF]);
}

static void write_bios(u32 addr,u8 data)
{
	if(addr < 0x3100) {
		switch(addr & 0xFF) {
			case 4:
				bios_cmd = data;
				log_message("writing bios cmd = $%02X\n",data);
				break;
			case 0x10:
				song = data;
				log_message("writing song reg = $%02X\n",song);
				break;
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
				log_message("writing reg $%02X = $%02X\n",addr & 0xFF,song);
				break;
		}
	}
}

static u8 read_vectors(u32 addr)
{
	if(addr >= 0xFFFA) {
//		log_message("reading vector at $%04X\n",addr);
		return(nsfbios_rom[addr & 0xFFF]);
	}
	return(readptrF[addr & 0xFFF]);
}

static void write45(u32 addr,u8 data)
{
	if(addr < 0x4020) {
		write4(addr,data);
		return;
	}
	switch(addr) {
		case 0x5FF0:
			if(disabled == 0) {
				switch(data) {

				}
			}
		case 0x5FF8: prg[0] = data; sync(); break;
		case 0x5FF9: prg[1] = data; sync(); break;
		case 0x5FFA: prg[2] = data; sync(); break;
		case 0x5FFB: prg[3] = data; sync(); break;
		case 0x5FFC: prg[4] = data; sync(); break;
		case 0x5FFD: prg[5] = data; sync(); break;
		case 0x5FFE: prg[6] = data; sync(); break;
		case 0x5FFF: prg[7] = data; sync(); break;
	}
}

static void changesong(int s)
{
	nes->cpu.a = s;
	nes->cpu.x = 0;
	nes->cpu.pc = nes->rom->nsf.initaddr;
	dead6502_read(0x3080);
	dead6502_push(0x30);
	dead6502_push(0xFF);
}

static int currentsong = 0;
static int playing = 0;

void nsf_line(int line,int pcycles)
{
	static int oldinput;
	int input,i;

	//call play address
	if(line == 21 && playing) {
		nes->cpu.pc = nes->rom->nsf.playaddr;
		dead6502_push(0x30);
		dead6502_push(0xFF);
	}
	input = 0;
	if(joykeys[config.joy_keys[0][0]]) input |= INPUT_A;
	if(joykeys[config.joy_keys[0][1]]) input |= INPUT_B;
	if(joykeys[config.joy_keys[0][2]]) input |= INPUT_SELECT;
	if(joykeys[config.joy_keys[0][3]]) input |= INPUT_START;
	if(joykeys[config.joy_keys[0][4]]) input |= INPUT_UP;
	if(joykeys[config.joy_keys[0][5]]) input |= INPUT_DOWN;
	if(joykeys[config.joy_keys[0][6]]) input |= INPUT_LEFT;
	if(joykeys[config.joy_keys[0][7]]) input |= INPUT_RIGHT;
	i = input & ~oldinput;
	oldinput = input;
	if(i & INPUT_A) {
		playing = 1;
		changesong(currentsong);
	}
	if(i & INPUT_B)
		playing = 0;
	if(i & INPUT_RIGHT && currentsong < nes->rom->nsf.totalsongs)
		currentsong++;
	if(i & INPUT_LEFT && currentsong > 0)
		currentsong--;
}

void nsf_init(int hard)
{
	int i,j;

	for(i=0;i<8;i++)
		prg[i] = 0;
	disabled = 0;
	write4 = mem_getwrite(4);
	readF = mem_getread(0xF);
	mem_setwrite(4,write45);
	mem_setwrite(5,write45);
	mem_setread(0x3,read_bios);
	mem_setread(0xF,read_vectors);
	currentsong = 0;
	playing = 0;
	disabled = 0;
	bios_cmd = 1;
	if(memcmp(nes->rom->nsf.bankswitch,"\0\0\0\0\0\0\0\0",8) != 0) { //bankswitched
		log_message("setting up nsf bankswitch: startaddr = $%04X\n",nes->rom->nsf.loadaddr);
		for(i=0;i<8;i++)
			prg[i] = nes->rom->nsf.bankswitch[i];
		j = nes->rom->prgsize / 4096 / 1024;
		nes_setsramsize(2 + 8 + j + 1);
		memset(nes->rom->sram,0,0x8000 + 0x2000);
		memcpy(nes->rom->sram + 0x2000 + (nes->rom->nsf.loadaddr & 0xFFF),nes->rom->prg,nes->rom->prgsize);
	}
	else {
		nes_setsramsize(2 + 8 + 1);
		mem_setsram8(0x8,1);
		mem_setsram8(0xA,2);
		mem_setsram8(0xC,3);
		mem_setsram8(0xE,4);
		memset(nes->rom->sram,0,0x8000 + 0x2000);
		memcpy(nes->rom->sram + 0x2000 + (nes->rom->nsf.loadaddr - 0x8000),nes->rom->prg,nes->rom->prgsize);
		readptrF = mem_getreadptr(0xF);
		mem_setreadptr(0xF,0);
		mem_setread(0xF,read_vectors);
		printf("nsf not bankswitched\n");
	}
	mem_setsram8(6,0);
	//setup bios
	mem_setread(3,read_bios);
	mem_setwrite(3,write_bios);
//	mem_setreadptr(0xF,0);
//	mem_setwriteptr(0xF,0);
	sync();
}

MAPPER_INES(NSF_MAPPER,nsf_init,0,nsf_line,0);
