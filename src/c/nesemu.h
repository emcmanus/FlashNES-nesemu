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

#ifndef __nesemu_h__
#define __nesemu_h__

extern int quit;

int init();
void nesemu_kill();
int loadrom(char *fn);

void loadstate();
void savestate();
void loaddiskstate();
void savediskstate();

#ifdef FLASH
AS3_Val f_setup(void *data, AS3_Val args);
AS3_Val f_loop(void *data, AS3_Val args);
AS3_Val f_teardown(void *data, AS3_Val args);
AS3_Val f_screen(void *data, AS3_Val args);
#endif

#endif

