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

#include <string.h>
#include "defines.h"
#include "mappers/mapper.h"
#include "nes/ppu/ppu.h"
#include "nes/nes.h"

#define MAPPER(n) \
	extern mapper_ines_t mapper##n; \
	if(num == n)return(&mapper##n);

#ifdef __cplusplus
extern "C" 
#endif
mapper_ines_t *mapper_init_ines(int num)
{
	MAPPER(0);		MAPPER(1);		MAPPER(2);		MAPPER(3);
	MAPPER(4);		MAPPER(5);		MAPPER(6);		MAPPER(7);
	MAPPER(8);		MAPPER(9);		MAPPER(10);		MAPPER(11);
	MAPPER(13);
	MAPPER(15);	/*	MAPPER(16);*/	MAPPER(18);
	MAPPER(19);
						MAPPER(21);		MAPPER(22);		MAPPER(23);
	MAPPER(24);		MAPPER(25);		MAPPER(26);		MAPPER(27);
	MAPPER(33);		MAPPER(34);
	MAPPER(39);
	MAPPER(40);		MAPPER(41);		MAPPER(42);
	MAPPER(44);		MAPPER(45);		MAPPER(46);		MAPPER(47);
	MAPPER(48);		MAPPER(49);		MAPPER(50);		MAPPER(51);
	MAPPER(52);
	MAPPER(57);		MAPPER(58);
	MAPPER(60);		MAPPER(61);		MAPPER(62);
	MAPPER(64);		MAPPER(65);		MAPPER(66);		MAPPER(67);
	MAPPER(68);		MAPPER(69);		MAPPER(70);		MAPPER(71);
	MAPPER(72);		MAPPER(73);		MAPPER(75);
	MAPPER(79);
	MAPPER(85);		MAPPER(87);
	MAPPER(90);		MAPPER(92);
	MAPPER(94);
	MAPPER(105);	MAPPER(107);	MAPPER(108);
	MAPPER(112);	MAPPER(113);
	MAPPER(115);	MAPPER(118);	MAPPER(119);
	MAPPER(132);	MAPPER(133);
	MAPPER(137);
	MAPPER(140);	MAPPER(143);
	MAPPER(164);	MAPPER(165);
	MAPPER(170);
	MAPPER(172);	MAPPER(173);
	MAPPER(180);
	MAPPER(184);	MAPPER(185);
	MAPPER(189);
	MAPPER(191);	MAPPER(192);
	MAPPER(193);	MAPPER(194);
	MAPPER(197);
	MAPPER(200);	MAPPER(201);	MAPPER(203);
	MAPPER(209);	MAPPER(210);
	MAPPER(212);	MAPPER(214);
	MAPPER(225);
	MAPPER(230);	MAPPER(231);	MAPPER(232);
	MAPPER(234);
	MAPPER(240);	MAPPER(242);
	MAPPER(245);	MAPPER(246);
	MAPPER(249);	MAPPER(253);

	//is this mapper still valid?
	MAPPER(254);

	//nsf
	MAPPER(NSF_MAPPER);

	//no mapper found, return error
	return(0);
}
