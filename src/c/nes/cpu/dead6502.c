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

/* copyright 2000 Dead_Body */
/* version 1.04 - its complete now!
   thanks to: [_TRAC_] + mdw2 + GreyBrain + TNSe + nyef + Azimer + Delta
   compiling: there are some defines you can do:
     NES - dont include adc/sbc decimal mode stuff
     JMPBUG - make the indirect jmp have the bug, just like the real nes 6502
     SKIPBADOPS - skip over bad opcodes instead of returning an error
     CPUDEBUG - debug the cpu core (or debug a rom...)                                          */
/************************************************************************************************/
#define NES
#define JMPBUG
/*#define SKIPBADOPS*/
//#define CPUDEBUG
/*#define MSGFUNC*/
//#include "marijuanes.h"
/************************************************************************************************/
//#define INC_OPCYCLES //opcycles++;
#define INC_OPCYCLES

#ifdef _WINDOWS /*prolly msvc (i hope so :P)*/
#pragma warning(disable:4244) /*nasty warning, conversion from bleh to blah, possible loss of data*/
#endif
#include <ctype.h>
#if defined(WIN32)
	#include <windows.h>
	#define SPRINTF wsprintf
#else
	#include <stdio.h>
	#include <string.h>
	#define SPRINTF sprintf
#endif
//#ifdef CPUDEBUG
	#ifdef MSGFUNC
		extern void msg(char *,...);
	#else
		#include <stdio.h>
		#define msg	log_message
	#endif
//#endif
#include "defines.h"
#define __DEAD6502COMPILED__
#include "dead6502.h"

#define checknz(b)\
	d6502_f &= 0x7D;\
	d6502_f |= nztable[b];
#define push(b)\
	{\
	a = (u32)(d6502_s-- | 0x100);\
	d6502_write6502(a,(u8)(b));\
	}
#define pop(b)\
	{\
	a = (u32)(++d6502_s | 0x100);\
	(b) = d6502_read6502(a);\
	}
#define INLINE						__inline
#define EXTRA_CYCLE				1
#define am_xxx						0
#define op_xxx						0
#define op_xxx_xxx				0
#define NMI_MASK					0xfe
#define IRQ_MASK					0xfd
#define NZ							0x82
#define NZ_MASK					0x7d
#define FLAG_NEGATIVE			0x80
#define FLAG_OVERFLOW			0x40
#define FLAG_XXX					0x20
#define FLAG_BRK					0x10
#define FLAG_DECIMAL				0x08
#define FLAG_IRQDISABLE			0x04
#define FLAG_ZERO					0x02
#define FLAG_CARRY				0x01
#define FLAG_NEGATIVE_MASK		0x7f
#define FLAG_OVERFLOW_MASK		0xbf
#define FLAG_XXX_MASK			0xdf
#define FLAG_BRK_MASK			0xef
#define FLAG_DECIMAL_MASK		0xf7
#define FLAG_IRQDISABLE_MASK	0xfb
#define FLAG_ZERO_MASK			0xfd
#define FLAG_CARRY_MASK			0xfe
#define OFFSET_A					0
#define OFFSET_X					(OFFSET_A + 4)
#define OFFSET_Y					(OFFSET_X + 4)
#define OFFSET_S					(OFFSET_Y + 4)
#define OFFSET_F					(OFFSET_S + 4)
#define OFFSET_PC					(OFFSET_F + 4)
#define OFFSET_TOTALCYCLES		(OFFSET_PC + 4)
#define OFFSET_NEEDIRQ			(OFFSET_TOTALCYCLES + 4)
#define OFFSET_OP6502			(OFFSET_NEEDIRQ + 4)
#define OFFSET_READ6502			(OFFSET_OP6502 + 4)
#define OFFSET_WRITE6502		(OFFSET_READ6502 + 4)
#define OFFSET_OPPAGES			(OFFSET_WRITE6502 + 4)
#define OFFSET_READPAGES		(OFFSET_OPPAGES + (PAGE_NUM * 4))
#define OFFSET_WRITEPAGES		(OFFSET_READPAGES + (PAGE_NUM * 4))
/************************************************************************************************/
typedef void(*voidcall)();
/************************************************************************************************/
static u32 a;
static dead6502data *data;
#define d6502_a data->a
#define d6502_x data->x
#define d6502_y data->y
#define d6502_s data->s
#define d6502_f data->f
#define d6502_pc data->pc
#define d6502_eaddr data->eaddr
#define d6502_totalcycles data->totalcycles
#define d6502_needirq data->needirq
#ifdef EXECFRAME
#define d6502_framecycles data->framecycles
#define d6502_scanlinecycles data->scanlinecycles
#define d6502_line data->line
static u32 framecycles;
static u32 scanlinecycles;
#endif
static u8 nmicycles;
static u8 irqcycles;
static u32 cyclesexec;
static u16 addr,addrtmp;
static u8 value;
enum addrmodes {er=0,no,ab,ax,ay,ac,im,ix,iy,in,re,zp,zx,zy};
static u8 cycletable[256];
static u8 nztable[256];
static u8 basecycletable[256] =
	{
	7,6,0,0,0,3,5,0,3,2,2,0,0,4,6,0, /* 00-0f */
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0, /* 10-1f */
	6,6,0,0,3,3,5,0,4,2,2,0,4,4,6,0, /* 20-2f */
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0, /* 30-3f */
	6,6,0,0,0,3,5,0,3,2,2,0,3,4,6,0, /* 40-4f */
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0, /* 50-5f */
	6,6,0,0,0,3,5,0,4,2,2,0,5,4,6,0, /* 60-6f */
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0, /* 70-7f */
	0,6,0,0,3,3,3,0,2,0,2,0,4,4,4,0, /* 80-8f */
	2,6,0,0,4,4,4,0,2,5,2,0,0,5,0,0, /* 90-9f */
	2,6,2,0,3,3,3,0,2,2,2,0,4,4,4,0, /* a0-af */
	2,5,0,0,4,4,4,0,2,4,2,0,4,4,4,0, /* b0-bf */
	2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0, /* c0-cf */
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0, /* d0-df */
	2,6,0,0,3,3,5,0,2,2,2,0,4,4,6,0, /* e0-ef */
	2,5,0,0,0,4,6,0,2,4,0,0,0,4,7,0  /* f0-ff */
	};

/*#define branch(cond)\
{\
value = d6502_read6502(d6502_pc);\
d6502_pc++;\
addrtmp = d6502_pc + (s8)value;\
if((addrtmp ^ d6502_pc) & 0xff00)\
	cyclesexec++;\
if(cond)\
	{\
	d6502_pc = addrtmp;\
	cyclesexec++;\
	}\
}*/

#define branch(cond)\
{\
if(cond)\
	{\
	value = d6502_read6502(d6502_pc);\
	d6502_pc++;\
	addrtmp = d6502_pc + (s8)value;\
	if((addrtmp ^ d6502_pc) & 0xff00)\
		cyclesexec++;\
	d6502_pc = addrtmp;\
	cyclesexec++;\
	}\
else\
	d6502_pc++;\
}

INLINE u8 d6502_op6502(u32 address)
{
int p = address >> 12;
u8 *page = data->oppages[address >> DEAD6502_PAGE_SHIFT];

if(address < 0x2000)
	return(page[address & 0x7FF]);
if(page)
	return(page[address & DEAD6502_PAGE_MASK]);
if(data->op6502[p])
	return(data->op6502[p](address));
#ifdef _DEBUG
log_message("dead6502: unhandled op read at $%04X\n",addr);
#endif
return(0);
}

INLINE u8 d6502_read6502(u32 address)
{
int p = address >> 12;
u8 *page = data->readpages[address >> DEAD6502_PAGE_SHIFT];

if(address < 0x2000)
	return(page[address & 0x7FF]);
if(page)
	return(page[address & DEAD6502_PAGE_MASK]);
if(data->read6502[p])
	return(data->read6502[p](address));
#ifdef _DEBUG
log_message("dead6502: unhandled read at $%04X\n",address);
#endif
return(0);
}

u8 dead6502_read(u32 addr){return(d6502_read6502(addr));}

INLINE void d6502_write6502(u32 address,u8 value)
{
int p = address >> 12;
u8 *page = data->writepages[address >> DEAD6502_PAGE_SHIFT];

//log_message("write at $%04X\n",address);
if(address < 0x2000)
	page[address & 0x7FF] = value;
else if(page)
	page[address & DEAD6502_PAGE_MASK] = value;
else if(data->write6502[p])
	data->write6502[p](address,value);
#ifdef _DEBUG
//else
//	log_message("dead6502: unhandled write at $%04X\n",address);
#endif
}

void dead6502_write(u32 addr,u8 data){d6502_write6502(addr,data);}

static void op_brk_imp()
{
//log_message("brk at $%04X\n",d6502_pc-1);
//for(;;);
d6502_pc++;
d6502_f |= FLAG_BRK;
push(d6502_pc >> 8);
push(d6502_pc);
push(d6502_f);
d6502_f |= FLAG_IRQDISABLE;
d6502_pc = d6502_read6502(0xfffe);
d6502_pc |= d6502_read6502(0xffff) << 8;
}

void dead6502_push(u8 d){push(d);}
u8 dead6502_pop(){u8 ret;pop(ret);return(ret);}

static void op_ora_inx()
{
u8 zpaddr;

//opcycles++;
zpaddr = d6502_read6502(d6502_pc++);

//opcycles++;
addr = d6502_read6502(zpaddr);
zpaddr += d6502_x;

//opcycles++;
addr = d6502_read6502(zpaddr++);

//opcycles++;
addr |= d6502_read6502(zpaddr) << 8;

//opcycles++;
d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ora_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_asl_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
d6502_f &= FLAG_CARRY_MASK;
if(value & 0x80)
	d6502_f |= FLAG_CARRY;
value = value << 1;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_php_imp()
{
////opcycles++;
push(d6502_f | FLAG_BRK);
}
/************************************************************************************************/
static void op_ora_imm()
{
addr = d6502_pc++;

//opcycles++;
d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_asl_acc()
{
//opcycles++;
d6502_f &= FLAG_CARRY_MASK;
if(d6502_a & 0x80)
	d6502_f |= FLAG_CARRY;
d6502_a = d6502_a << 1;
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ora_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_asl_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
d6502_f &= FLAG_CARRY_MASK;
if(value & 0x80)
	d6502_f |= FLAG_CARRY;
value = value << 1;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_bpl_rel()
{
branch((d6502_f & FLAG_NEGATIVE) == 0);
}
/************************************************************************************************/
static void op_ora_iny()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ora_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_asl_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(value & 0x80)
	d6502_f |= FLAG_CARRY;
value = value << 1;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_clc_imp()
{
d6502_f &= FLAG_CARRY_MASK;
}
/************************************************************************************************/
static void op_ora_aby()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ora_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a |= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_asl_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(value & 0x80)
	d6502_f |= FLAG_CARRY;
value = value << 1;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_jsr_abs()
{
addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc) << 8;
push(d6502_pc >> 8);
push(d6502_pc);
d6502_pc = addr;
}
/************************************************************************************************/
static void op_and_inx()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_bit_zpg()
{
addr = d6502_read6502(d6502_pc++);

value = d6502_read6502(addr);
d6502_f &= 0x3d; /* clear negative, overflow and zero */
d6502_f |= value & 0xc0; /* set negative and overflow */
if((d6502_a & value) == 0)
	d6502_f |= FLAG_ZERO;
}
/************************************************************************************************/
static void op_and_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_rol_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
if(value & 0x80)
	{
	value = value << 1;
	value |= d6502_f & 1; /* carry flag is bit0, hehe */
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value << 1;
	value |= d6502_f & 1;
	d6502_f &= FLAG_CARRY_MASK;
	}

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_plp_imp()
{
pop(d6502_f);
d6502_f |= FLAG_XXX;
}
/************************************************************************************************/
static void op_and_imm()
{
addr = d6502_pc++;

//opcycles++;
d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_rol_acc()
{
//opcycles++;
if(d6502_a & 0x80)
	{
	d6502_a = d6502_a << 1;
	d6502_a |= d6502_f & 1; /* carry flag is bit0, hehe */
	d6502_f |= FLAG_CARRY;
	}
else
	{
	d6502_a = d6502_a << 1;
	d6502_a |= d6502_f & 1;
	d6502_f &= FLAG_CARRY_MASK;
	}
checknz(d6502_a);
}
/************************************************************************************************/
static void op_bit_abs()
{
////opcycles++;
addr = d6502_read6502(d6502_pc++);

////opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

////opcycles++;
value = d6502_read6502(addr);
d6502_f &= 0x3d; /* clear negative, overflow and zero */
d6502_f |= value & 0xc0; /* set negative and overflow */
if((d6502_a & value) == 0)
	d6502_f |= FLAG_ZERO;
}
/************************************************************************************************/
static void op_and_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_rol_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
if(value & 0x80)
	{
	value = value << 1;
	value |= d6502_f & 1; /* carry flag is bit0, hehe */
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value << 1;
	value |= d6502_f & 1;
	d6502_f &= FLAG_CARRY_MASK;
	}

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_bmi_rel()
{
branch(d6502_f & FLAG_NEGATIVE);
}
/************************************************************************************************/
static void op_and_iny()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_and_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_rol_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
if(value & 0x80)
	{
	value = value << 1;
	value |= d6502_f & 1; /* carry flag is bit0, hehe */
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value << 1;
	value |= d6502_f & 1;
	d6502_f &= FLAG_CARRY_MASK;
	}
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_sec_imp()
{
//opcycles++;
d6502_f |= FLAG_CARRY;
}
/************************************************************************************************/
static void op_and_aby()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_and_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a &= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_rol_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
if(value & 0x80)
	{
	value = value << 1;
	value |= d6502_f & 1; /* carry flag is bit0, hehe */
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value << 1;
	value |= d6502_f & 1;
	d6502_f &= FLAG_CARRY_MASK;
	}
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_rti_imp()
{
pop(d6502_f);
pop(d6502_pc);
pop(value);
d6502_f |= FLAG_XXX;
d6502_pc |= value << 8;
}
/************************************************************************************************/
static void op_eor_inx()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_eor_zpg()
{
addr = d6502_read6502(d6502_pc++);

d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_lsr_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
d6502_f &= FLAG_CARRY_MASK;
d6502_f |= value & 1;
value = value >> 1;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_pha_imp()
{
push(d6502_a);
}
/************************************************************************************************/
static void op_eor_imm()
{
addr = d6502_pc++;

//opcycles++;
d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_lsr_acc()
{
//opcycles++;
d6502_f &= FLAG_CARRY_MASK;
d6502_f |= d6502_a & 1;
d6502_a = d6502_a >> 1;
checknz(d6502_a);
}
/************************************************************************************************/
static void op_jmp_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

d6502_pc = addr;
}
/************************************************************************************************/
static void op_eor_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_lsr_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
d6502_f &= FLAG_CARRY_MASK;
d6502_f |= value & 1;
value = value >> 1;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_bvc_rel()
{
branch((d6502_f & FLAG_OVERFLOW) == 0);
}
/************************************************************************************************/
static void op_eor_iny()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_eor_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_lsr_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
d6502_f |= value & 1;
value = value >> 1;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_cli_imp()
{
d6502_f &= FLAG_IRQDISABLE_MASK;
}
/************************************************************************************************/
static void op_eor_aby()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_eor_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a ^= d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_lsr_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
d6502_f |= value & 1;
value = value >> 1;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_rts_imp()
{
pop(d6502_pc);
pop(value);
d6502_pc |= value << 8;
d6502_pc++;
}
/************************************************************************************************/
static void op_adc_inx()
{
u8 zpaddr;
u16 newa;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_adc_zpg()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);

value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ror_zpg()
{
addr = d6502_read6502(d6502_pc++);

value = d6502_read6502(addr);
if(value & 1)
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f &= FLAG_CARRY_MASK;
	}
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_pla_imp()
{
pop(d6502_a);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_adc_imm()
{
u16 newa;

addr = d6502_pc++;

value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ror_acc()
{
if(d6502_a & 1)
	{
	d6502_a = d6502_a >> 1;
	if(d6502_f & FLAG_CARRY)
		d6502_a |= 0x80;
	d6502_f |= FLAG_CARRY;
	}
else
	{
	d6502_a = d6502_a >> 1;
	if(d6502_f & FLAG_CARRY)
		d6502_a |= 0x80;
	d6502_f &= FLAG_CARRY_MASK;
	}
checknz(d6502_a);
}
/************************************************************************************************/
static void op_jmp_ind()
{
u16 p00p;

p00p = d6502_read6502(d6502_pc++);
p00p |= d6502_read6502(d6502_pc++) << 8;
#ifdef JMPBUG
	addr = d6502_read6502(p00p);
	if((p00p & 0xff) == 0xff)
		p00p &= 0xff00;
	else
		p00p++;
	addr |= d6502_read6502(p00p) << 8;
#else
	addr = d6502_read6502(p00p++);
	addr |= d6502_read6502(p00p) << 8;
#endif

d6502_pc = addr;
}
/************************************************************************************************/
static void op_adc_abs()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ror_abs()
{
addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
if(value & 1)
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f &= FLAG_CARRY_MASK;
	}
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_bvs_rel()
{
branch(d6502_f & FLAG_OVERFLOW);
}
/************************************************************************************************/
static void op_adc_iny()
{
u8 zpaddr;
u16 newa;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_adc_zpx()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ror_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
if(value & 1)
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f &= FLAG_CARRY_MASK;
	}
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_sei_imp()
{
d6502_f |= FLAG_IRQDISABLE;
}
/************************************************************************************************/
static void op_adc_aby()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_adc_abx()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
value = d6502_read6502(addr);
newa = d6502_a + value + (d6502_f & FLAG_CARRY); /* how convenient, carry flag is bit0  :) */
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(newa & 0xff00)
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ newa) & (value ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ror_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
if(value & 1)
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f |= FLAG_CARRY;
	}
else
	{
	value = value >> 1;
	if(d6502_f & FLAG_CARRY)
		value |= 0x80;
	d6502_f &= FLAG_CARRY_MASK;
	}
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_sta_inx()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_sty_zpg()
{
addr = d6502_read6502(d6502_pc++);

d6502_write6502(addr,d6502_y);
}
/************************************************************************************************/
static void op_sta_zpg()
{
addr = d6502_read6502(d6502_pc++);

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_stx_zpg()
{
addr = d6502_read6502(d6502_pc++);

d6502_write6502(addr,d6502_x);
}
/************************************************************************************************/
static void op_dey_imp()
{
d6502_y--;
checknz(d6502_y);
}
/************************************************************************************************/
static void op_txa_imp()
{
d6502_a = d6502_x;
checknz(d6502_a);
}
/************************************************************************************************/
static void op_sty_abs()
{
addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

d6502_write6502(addr,d6502_y);
}
/************************************************************************************************/
static void op_sta_abs()
{
addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_stx_abs()
{
addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

d6502_write6502(addr,d6502_x);
}
/************************************************************************************************/
static void op_bcc_rel()
{
branch((d6502_f & FLAG_CARRY) == 0);
}
/************************************************************************************************/
static void op_sta_iny()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
addr += d6502_read6502(zpaddr) << 8;

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_sty_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_write6502(addr,d6502_y);
}
/************************************************************************************************/
static void op_sta_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_stx_zpy()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
addr &= 0xff;

d6502_write6502(addr,d6502_x);
}
/************************************************************************************************/
static void op_tya_imp()
{
d6502_a = d6502_y;
checknz(d6502_a);
}
/************************************************************************************************/
static void op_sta_aby()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
addr += d6502_read6502(d6502_pc++) << 8;

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_txs_imp()
{
d6502_s = d6502_x;
}
/************************************************************************************************/
static void op_sta_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

d6502_write6502(addr,d6502_a);
}
/************************************************************************************************/
static void op_ldy_imm()
{
addr = d6502_pc++;

d6502_y = d6502_read6502(addr);
checknz(d6502_y);
}
/************************************************************************************************/
static void op_lda_inx()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ldx_imm()
{
addr = d6502_pc++;

d6502_x = d6502_read6502(addr);
checknz(d6502_x);
}
/************************************************************************************************/
static void op_ldy_zpg()
{
addr = d6502_read6502(d6502_pc++);

d6502_y = d6502_read6502(addr);
checknz(d6502_y);
}
/************************************************************************************************/
static void op_lda_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ldx_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
d6502_x = d6502_read6502(addr);
checknz(d6502_x);
}
/************************************************************************************************/
static void op_tay_imp()
{
//opcycles++;
d6502_y = d6502_a;
checknz(d6502_y);
}
/************************************************************************************************/
static void op_lda_imm()
{
addr = d6502_pc++;

//opcycles++;
d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_tax_imp()
{
//opcycles++;
d6502_x = d6502_a;
checknz(d6502_x);
}
/************************************************************************************************/
static void op_ldy_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
d6502_y = d6502_read6502(addr);
checknz(d6502_y);
}
/************************************************************************************************/
static void op_lda_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ldx_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
d6502_x = d6502_read6502(addr);
checknz(d6502_x);
}
/************************************************************************************************/
static void op_bcs_rel()
{
branch(d6502_f & FLAG_CARRY);
}
/************************************************************************************************/
static void op_lda_iny()
{
u8 zpaddr;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ldy_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_y = d6502_read6502(addr);
checknz(d6502_y);
}
/************************************************************************************************/
static void op_lda_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ldx_zpy()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
addr &= 0xff;

d6502_x = d6502_read6502(addr);
checknz(d6502_x);
}
/************************************************************************************************/
static void op_clv_imp()
{
//opcycles++;
d6502_f &= FLAG_OVERFLOW_MASK;
}
/************************************************************************************************/
static void op_lda_aby()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_tsx_imp()
{
//opcycles++;
d6502_x = d6502_s;
checknz(d6502_x);
}
/************************************************************************************************/
static void op_ldy_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_y = d6502_read6502(addr);
checknz(d6502_y);
}
/************************************************************************************************/
static void op_lda_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_a = d6502_read6502(addr);
checknz(d6502_a);
}
/************************************************************************************************/
static void op_ldx_aby()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
d6502_x = d6502_read6502(addr);
checknz(d6502_x);
}
/************************************************************************************************/
static void op_cpy_imm()
{
u16 newvalue;

addr = d6502_pc++;

//opcycles++;
newvalue = (d6502_y + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_cmp_inx()
{
u8 zpaddr;
u16 newvalue;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_cpy_zpg()
{
u16 newvalue;

//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
newvalue = (d6502_y + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_cmp_zpg()
{
u16 newvalue;

//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_dec_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
value--;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_iny_imp()
{
//opcycles++;
d6502_y++;
checknz(d6502_y);
}
/************************************************************************************************/
static void op_cmp_imm()
{
u16 newvalue;

addr = d6502_pc++;

//opcycles++;
newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_dex_imp()
{
//opcycles++;
d6502_x--;
checknz(d6502_x);
}
/************************************************************************************************/
static void op_cpy_abs()
{
u16 newvalue;

//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
newvalue = (d6502_y + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_cmp_abs()
{
u16 newvalue;

//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_dec_abs()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
value--;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_bne_rel()
{
branch((d6502_f & FLAG_ZERO) == 0);
}
/************************************************************************************************/
static void op_cmp_iny()
{
u8 zpaddr;
u16 newvalue;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_cmp_zpx()
{
u16 newvalue;

addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_dec_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
value--;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_cld_imp()
{
//opcycles++;
d6502_f &= FLAG_DECIMAL_MASK;
}
/************************************************************************************************/
static void op_cmp_aby()
{
u16 newvalue;

addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_cmp_abx()
{
u16 newvalue;

addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
newvalue = (d6502_a + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_dec_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
value--;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_cpx_imm()
{
u16 newvalue;

addr = d6502_pc++;

newvalue = (d6502_x + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_sbc_inx()
{
u8 zpaddr;
u16 newa;

zpaddr = d6502_read6502(d6502_pc++);
zpaddr += d6502_x;
addr = d6502_read6502(zpaddr++);
addr |= d6502_read6502(zpaddr) << 8;

value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_cpx_zpg()
{
u16 newvalue;

addr = d6502_read6502(d6502_pc++);

newvalue = (d6502_x + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_sbc_zpg()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);

value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_inc_zpg()
{
//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
value = d6502_read6502(addr);

//opcycles++;
d6502_write6502(addr,value);
value++;

//opcycles++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_inx_imp()
{
//opcycles++;
d6502_x++;
checknz(d6502_x);
}
/************************************************************************************************/
static void op_sbc_imm()
{
u16 newa;

addr = d6502_pc++;

value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_nop_imp()
{
}
/************************************************************************************************/
static void op_cpx_abs()
{
u16 newvalue;

//opcycles++;
addr = d6502_read6502(d6502_pc++);

//opcycles++;
addr |= d6502_read6502(d6502_pc++) << 8;

//opcycles++;
newvalue = (d6502_x + 0x100) - d6502_read6502(addr);
d6502_f &= FLAG_CARRY_MASK;
if(newvalue & 0xff00)
	d6502_f |= FLAG_CARRY;
newvalue &= 0xff;
checknz(newvalue);
}
/************************************************************************************************/
static void op_sbc_abs()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_inc_abs()
{
addr = d6502_read6502(d6502_pc++);
addr |= d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
value++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_beq_rel()
{
branch(d6502_f & FLAG_ZERO);
}
/************************************************************************************************/
static void op_sbc_iny()
{
u8 zpaddr;
u16 newa;

zpaddr = d6502_read6502(d6502_pc++);
addr = d6502_read6502(zpaddr++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec++;
addr += d6502_read6502(zpaddr) << 8;

value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_sbc_zpx()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_inc_zpx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr &= 0xff;

value = d6502_read6502(addr);
value++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static void op_sed_imp()
{
d6502_f |= FLAG_DECIMAL;
}
/************************************************************************************************/
static void op_sbc_aby()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr += d6502_y;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_sbc_abx()
{
u16 newa;

addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
if(addr & 0xff00)
	cyclesexec += EXTRA_CYCLE;
addr += d6502_read6502(d6502_pc++) << 8;
value = d6502_read6502(addr);
newa = d6502_a - value - (1 - (d6502_f & FLAG_CARRY));
d6502_f &= 0xbe; /* clear carry and overflow flags */
if(!(newa & 0xff00))
	d6502_f |= FLAG_CARRY;
d6502_f |= (((d6502_a ^ value) & (d6502_a ^ newa)) & 0x80) >> 1;
d6502_a = newa & 0xff;
#ifndef NES
if(d6502_f & FLAG_DECIMAL)
	{
	d6502_a -= 0x66;
	d6502_f &= FLAG_CARRY_MASK;
	if((d6502_a & 0x0f) > 0x09)
		d6502_a += 0x06;
	if((d6502_a & 0xf0) > 0x90)
		{
		d6502_a += 0x60;
		d6502_f |= FLAG_CARRY;
		}
	}
#endif
checknz(d6502_a);
}
/************************************************************************************************/
static void op_inc_abx()
{
addr = d6502_read6502(d6502_pc++);
addr += d6502_x;
addr += d6502_read6502(d6502_pc++) << 8;

value = d6502_read6502(addr);
value++;
d6502_write6502(addr,value);
checknz(value);
}
/************************************************************************************************/
static voidcall fulloptable[256] =
	{
	op_brk_imp,op_ora_inx,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_ora_zpg,op_asl_zpg,op_xxx_xxx, /* 00-07 */
	op_php_imp,op_ora_imm,op_asl_acc,op_xxx_xxx,op_xxx_xxx,op_ora_abs,op_asl_abs,op_xxx_xxx, /* 08-0f */
	op_bpl_rel,op_ora_iny,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_ora_zpx,op_asl_zpx,op_xxx_xxx, /* 10-17 */
	op_clc_imp,op_ora_aby,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_ora_abx,op_asl_abx,op_xxx_xxx, /* 18-1f */
	op_jsr_abs,op_and_inx,op_xxx_xxx,op_xxx_xxx,op_bit_zpg,op_and_zpg,op_rol_zpg,op_xxx_xxx, /* 20-27 */
	op_plp_imp,op_and_imm,op_rol_acc,op_xxx_xxx,op_bit_abs,op_and_abs,op_rol_abs,op_xxx_xxx, /* 28-2f */
	op_bmi_rel,op_and_iny,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_and_zpx,op_rol_zpx,op_xxx_xxx, /* 30-37 */
	op_sec_imp,op_and_aby,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_and_abx,op_rol_abx,op_xxx_xxx, /* 38-3f */
	op_rti_imp,op_eor_inx,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_eor_zpg,op_lsr_zpg,op_xxx_xxx, /* 40-47 */
	op_pha_imp,op_eor_imm,op_lsr_acc,op_xxx_xxx,op_jmp_abs,op_eor_abs,op_lsr_abs,op_xxx_xxx, /* 48-4f */
	op_bvc_rel,op_eor_iny,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_eor_zpx,op_lsr_zpx,op_xxx_xxx, /* 50-57 */
	op_cli_imp,op_eor_aby,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_eor_abx,op_lsr_abx,op_xxx_xxx, /* 58-5f */
	op_rts_imp,op_adc_inx,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_adc_zpg,op_ror_zpg,op_xxx_xxx, /* 60-67 */
	op_pla_imp,op_adc_imm,op_ror_acc,op_xxx_xxx,op_jmp_ind,op_adc_abs,op_ror_abs,op_xxx_xxx, /* 68-6f */
	op_bvs_rel,op_adc_iny,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_adc_zpx,op_ror_zpx,op_xxx_xxx, /* 70-77 */
	op_sei_imp,op_adc_aby,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_adc_abx,op_ror_abx,op_xxx_xxx, /* 78-7f */
	op_xxx_xxx,op_sta_inx,op_xxx_xxx,op_xxx_xxx,op_sty_zpg,op_sta_zpg,op_stx_zpg,op_xxx_xxx, /* 80-87 */
	op_dey_imp,op_xxx_xxx,op_txa_imp,op_xxx_xxx,op_sty_abs,op_sta_abs,op_stx_abs,op_xxx_xxx, /* 88-8f */
	op_bcc_rel,op_sta_iny,op_xxx_xxx,op_xxx_xxx,op_sty_zpx,op_sta_zpx,op_stx_zpy,op_xxx_xxx, /* 90-97 */
	op_tya_imp,op_sta_aby,op_txs_imp,op_xxx_xxx,op_xxx_xxx,op_sta_abx,op_xxx_xxx,op_xxx_xxx, /* 98-9f */
	op_ldy_imm,op_lda_inx,op_ldx_imm,op_xxx_xxx,op_ldy_zpg,op_lda_zpg,op_ldx_zpg,op_xxx_xxx, /* a0-a7 */
	op_tay_imp,op_lda_imm,op_tax_imp,op_xxx_xxx,op_ldy_abs,op_lda_abs,op_ldx_abs,op_xxx_xxx, /* a8-af */
	op_bcs_rel,op_lda_iny,op_xxx_xxx,op_xxx_xxx,op_ldy_zpx,op_lda_zpx,op_ldx_zpy,op_xxx_xxx, /* b0-b7 */
	op_clv_imp,op_lda_aby,op_tsx_imp,op_xxx_xxx,op_ldy_abx,op_lda_abx,op_ldx_aby,op_xxx_xxx, /* b8-bf */
	op_cpy_imm,op_cmp_inx,op_xxx_xxx,op_xxx_xxx,op_cpy_zpg,op_cmp_zpg,op_dec_zpg,op_xxx_xxx, /* c0-c7 */
	op_iny_imp,op_cmp_imm,op_dex_imp,op_xxx_xxx,op_cpy_abs,op_cmp_abs,op_dec_abs,op_xxx_xxx, /* c8-cf */
	op_bne_rel,op_cmp_iny,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_cmp_zpx,op_dec_zpx,op_xxx_xxx, /* d0-d7 */
	op_cld_imp,op_cmp_aby,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_cmp_abx,op_dec_abx,op_xxx_xxx, /* d8-df */
	op_cpx_imm,op_sbc_inx,op_xxx_xxx,op_xxx_xxx,op_cpx_zpg,op_sbc_zpg,op_inc_zpg,op_xxx_xxx, /* e0-e7 */
	op_inx_imp,op_sbc_imm,op_nop_imp,op_xxx_xxx,op_cpx_abs,op_sbc_abs,op_inc_abs,op_xxx_xxx, /* e8-ef */
	op_beq_rel,op_sbc_iny,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_sbc_zpx,op_inc_zpx,op_xxx_xxx, /* f0-f7 */
	op_sed_imp,op_sbc_aby,op_xxx_xxx,op_xxx_xxx,op_xxx_xxx,op_sbc_abx,op_inc_abx,op_xxx_xxx  /* f8-ff */
	};
/************************************************************************************************/
u16 dead6502_getvector(u8 v)
{
switch(v)
	{
	case CPU_NMI:return(d6502_read6502(0xfffa) | (d6502_read6502(0xfffb) << 8));
	case CPU_RESET:return(d6502_read6502(0xfffc) | (d6502_read6502(0xfffd) << 8));
	case CPU_IRQ:return(d6502_read6502(0xfffe) | (d6502_read6502(0xffff) << 8));
	}
return(0);
}
/************************************************************************************************/
u32 dead6502_getcycles()
{
return(cyclesexec);
}
/************************************************************************************************/
void dead6502_setdata(dead6502data *newdata)
{
data = newdata;
}
/************************************************************************************************/
void dead6502_getdata(dead6502data **newdata)
{
*newdata = data;
}
/************************************************************************************************/
void dead6502_init()
{
int i;

for(i=0;i<256;i++)
	{
	cycletable[i] = basecycletable[i];
	nztable[i] = (i==0?2:i&0x80);
	}
nmicycles = 7;
irqcycles = 7;
}

void dead6502_nmi()
{
	d6502_needirq |= CPU_NMI;
}

void dead6502_irq()
{
	d6502_needirq |= CPU_IRQ;
}

void dead6502_reset()
{
	d6502_a = d6502_x = d6502_y = 0;
	d6502_s = 0xFF;
	d6502_f = FLAG_XXX | FLAG_IRQDISABLE;
	d6502_needirq = 0;
	d6502_pc = dead6502_getvector(CPU_RESET);
	d6502_totalcycles = 0;
	addr = 0;
}

static u32 burn = 0;
void dead6502_burn(u32 cycles)
{
//	log_message("burning %d cycles\n",cycles);
	burn += cycles;
}

u32 dead6502_step()
{
	u8 op;

	cyclesexec = burn;
	burn = 0;
	if(d6502_needirq) {
		if(d6502_needirq & CPU_NMI) {
			d6502_f &= ~(FLAG_BRK | FLAG_DECIMAL);
			push(d6502_pc >> 8);
			push(d6502_pc);
			push(d6502_f);
			d6502_pc = d6502_read6502(0xfffa);
			d6502_pc |= d6502_read6502(0xfffb) << 8;
			cyclesexec += nmicycles;
			d6502_needirq &= NMI_MASK;
			return(cyclesexec);
		}
		if(d6502_needirq & CPU_IRQ) {
			if((d6502_f & FLAG_IRQDISABLE) == 0) {
				d6502_f &= ~(FLAG_BRK | FLAG_DECIMAL);
				push(d6502_pc >> 8);
				push(d6502_pc);
				push(d6502_f);
				d6502_f |= FLAG_IRQDISABLE;
				d6502_pc = d6502_read6502(0xfffe);
				d6502_pc |= d6502_read6502(0xffff) << 8;
				cyclesexec += irqcycles;
				d6502_needirq &= IRQ_MASK;
				return(cyclesexec);
			}
		}
	}
	//opcycles++;
	op = d6502_op6502(d6502_pc++);
	//nsf opcode
//	if(op == 2)
//		log_message("nsf opcode\n");
	if(fulloptable[op] != 0)
		fulloptable[op]();
	else {
		d6502_pc--; /*return to the bad opcode*/
		msg("dead6502: bad opcode at $%04X\n",d6502_pc-1);
		d6502_totalcycles += cyclesexec; /*add in cycles taken*/
		return((u32)cyclesexec | BAD_OPCODE); /* bad opcode */
	}
	cyclesexec += cycletable[op];
	return(cyclesexec);
}
