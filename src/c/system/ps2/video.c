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

#include "g2.h"
#include "gs.h"
#include "system/system.h"
#include "gui2/colors.h"

static u16 maxx, maxy;
static u32 palette[256];
static u32 *screen = 0;
static u32 *screen2 = 0;

static void clearscreen()
{
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);
}

void flip_buffers()
{
	g2_set_visible_frame(1 - g2_get_visible_frame());
	g2_set_active_frame(1 - g2_get_active_frame());
}

void video_reinit()
{
}

void video_init()
{
	if(gs_is_ntsc())
		g2_init(NTSC_256_224_32);
	else
		g2_init(PAL_256_256_32);
	maxx = g2_get_max_x();
	maxy = g2_get_max_y();
	g2_set_active_frame(0);
	g2_set_visible_frame(0);

	clearscreen();
	screen = (u32*)malloc(256 * 256 * sizeof(u32));
	screen2 = (u32*)malloc(256 * 256 * sizeof(u32));
	memset(screen,GUI_COLOR_OFFSET,(256+8)*240);
	memset(screen2,GUI_COLOR_OFFSET,(256+8)*240);
	g2_wait_vsync();
	g2_put_image(0,0,256,224,screen);
	flip_buffers();
	g2_put_image(0,0,256,224,screen2);
	flip_buffers();
}

void video_kill()
{
	if(screen)
		free(screen);
	if(screen2)
		free(screen2);
	screen = 0;
	screen2 = 0;
	g2_end();
}

void video_setpalentry(int i,u8 r,u8 g,u8 b)
{
	palette[i] = 0xFF000000 | (b << 16) | (g << 8) | (r << 0);
}

void video_updateline(int line,u8 *s)
{
	int x;
	u64 *dest;

	if((line < 8) || (line > 232))
		return;

	//offset to line to be drawn
	line -= 8;
	dest = screen + (line * 256);

	//draw the line
	for(x=0;x<256;x+=2) {
		dest[x / 2] = (u64)palette[s[x+0]] | ((u64)palette[s[x+1]] << 32);
	}
}

extern u8 disk_read[24][24];
extern u8 disk_write[24][24];
extern int fdsdrive;

void video_endframe()
{
	static int fastforward = 0;

	if(joykeys[config.gui_keys[0]]) {
		if(fastforward-- > 0)
			return;
		else
			fastforward = 10;
	}

	if(nes && nes->rom && nes->rom->mapper == 20 && fdsdrive) {
		//draw fds drive read/write icon...
		u32 *scr = screen;
		int x,y;

		scr += (8 * 256) + (256 - 32);
		for(y=0;y<24;y++) {
			for(x=0;x<24;x++) {
				u8 p;

				if(fdsdrive & 2)
					p = disk_write[y][x];
				else
					p = disk_read[y][x];
				if(p)
					scr[x] = palette[p];
			}
			scr += 256;
		}
		fdsdrive = 0;
	}

	g2_wait_vsync();
	g2_put_image(0,0,256,224,screen);
	flip_buffers();
}

void video_update(u8 *s,int spitch)
{
	int x,y;
	u32 *dest = screen;

//	s += spitch * 16;
	for(y=0;y<224;y++) {
		for(x=0;x<256;x++) {
			if(s[x])
				dest[x] = palette[s[x]];
			else
				dest[x] = (screen2[x + y * 256] >> 2) & 0x3F3F3F3F;
		}
		s += spitch;
		dest += 256;
	}
	g2_wait_vsync();
	g2_put_image(0,0,256,224,screen);
	flip_buffers();
}


void video_copyscreen()
{
	int x,y;

	for(y=0;y<240;y++) {
		for(x=0;x<256;x++)
			screen2[x + y * 256] = screen[x + y * 256];
	}
}

u32 video_getpixel(int x,int y)
{
	return(screen[x + y * 256]);
}

u32 video_getpalette(int idx)
{
	return(palette[idx]);
}

void video_minimize()
{
	//no need to minimize on the ps2
}

void video_togglefullscreen()
{
	//ps2 is always fullscreen
}
