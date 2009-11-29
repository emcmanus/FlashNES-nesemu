#ifndef __DEAD6502_H__
#define __DEAD6502_H__
/************************************************************************************************/
#define DEAD6502_PAGES4096 /* pick your page size here */
/************************************************************************************************/
#define BAD_OPCODE				0x80000000
#define CPU_NMI					0x01
#define CPU_IRQ					0x02
#define CPU_RESET					0x04
#if defined(DEAD6502_PAGES4096)
	#define DEAD6502_PAGE_SIZE		0x1000
	#define DEAD6502_PAGE_SHIFT	12
#elif defined(DEAD6502_PAGES2048)
	#define DEAD6502_PAGE_SIZE		0x0800
	#define DEAD6502_PAGE_SHIFT	11
#elif defined(DEAD6502_PAGES1024)
	#define DEAD6502_PAGE_SIZE		0x0400
	#define DEAD6502_PAGE_SHIFT	10
#elif defined(DEAD6502_PAGES512)
	#define DEAD6502_PAGE_SIZE		0x0200
	#define DEAD6502_PAGE_SHIFT	9
#elif defined(DEAD6502_PAGES256)
	#define DEAD6502_PAGE_SIZE		0x0100
	#define DEAD6502_PAGE_SHIFT	8
#endif
#if !defined(DEAD6502_PAGE_SIZE) || !defined(DEAD6502_PAGE_SHIFT)
	#undef DEAD6502_PAGE_SIZE
	#undef DEAD6502_PAGE_SHIFT
	#define DEAD6502_PAGE_SIZE		0x0800
	#define DEAD6502_PAGE_SHIFT	11
#endif
#define DEAD6502_PAGE_MASK		(DEAD6502_PAGE_SIZE - 1)
#define DEAD6502_PAGE_NUM		(0x10000 / DEAD6502_PAGE_SIZE)
/************************************************************************************************/
#ifdef PS2
#include <tamtypes.h>
#else
/*#ifdef __DEAD6502COMPILED__*/
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long int s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long int u64;
/*#endif*/
#endif
typedef u8 (*readfunc_t)(u32);
typedef void (*writefunc_t)(u32,u8);
typedef struct /* do not change order of these, only add stuff to bottom */
	{
	u8 a; /* accumulator */
	u8 x;
	u8 y;
	u8 s; /* stack pointer */
	u8 f; /* flags */
	u16 pc; /* program counter */
	u32 totalcycles;
	u8 needirq; /* if we need nmi or irq */
//	u8 (*op6502)(u32); /* read opcodes only function */
//	u8 (*read6502)(u32); /* read all but opcodes function */
//	void (*write6502)(u32,u8); /* write function */
	readfunc_t op6502[16];
	readfunc_t read6502[16];
	writefunc_t write6502[16];
	u8 *oppages[DEAD6502_PAGE_NUM];
	u8 *readpages[DEAD6502_PAGE_NUM];
	u8 *writepages[DEAD6502_PAGE_NUM];
	u8 resoulution; /*set to 1, most of the time not needed*/
#ifdef EXECFRAME
	u32 framecycles; /*number of frame cycles*/
	u32 scanlinecycles; /*number of scanline cycles*/
	void (*line)(u32);
#endif
	} dead6502data;
/************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
u8 dead6502_read(u32 addr);
void dead6502_write(u32 addr,u8 data);
u16 dead6502_disassembleopcode(char *buffer,u16 opcodepos);
u8 dead6502_getlastopcode();
u16 dead6502_getlastopcodepc();
u8 dead6502_getcurrentopcode();
u16 dead6502_getcurrentopcodepc();
u8 dead6502_getnextopcode();
u16 dead6502_getnextopcodepc();
u16 dead6502_getvector(u8 v);
u32 dead6502_getcycles();
void dead6502_addcycles(u32 numcycles);
void dead6502_setdata(dead6502data *newdata);
void dead6502_getdata(dead6502data **newdata);
void dead6502_push(u8 data);
u8 dead6502_pop();
void dead6502_init();
void dead6502_nmi();
void dead6502_irq();
void dead6502_reset();
void dead6502_stop();
void dead6502_burn(u32 cycles);
u32 dead6502_step();
u32 dead6502_exec(u32 cycles);
#ifdef EXECFRAME
u32 dead6502_execframe();
#endif
#ifdef __cplusplus
}
#endif
/************************************************************************************************/
#endif
