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

#ifndef __gui2_draw_h__
#define __gui2_draw_h__

#include "defines.h"

u8 *gui_draw_getscreen();
int gui_draw_getscreenpitch();
int gui_draw_getscreenwidth();
int gui_draw_getscreenheight();

void gui_draw_setscreen(u8 *s,int p);
void gui_draw_setscreensize(int w,int h);

void gui_draw_pixel(u8 color,int x,int y);
void gui_draw_hline(u8 color,int x,int y,int len);
void gui_draw_vline(u8 color,int x,int y,int len);
void gui_draw_box(u8 color,int x,int y,int w,int h);
void gui_draw_fillbox(u8 color,int x,int y,int w,int h);
void gui_draw_char(u8 color,int x,int y,char ch);
void gui_draw_text(u8 color,int x,int y,char *str);
void gui_draw_text_highlight(u8 color,u8 bgcolor,int x,int y,char *str);
void gui_draw_text2(u8 color,int x,int y,int len,char *str);
void gui_draw_text2_highlight(u8 color,u8 bgcolor,int x,int y,int len,char *str);

void gui_draw_window(char *title,int x,int y,int w,int h);
void gui_draw_border(u8 color,int x,int y,int w,int h);
void gui_draw_borderpressed(u8 color,int x,int y,int w,int h);
void gui_draw_button(char *caption,int x,int y,int w,int h);
void gui_draw_buttonpressed(char *caption,int x,int y,int w,int h);

void gui_draw_cursor();

#endif
