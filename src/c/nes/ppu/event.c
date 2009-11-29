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

#include "nes/ppu/event.h"
#include "nes/nes.h"
#include "nes/cpu/dead6502.h"
#include "system/video.h"

static u8 screenline[(256 + 32) * 16];
static int scanline20_deadcycle;
static int curevent;
int ppucycles,lastppucycles;
u32 ppuframes;
int ppulaststatusread;

//set vint flag
static void event_0()
{
	scanline20_deadcycle = 0;

//	if(ppulaststatusread == (89340 - scanline20_deadcycle))
//		nes->ppu.suppressvblflag = 1;



//	BIG PROBLEM:
//		with current cycle system, we cannot detect a suppressed nmi
//		when reading...

//	if(ppulaststatusread == (89342 - scanline20_deadcycle))
//		nes->ppu.nmi = 0;
//	if(ppulaststatusread == 0)
//		nes->ppu.nmi = 0;
	if(nes->ppu.suppressvblflag == 0)
		nes->ppu.status |= 0x80;
	nes->ppu.suppressvblflag = 0;
	//update the ppu nmi enabled flag
	//TODO: necessary still?  it is updated when ctrl0 is updated
//	nes->ppu.nmi = nes->ppu.ctrl0 & 0x80;
}

static int nmilock;

//execute nmi
static void event_2()
{
	//after this cycle, ppu nmi flag is locked
	nmilock = nes->ppu.nmi;
}

//execute nmi
static void event_6()
{
	//if we can execute an nmi, do it now
	if(nmilock)
		dead6502_nmi();
}
/*
//set vint flag
static void event_0()
{
	scanline20_deadcycle = 0;
//	if(ppulaststatusread > 0)
//	log_message("ppulaststatusread = %d\n",ppulaststatusread);
	if(nes->ppu.suppressvblflag == 0)
		nes->ppu.status |= 0x80;
	nes->ppu.suppressvblflag = 0;
}

//execute nmi
static void event_2()
{
	nes->ppu.nmi = nes->ppu.ctrl0 & 0x80;
}

//execute nmi
static void event_6()
{
	if(nes->ppu.nmi) {
//		nes->ppu.nmi = 1;
		dead6502_nmi();
	}
}
*/
//beginning of scanline 20
static void event_6820()
{
	//clear ppu VINT flag, sprite0 hit flag, and sprite overflow flag
	nes->ppu.status = 0;
	nes->scanline = -1;
}

//scanline 20, cycle 256
static void event_7076()
{
	//update scroll counters
	if(nes->ppu.ctrl1 & 0x18)
		nes->ppu.scroll = nes->ppu.temp;
}

//scanline 20, cycle 328
static void event_7148()
{
	int i;

	//determine if this is a short frame or not
	if(((nes->ppu.ctrl1 & 8) != 0) && ((ppuframes & 1) != 0))
		scanline20_deadcycle = 1;
	else
		scanline20_deadcycle = 0;

	//refresh sprites for first visible line
	ppu_getsprites();

	//go to next scanline 13 ppu cycles early...
	nes->scanline++;

	//call mapper line callback for 21 lines
	for(i=0;i<=20 && nes->mapper->line;i++)
      nes->mapper->line(i,341);

	//save ppu time last line func was called
	//TODO: not absolutely necessary, the mapper line callback
	//could keep track of this, since it is always updated after
	//the mapper line callback is called
	lastppucycles = ppucycles;
}

extern u8 nes_frame_irqmode;
extern u8 nes_frameirq;

//last cycle
static void event_89341()
{
//	end_frame_counter(ppucycles / 3);
/*	if((nes_frame_irqmode & 0x40) == 0) {
//		log_message("generating frame irq\n");
		nes->apu->frame_irq_occurred = 1;
		nes_frameirq = 0x40;
		apu_setcontext(nes->apu);
		dead6502_irq();
	}*/
}

//scanline cycle 340, lines 21,260
static void event_line_340()
{
	//refresh  sprites if they are visible
	if(nes->ppu.ctrl1 & 0x10) {
		//refresh sprites for this line
		ppu_getsprites();
	}

	//mapper hblank callback
	if(nes->mapper->line)
		nes->mapper->line(nes->scanline + 21,ppucycles - lastppucycles);

	//save ppu time
	lastppucycles = ppucycles;

	//increment scanline counter
	nes->scanline++;
}

//scanline cycle 256, lines 21-260
static void event_line_256()
{
	int n;

	//update x coordinate
	if(nes->ppu.ctrl1 & 0x18)
		nes->ppu.scroll = (nes->ppu.scroll & ~0x041F) | (nes->ppu.temp & 0x041F);

	//draw single line of the screen
	ppu_drawline(screenline);

	//send off the line to the video output system
	video_updateline(nes->scanline,screenline + 8);

	//if this is the last visible scanline, signal the video system
	if(nes->scanline == 239) {
		video_endframe();
		apu_endframe();
	}

	//update y coordinate
	if(nes->ppu.ctrl1 & 0x18) {
		if((nes->ppu.scroll >> 12) == 7) {
			nes->ppu.scroll &= ~0x7000;
			n = (nes->ppu.scroll >> 5) & 0x1F;
			if(n == 29) {
				nes->ppu.scroll &= ~0x03E0;
				nes->ppu.scroll ^= 0x0800;
			}
			else if(n == 31)
				nes->ppu.scroll &= ~0x03E0;
			else
				nes->ppu.scroll += 0x20;
		}
		else
			nes->ppu.scroll += 0x1000;
	}
}

#define EVENT(c)		{c,event_##c}
#define EVENT_LINE(c){7417+(c*341),event_line_256},{7501+(c*341),event_line_340}
#define EVENT_END()  {0xFFFFFFFF,0}

static ppu_event_t events[] = {
	EVENT(0),			//set vint flag
	EVENT(2),			//execute nmi if necessary
	EVENT(6),			//execute nmi if necessary
	EVENT(6820),		//scanline 20, cycle 0
	EVENT(7076),		//scanline 20, cycle 256
	EVENT(7148),		//scanline 20, cycle 328
//	EVENT(29830),		//scanline 65, cycle ? (apu frame irq)
//	EVENT(29831),		//scanline 65, cycle ? (apu frame irq)
//	EVENT(29832),		//scanline 65, cycle ? (apu frame irq)

	//updates for scanlines happen on cycle 256
	EVENT_LINE(0),		EVENT_LINE(1),		EVENT_LINE(2),		EVENT_LINE(3),
	EVENT_LINE(4),		EVENT_LINE(5),		EVENT_LINE(6),		EVENT_LINE(7),
	EVENT_LINE(8),		EVENT_LINE(9),		EVENT_LINE(10),	EVENT_LINE(11),
	EVENT_LINE(12),	EVENT_LINE(13),	EVENT_LINE(14),	EVENT_LINE(15),
	EVENT_LINE(16),	EVENT_LINE(17),	EVENT_LINE(18),	EVENT_LINE(19),
	EVENT_LINE(20),	EVENT_LINE(21),	EVENT_LINE(22),	EVENT_LINE(23),
	EVENT_LINE(24),	EVENT_LINE(25),	EVENT_LINE(26),	EVENT_LINE(27),
	EVENT_LINE(28),	EVENT_LINE(29),	EVENT_LINE(30),	EVENT_LINE(31),
	EVENT_LINE(32),	EVENT_LINE(33),	EVENT_LINE(34),	EVENT_LINE(35),
	EVENT_LINE(36),	EVENT_LINE(37),	EVENT_LINE(38),	EVENT_LINE(39),
	EVENT_LINE(40),	EVENT_LINE(41),	EVENT_LINE(42),	EVENT_LINE(43),
	EVENT_LINE(44),	EVENT_LINE(45),	EVENT_LINE(46),	EVENT_LINE(47),
	EVENT_LINE(48),	EVENT_LINE(49),	EVENT_LINE(50),	EVENT_LINE(51),
	EVENT_LINE(52),	EVENT_LINE(53),	EVENT_LINE(54),	EVENT_LINE(55),
	EVENT_LINE(56),	EVENT_LINE(57),	EVENT_LINE(58),	EVENT_LINE(59),
	EVENT_LINE(60),	EVENT_LINE(61),	EVENT_LINE(62),	EVENT_LINE(63),
	EVENT_LINE(64),	EVENT_LINE(65),	EVENT_LINE(66),	EVENT_LINE(67),
	EVENT_LINE(68),	EVENT_LINE(69),	EVENT_LINE(70),	EVENT_LINE(71),
	EVENT_LINE(72),	EVENT_LINE(73),	EVENT_LINE(74),	EVENT_LINE(75),
	EVENT_LINE(76),	EVENT_LINE(77),	EVENT_LINE(78),	EVENT_LINE(79),
	EVENT_LINE(80),	EVENT_LINE(81),	EVENT_LINE(82),	EVENT_LINE(83),
	EVENT_LINE(84),	EVENT_LINE(85),	EVENT_LINE(86),	EVENT_LINE(87),
	EVENT_LINE(88),	EVENT_LINE(89),	EVENT_LINE(90),	EVENT_LINE(91),
	EVENT_LINE(92),	EVENT_LINE(93),	EVENT_LINE(94),	EVENT_LINE(95),
	EVENT_LINE(96),	EVENT_LINE(97),	EVENT_LINE(98),	EVENT_LINE(99),
	EVENT_LINE(100),	EVENT_LINE(101),	EVENT_LINE(102),	EVENT_LINE(103),
	EVENT_LINE(104),	EVENT_LINE(105),	EVENT_LINE(106),	EVENT_LINE(107),
	EVENT_LINE(108),	EVENT_LINE(109),	EVENT_LINE(110),	EVENT_LINE(111),
	EVENT_LINE(112),	EVENT_LINE(113),	EVENT_LINE(114),	EVENT_LINE(115),
	EVENT_LINE(116),	EVENT_LINE(117),	EVENT_LINE(118),	EVENT_LINE(119),
	EVENT_LINE(120),	EVENT_LINE(121),	EVENT_LINE(122),	EVENT_LINE(123),
	EVENT_LINE(124),	EVENT_LINE(125),	EVENT_LINE(126),	EVENT_LINE(127),
	EVENT_LINE(128),	EVENT_LINE(129),	EVENT_LINE(130),	EVENT_LINE(131),
	EVENT_LINE(132),	EVENT_LINE(133),	EVENT_LINE(134),	EVENT_LINE(135),
	EVENT_LINE(136),	EVENT_LINE(137),	EVENT_LINE(138),	EVENT_LINE(139),
	EVENT_LINE(140),	EVENT_LINE(141),	EVENT_LINE(142),	EVENT_LINE(143),
	EVENT_LINE(144),	EVENT_LINE(145),	EVENT_LINE(146),	EVENT_LINE(147),
	EVENT_LINE(148),	EVENT_LINE(149),	EVENT_LINE(150),	EVENT_LINE(151),
	EVENT_LINE(152),	EVENT_LINE(153),	EVENT_LINE(154),	EVENT_LINE(155),
	EVENT_LINE(156),	EVENT_LINE(157),	EVENT_LINE(158),	EVENT_LINE(159),
	EVENT_LINE(160),	EVENT_LINE(161),	EVENT_LINE(162),	EVENT_LINE(163),
	EVENT_LINE(164),	EVENT_LINE(165),	EVENT_LINE(166),	EVENT_LINE(167),
	EVENT_LINE(168),	EVENT_LINE(169),	EVENT_LINE(170),	EVENT_LINE(171),
	EVENT_LINE(172),	EVENT_LINE(173),	EVENT_LINE(174),	EVENT_LINE(175),
	EVENT_LINE(176),	EVENT_LINE(177),	EVENT_LINE(178),	EVENT_LINE(179),
	EVENT_LINE(180),	EVENT_LINE(181),	EVENT_LINE(182),	EVENT_LINE(183),
	EVENT_LINE(184),	EVENT_LINE(185),	EVENT_LINE(186),	EVENT_LINE(187),
	EVENT_LINE(188),	EVENT_LINE(189),	EVENT_LINE(190),	EVENT_LINE(191),
	EVENT_LINE(192),	EVENT_LINE(193),	EVENT_LINE(194),	EVENT_LINE(195),
	EVENT_LINE(196),	EVENT_LINE(197),	EVENT_LINE(198),	EVENT_LINE(199),
	EVENT_LINE(200),	EVENT_LINE(201),	EVENT_LINE(202),	EVENT_LINE(203),
	EVENT_LINE(204),	EVENT_LINE(205),	EVENT_LINE(206),	EVENT_LINE(207),
	EVENT_LINE(208),	EVENT_LINE(209),	EVENT_LINE(210),	EVENT_LINE(211),
	EVENT_LINE(212),	EVENT_LINE(213),	EVENT_LINE(214),	EVENT_LINE(215),
	EVENT_LINE(216),	EVENT_LINE(217),	EVENT_LINE(218),	EVENT_LINE(219),
	EVENT_LINE(220),	EVENT_LINE(221),	EVENT_LINE(222),	EVENT_LINE(223),
	EVENT_LINE(224),	EVENT_LINE(225),	EVENT_LINE(226),	EVENT_LINE(227),
	EVENT_LINE(228),	EVENT_LINE(229),	EVENT_LINE(230),	EVENT_LINE(231),
	EVENT_LINE(232),	EVENT_LINE(233),	EVENT_LINE(234),	EVENT_LINE(235),
	EVENT_LINE(236),	EVENT_LINE(237),	EVENT_LINE(238),	EVENT_LINE(239),

	EVENT(89341),		//scanline 261, cycle 340
	EVENT_END()
};

void ppu_event_init()
{
	curevent = 0;
	ppucycles = 0;
	ppuframes = 0;
//	ppulaststatusread = 0;
	scanline20_deadcycle = 0;
}

void ppu_event_kill()
{
}

//for debugging purposes only
int ppu_event_step()
{
	ppucycles += dead6502_step() * 3;
	if(ppucycles >= 0x80000000)
		return(1);
	while((u32)ppucycles >= (events[curevent].timestamp - scanline20_deadcycle))
		events[curevent++].func();
	while(ppucycles >= 89341) {
		ppucycles -= 89342 - scanline20_deadcycle;
		ppuframes++;
	}
	return(0);
}

int ppu_event_frame()
{
	curevent = 0;
	while((u32)ppucycles >= events[curevent].timestamp)
		events[curevent++].func();
	for(;;) {
//		char buf[128];dead6502_disassembleopcode(buf,nes->cpu.pc);log_message("$%04X:  %s\n",nes->cpu.pc,buf);
		ppucycles += dead6502_step() * 3;
		if((u32)ppucycles >= 0x80000000)
			return(1);
		while((u32)ppucycles >= (events[curevent].timestamp - scanline20_deadcycle))
			events[curevent++].func();
		if(ppucycles >= (89342 - scanline20_deadcycle))
			break;
	}
	ppucycles -= 89342 - scanline20_deadcycle;
	ppuframes++;
	return(0);
}
