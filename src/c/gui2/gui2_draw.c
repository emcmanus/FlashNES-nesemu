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
#include <ctype.h>
#include "defines.h"
#include "gui2.h"

static u8 *screen;
static int pitch;
static int screen_width,screen_height;

extern u8 font_zsnes_char_width;
extern u8 font_zsnes_char_height;
extern u8 font_zsnes_map[];
extern u8 font_zsnes[];

u8 *gui_draw_getscreen()
{
	return(screen);
}

int gui_draw_getscreenpitch()
{
	return(pitch);
}

int gui_draw_getscreenwidth()
{
	return(screen_width);
}

int gui_draw_getscreenheight()
{
	return(screen_height);
}

void gui_draw_setscreen(u8 *s,int p)
{
	screen = s;
	pitch = p;
}

void gui_draw_setscreensize(int w,int h)
{
	screen_width = w;
	screen_height = h;
}

void gui_draw_pixel(u8 color,int x,int y)
{
	if((x >= screen_width) || (y >= screen_height))
	   return;
	screen[x + (y * pitch)] = (u8)(color + GUI_COLOR_OFFSET);
}

void gui_draw_hline(u8 color,int x,int y,int len)
{
	int i;

	for(i=0;i<len;i++)
	   gui_draw_pixel(color,x + i,y);
}

void gui_draw_vline(u8 color,int x,int y,int len)
{
	int i;

	for(i=0;i<len;i++)
	   gui_draw_pixel(color,x,y + i);
}

void gui_draw_box(u8 color,int x,int y,int w,int h)
{
	gui_draw_hline(color,x,y,w);
	gui_draw_hline(color,x,y + h,w);
	gui_draw_vline(color,x,y,h);
	gui_draw_vline(color,x + w,y,h);
	gui_draw_pixel(color,x + w,y + h);
}

void gui_draw_fillbox(u8 color,int x,int y,int w,int h)
{
	int i;

	for(i=0;i<h;i++)
		gui_draw_hline(color,x,y + i,w);
}

static char translatechar(char ch)
{
	int i,n;

	n = 0x50 - 2;
	for(i=0;i<n;i++) {
		if(font_zsnes_map[i] == toupper(ch))
		   return(i);
	}
	return(0);
}

static u8 *getfontdata(char ch)
{
	return(font_zsnes + (font_zsnes_char_width * translatechar(ch)));
}

void gui_draw_char(u8 color,int x,int y,char ch)
{
	u8 *fontdata = getfontdata(ch);
	int cx,cy;

	for(cy=0;cy<5;cy++) {
		for(cx=0;cx<5;cx++) {
			if((fontdata[cy] >> (4 - cx)) & 1) {
				gui_draw_pixel(color,x + cx,y + cy);
				gui_draw_pixel(GUI_COLOR_DARKGREY,x + cx + 1,y + cy + 1);
			}
		}
	}
}

void gui_draw_text(u8 color,int x,int y,char *str)
{
	for(;*str;str++) {
		gui_draw_char(color,x,y,*str);
		x += font_zsnes_char_width + 1;
	}
}

void gui_draw_text2(u8 color,int x,int y,int len,char *str)
{
	for(;*str && len--;str++) {
		gui_draw_char(color,x,y,*str);
		x += font_zsnes_char_width + 1;
	}
}

void gui_draw_text_highlight(u8 color,u8 bgcolor,int x,int y,char *str)
{
	int i;

	for(;*str;str++) {
		for(i=0;i<font_zsnes_char_height;i++)
		   gui_draw_hline(bgcolor,x,y+i,strlen(str) * font_zsnes_char_width);
		gui_draw_char(color,x,y,*str);
		x += font_zsnes_char_width + 1;
	}
}

void gui_draw_text2_highlight(u8 color,u8 bgcolor,int x,int y,int len,char *str)
{
	int i;

	for(i=0;i<font_zsnes_char_height;i++)
	   gui_draw_hline(bgcolor,x,y+i,len * (font_zsnes_char_width + 1));
	for(;*str && len--;str++) {
		gui_draw_char(color,x,y,*str);
		x += font_zsnes_char_width + 1;
	}
}

//----- more complex things to draw -----//

void gui_draw_window(char *title,int x,int y,int w,int h)
{
	gui_draw_fillbox(GUI_WINDOWBACKGROUND,x,y,w,h);
	gui_draw_fillbox(GUI_WINDOWTITLEBAR,x,y,w,9);
	gui_draw_box(GUI_WINDOWBORDER,x,y,w,h);
	gui_draw_text(GUI_TEXT,x + 2,y + 2,title);
}

void gui_draw_border(u8 color,int x,int y,int w,int h)
{
	gui_draw_fillbox(color,x,y,w,h);
	gui_draw_hline(GUI_COLOR_LIGHTGREY,x,y,w);
	gui_draw_hline(GUI_COLOR_DARKGREY,x,y+h,w);
	gui_draw_vline(GUI_COLOR_LIGHTGREY,x,y,h);
	gui_draw_vline(GUI_COLOR_DARKGREY,x+w,y,h);
}

void gui_draw_borderpressed(u8 color,int x,int y,int w,int h)
{
	gui_draw_fillbox(color,x,y,w,h);
	gui_draw_hline(GUI_COLOR_DARKGREY,x,y,w);
	gui_draw_hline(GUI_COLOR_LIGHTGREY,x,y+h,w);
	gui_draw_vline(GUI_COLOR_DARKGREY,x,y,h);
	gui_draw_vline(GUI_COLOR_LIGHTGREY,x+w,y,h);
}

void gui_draw_button(char *caption,int x,int y,int w,int h)
{
	gui_draw_border(GUI_BUTTONBACKGROUND,x,y,w,h);
	gui_draw_text(GUI_TEXT,x+2,y+2,caption);
}

void gui_draw_buttonpressed(char *caption,int x,int y,int w,int h)
{
	gui_draw_borderpressed(GUI_BUTTONBACKGROUND,x,y,w,h);
	gui_draw_text(GUI_TEXT,x+2,y+2,caption);
}

#define N GUI_COLOR_DARKERORANGE
#define M GUI_COLOR_LIGHTERORANGE
#define P GUI_COLOR_ORANGE

static u8 cursor[10*10] = {
	P,N,N,N,N,0,0,0,0,0,
	P,P,N,N,0,0,0,0,0,0,
	M,P,P,N,0,0,0,0,0,0,
	M,M,P,P,N,0,0,0,0,0,
	M,0,0,P,P,N,0,0,0,0,
	0,0,0,0,P,P,N,0,0,0,
	0,0,0,0,0,P,P,N,0,0,
	0,0,0,0,0,0,P,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};

extern int joyx,joyy;

void gui_draw_cursor()
{
	int i,j;
	u8 p;

	for(j=0;j<10;j++) {
		for(i=0;i<10;i++) {
			if((p = cursor[i + j * 10]) != 0)
				gui_draw_pixel(p,joyx + i,joyy + j);
		}
	}
}



//32 bit drawing text functions (for status messages)

extern u32 gui_palette[];

void gui_draw_pixel32(u32 *dest,int pitch,u8 color,int x,int y)
{
//	if((x >= screen_width) || (y >= screen_height))
//	   return;
	dest[x + (y * pitch)] = gui_palette[color];
}

void gui_draw_char32(u32 *dest,int pitch,u8 color,int x,int y,char ch)
{
	u8 *fontdata = getfontdata(ch);
	int cx,cy;

	for(cy=0;cy<5;cy++) {
		for(cx=0;cx<5;cx++) {
			if((fontdata[cy] >> (4 - cx)) & 1) {
				gui_draw_pixel32(dest,pitch,color,x + cx,y + cy);
				gui_draw_pixel32(dest,pitch,GUI_COLOR_DARKGREY,x + cx + 1,y + cy + 1);
			}
		}
	}
}

//draw text to 32bit destination surface
void gui_draw_text32(u32 *dest,int pitch,u8 color,int x,int y,char *str)
{
	for(;*str;str++) {
		gui_draw_char32(dest,pitch,color,x,y,*str);
		x += font_zsnes_char_width + 1;
	}
}
