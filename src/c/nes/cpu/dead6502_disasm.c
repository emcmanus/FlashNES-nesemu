#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"
#define __DEAD6502COMPILED__
#include "dead6502.h"

static char opcodes[256][4] =
	{
 	"brk","ora","???","???","???","ora","asl","???", /*00-07*/
	"php","ora","asl","???","???","ora","asl","???", /*08-0f*/
	"bpl","ora","???","???","???","ora","asl","???", /*10-17*/
	"clc","ora","???","???","???","ora","asl","???", /*18-1f*/
	"jsr","and","???","???","bit","and","rol","???", /*20-27*/
	"plp","and","rol","???","bit","and","rol","???", /*28-2f*/
	"bmi","and","???","???","???","and","rol","???", /*20-27*/
	"sec","and","???","???","???","and","rol","???", /*38-3f*/
	"rti","eor","???","???","???","eor","lsr","???", /*30-37*/
	"pha","eor","lsr","???","jmp","eor","lsr","???", /*48-4f*/
	"bvc","eor","???","???","???","eor","lsr","???", /*40-47*/
	"cli","eor","???","???","???","eor","lsr","???", /*58-5f*/
	"rts","adc","???","???","???","adc","ror","???", /*50-57*/
	"pla","adc","ror","???","jmp","adc","ror","???", /*68-6f*/
	"bvs","adc","???","???","???","adc","ror","???", /*60-67*/
	"sei","adc","???","???","???","adc","ror","???", /*78-7f*/
	"???","sta","???","???","sty","sta","stx","???", /*80-87*/
	"dey","???","txa","???","sty","sta","stx","???", /*88-8f*/
	"bcc","sta","???","???","sty","sta","stx","???", /*90-97*/
	"tya","sta","txs","???","???","sta","???","???", /*98-9f*/
	"ldy","lda","ldx","???","ldy","lda","ldx","???", /*a0-a7*/
	"tay","lda","tax","???","ldy","lda","ldx","???", /*a8-af*/
	"bcs","lda","???","???","ldy","lda","ldx","???", /*b0-b7*/
	"clv","lda","tsx","???","ldy","lda","ldx","???", /*b8-bf*/
	"cpy","cmp","???","???","cpy","cmp","dec","???", /*c0-c7*/
	"iny","cmp","dex","???","cpy","cmp","dec","???", /*c8-cf*/
	"bne","cmp","???","???","???","cmp","dec","???", /*d0-d7*/
	"cld","cmp","???","???","???","cmp","dec","???", /*d8-df*/
	"cpx","sbc","???","???","cpx","sbc","inc","???", /*e0-e7*/
	"inx","sbc","nop","???","cpx","sbc","inc","???", /*e8-ef*/
	"beq","sbc","???","???","???","sbc","inc","???", /*f0-f7*/
	"sed","sbc","???","???","???","sbc","inc","???"  /*f8-ff*/
	};
enum addrmodes {er=0,no,ab,ax,ay,ac,im,ix,iy,in,re,zp,zx,zy};
static u8 addrtable[256] =
	{
   no,ix,er,er,er,zp,zp,er, /*00-07*/
	no,im,ac,er,er,ab,ab,er, /*08-0f*/
   re,iy,er,er,er,zx,zx,er, /*10-17*/
	no,ay,er,er,er,ax,ax,er, /*18-1f*/
   ab,ix,er,er,zp,zp,zp,er, /*20-27*/
	no,im,ac,er,ab,ab,ab,er, /*28-2f*/
   re,iy,er,er,er,zx,zx,er, /*30-37*/
	no,ay,er,er,er,ax,ax,er, /*38-3f*/
   no,ix,er,er,er,zp,zp,er, /*40-47*/
	no,im,ac,er,ab,ab,ab,er, /*48-4f*/
   re,iy,er,er,er,zx,zx,er, /*50-57*/
	no,ay,er,er,er,ax,ax,er, /*58-5f*/
   no,ix,er,er,er,zp,zp,er, /*60-67*/
	no,im,ac,er,in,ab,ab,er, /*68-6f*/
   re,iy,er,er,er,zx,zx,er, /*70-77*/
	no,ay,er,er,er,ax,ax,er, /*78-7f*/
 	er,ix,er,er,zp,zp,zp,er, /*80-87*/
	no,er,no,er,ab,ab,ab,er, /*88-8f*/
   re,iy,er,er,zx,zx,zy,er, /*90-97*/
	no,ay,no,er,er,ax,er,er, /*98-9f*/
   im,ix,im,er,zp,zp,zp,er, /*a0-a7*/
	no,im,no,er,ab,ab,ab,er, /*a8-af*/
   re,iy,er,er,zx,zx,zy,er, /*b0-b7*/
	no,ay,no,er,ax,ax,ay,er, /*b8-bf*/
   im,ix,er,er,zp,zp,zp,er, /*c0-c7*/
	no,im,no,er,ab,ab,ab,er, /*c8-cf*/
   re,iy,er,er,er,zx,zx,er, /*d0-d7*/
	no,ay,er,er,er,ax,ax,er, /*d8-df*/
   im,ix,er,er,zp,zp,zp,er, /*e0-e7*/
	no,im,no,er,ab,ab,ab,er, /*e8-ef*/
   re,iy,er,er,zx,zx,zx,er, /*f0-f7*/
	no,ay,er,er,ax,ax,ax,er  /*f8-ff*/
   };
static u8 oplength[256] =
	{
	1,2,0,0,0,2,2,0, /*00-07*/
	1,2,1,0,0,3,3,0, /*08-0f*/
	2,2,0,0,0,2,2,0, /*10-17*/
	1,3,0,0,0,3,3,0, /*18-1f*/
	3,2,0,0,2,2,2,0, /*20-27*/
	1,2,1,0,3,3,3,0, /*28-2f*/
	2,2,0,0,0,2,2,0, /*30-37*/
	1,3,0,0,0,3,3,0, /*38-3f*/
	1,2,0,0,0,2,2,0, /*40-47*/
	1,2,1,0,3,3,3,0, /*48-4f*/
	2,2,0,0,0,2,2,0, /*50-57*/
	1,3,0,0,0,3,3,0, /*58-5f*/
	1,2,0,0,1,2,2,0, /*60-67*/
	1,2,1,0,3,3,3,0, /*68-6f*/
	2,2,0,0,1,2,2,0, /*70-77*/
	1,3,0,0,1,3,3,0, /*78-7f*/
	0,2,0,0,2,2,2,0, /*80-87*/
	1,0,1,0,3,3,3,0, /*88-8f*/
	2,2,0,0,2,2,2,0, /*90-97*/
	1,3,1,0,0,3,0,0, /*98-9f*/
	2,2,2,0,2,2,2,0, /*a0-a7*/
	1,2,1,0,3,3,3,0, /*a8-af*/
	2,2,0,0,2,2,2,0, /*b0-b7*/
	1,3,1,0,3,3,3,0, /*b8-bf*/
	2,2,0,0,2,2,2,0, /*c0-c7*/
	1,2,1,0,3,3,3,0, /*c8-cf*/
	2,2,0,0,0,2,2,0, /*d0-d7*/
	1,3,0,0,0,3,3,0, /*d8-df*/
	2,2,0,0,2,2,2,0, /*e0-e7*/
	1,2,1,0,3,3,3,0, /*e8-ef*/
	2,2,0,0,2,2,2,0, /*f0-f7*/
	1,3,0,0,3,3,3,0  /*f8-ff*/
	};

static char *mystrupr(char *buf)
{
char *ret = buf;

while(*buf != 0)
	{
	*buf = toupper(*buf);
	buf++;
	}
return(ret);
}

u16 dead6502_disassembleopcode(char *buffer,u16 opcodepos)
{
u8 opcode,size;
u16 addr;

strcpy(buffer,"");
opcode = dead6502_read(opcodepos);
switch(addrtable[opcode])
	{
	case er:size = 1;sprintf(buffer,".u8 $%02x",opcode);break;
	case no:size = 1;sprintf(buffer,"%s",opcodes[opcode]);break;
	case ac:size = 1;sprintf(buffer,"%s a",opcodes[opcode]);break;
	case ab:
		size = 3;
		addr = dead6502_read(opcodepos+1) | (dead6502_read(opcodepos+2) << 8);
		if(addr < 0x100) /*should be zero page, so its bad opcode*/
			{
			size = 1;
			sprintf(buffer,".u8 $%02x",opcode);
			break;
			}
		sprintf(buffer,"%s $%04x",opcodes[opcode],addr);
		break;
	case ax:
		size = 3;
		addr = dead6502_read(opcodepos+1) | (dead6502_read(opcodepos+2) << 8);
		if(addr < 0x100) /*should be zero page, so its bad opcode*/
			{
			size = 1;
			sprintf(buffer,".u8 $%02x",opcode);
			break;
			}
		sprintf(buffer,"%s $%04x,x",opcodes[opcode],addr);
		break;
	case ay:
		size = 3;
		addr = dead6502_read(opcodepos+1) | (dead6502_read(opcodepos+2) << 8);
		if(addr < 0x100) /*should be zero page, so its bad opcode*/
			{
			size = 1;
			sprintf(buffer,".u8 $%02x",opcode);
			break;
			}
		sprintf(buffer,"%s $%04x,y",opcodes[opcode],addr);
		break;
	case in:size = 3;sprintf(buffer,"%s ($%04X)",opcodes[opcode],dead6502_read(opcodepos+1) | (dead6502_read(opcodepos+2) << 8));break;
	case im:size = 2;sprintf(buffer,"%s #$%02x",opcodes[opcode],dead6502_read(opcodepos+1));break;
	case ix:size = 2;sprintf(buffer,"%s ($%02x,x)",opcodes[opcode],dead6502_read(opcodepos+1));break;
	case iy:size = 2;sprintf(buffer,"%s ($%02x),y",opcodes[opcode],dead6502_read(opcodepos+1));break;
	case re:size = 2;sprintf(buffer,"%s $%04x",opcodes[opcode],opcodepos+size+((s8)dead6502_read(opcodepos+1)));break;
	case zp:size = 2;sprintf(buffer,"%s $%02x",opcodes[opcode],dead6502_read(opcodepos+1));break;
	case zx:size = 2;sprintf(buffer,"%s $%02x,x",opcodes[opcode],dead6502_read(opcodepos+1));break;
	case zy:size = 2;sprintf(buffer,"%s $%02x,y",opcodes[opcode],dead6502_read(opcodepos+1));break;
	default:size = 1;sprintf(buffer,"disassembler bug");break;
	}
mystrupr(buffer); /*upper case is easier to read*/
return(opcodepos + size);
}

void dead6502_disasm_init()
{
int i;

for(i=0;i<256;i++)
	{
	switch(addrtable[i])
		{
		case er:
		case no:
		case ac:oplength[i] = 1;break;
		case im:
		case ix:
		case iy:
		case re:
		case zp:
		case zx:
		case zy:oplength[i] = 2;break;
		case ab:
		case ax:
		case ay:
		case in:oplength[i] = 3;break;
		}
	}
}
