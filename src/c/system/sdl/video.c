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

/* rewritten sdl video using libfilters, needs a rewrite */

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include "defines.h"
#include "system/system.h"
#include "gui2/gui2.h"

//#include "libfilter.h"

#include "system/sdl/scale2x/scalebit.h"
#include "system/sdl/interpolate/interpolate.h"
#include "system/sdl/hq2x/hq2x.h"
#include "system/sdl/hq2x/hq3x.h"
#include "system/sdl/nes_ntsc/nes_ntsc.h"

//#define _PROFILE

static SDL_Surface *screen = 0;
static u32 palette32[256];
static int flags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_NOFRAME;
static u32 *tmpscreen;
static u32 *oldscreen;
static u8 *nesscreen;
int screenw,screenh,screenscale;

typedef struct video_mode_s {
	int w,h,bpp;
	int scale;
} video_mode_t;

video_mode_t videomodes[] = {
	{ 320, 240,32,1},
	{ 640, 480,32,2},
	{ 800, 600,32,2},
	{1024, 768,32,3},
	{1280,1024,32,4},
	{0,0,0,0}
};

void video_reinit()
{
#ifdef WII
	config.windowed = 0;
	config.fullscreen = 1;
#endif
	if(config.windowed != 0) {
		flags &= ~SDL_FULLSCREEN;
		screenscale = config.windowscale;
		screenw = 256 * screenscale;
		screenh = 240 * screenscale;
	}
	else {
		if((config.fullscreen <= 0) || (config.fullscreen > 4)) {
			log_warning("bad fullscreen video mode, setting to 640x480\n");
			config.fullscreen = 1;
		}
		flags |= SDL_FULLSCREEN;
		screenw = videomodes[config.fullscreen].w;
		screenh = videomodes[config.fullscreen].h;
		screenscale = videomodes[config.fullscreen].scale;
	}
	screen = SDL_SetVideoMode(screenw,screenh,32,flags);
	SDL_WM_SetCaption("nesemu","IDI_MAIN");
	SDL_ShowCursor(0);
}

static nes_ntsc_setup_t setup;
static nes_ntsc_t *ntsc;

#include "nes/ppu/palette.h"

void video_init()
{
	u8 pal[256 * 3];
	int i;

	for(i=0;i<256;i++) {
		pal[i*3 + 0] = palette32[i] >> 0 & 0xFF;
		pal[i*3 + 1] = palette32[i] >> 8 & 0xFF;
		pal[i*3 + 2] = palette32[i] >> 16 & 0xFF;
	}
	video_reinit();
	tmpscreen = (u32*)malloc(256*256*4);
	oldscreen = (u32*)malloc(256*256*4);
	nesscreen = (u8*)malloc(256*256);
	ntsc = (nes_ntsc_t*)malloc(sizeof(nes_ntsc_t));
	setup = nes_ntsc_composite;
	setup.merge_fields = 1;
//	setup.hue = (double)config.hue / 10.0f;
//	setup.saturation = (double)config.sat;
//	setup.sharpness = custom_sharpness;
	nes_ntsc_init(ntsc,&setup);
	hq2x_InitLUTs();
	hq3x_InitLUTs();
}

void video_kill()
{
	hq2x_Kill();
	hq3x_Kill();
	SDL_ShowCursor(1);
	if(tmpscreen)	free(tmpscreen);
	if(oldscreen)	free(oldscreen);
	if(nesscreen)	free(nesscreen);
	if(ntsc)			free(ntsc);
	tmpscreen = 0;
	oldscreen = 0;
	nesscreen = 0;
	ntsc = 0;
}

void video_setpalentry(int i,u8 r,u8 g,u8 b)
{
#ifdef WII
	palette32[i] = palette32[i | 128] = (r << 24) | (g << 16) | (b << 8);
#else
	palette32[i] = palette32[i | 128] = (r << 16) | (g << 8) | (b << 0);
#endif
}

#include "nes/nes.h"

static u32 interval = 1000 / 60;
static u32 lasttime = 0;

//this handles lines coming directly from the nes engine
void video_updateline(int line,u8 *s)
{
#ifndef _PROFILE
	int x,pitch = 256;
	u32 *dest = tmpscreen;
	u8 *dest2 = nesscreen;

	//select line
	dest += pitch * line;
	dest2 += pitch * line;

	//do not show these lines
	if(line < 8 || line >= 232) {
		for(x=0;x<256;x++) {
			dest[x] = 0;
			dest2[x] = nes->pal[0];
		}
		return;
	}

	//copy palette'd pixels
	for(x=0;x<256;x++) {
		u8 pixel = s[x];
		dest2[x] = nes->pal[pixel & 0x1F];
		dest[x] = palette32[pixel];
	}
#endif
}

extern u8 disk_read[24][24];
extern u8 disk_write[24][24];
extern int fdsdrive;

extern int gui_active;

static int fps;

static void draw1x(u32 *dest,int destp,u32 *src,int srcp,int w,int h)
{
	int x,y;

	for(y=0;y<h;y++) {
		for(x=0;x<w;x++)
			dest[x + (y * destp)] = src[x + (y * srcp)];
	}
}

static void draw2x(u32 *dest,int destp,u32 *src,int srcp,int w,int h)
{
	int x,y;
	u32 *dest1,*dest2,pixel32;

	for(y=0;y<h;y++) {
		dest1 = dest;
		dest2 = dest + destp;
		for(x=0;x<w;x++) {
			pixel32 = src[x];
			*dest1++ = pixel32;
			*dest1++ = pixel32;
			*dest2++ = pixel32;
			*dest2++ = pixel32;
		}
		src += srcp;
		dest += destp * 2;
	}
}

static void draw3x(u32 *dest,int destp,u32 *src,int srcp,int w,int h)
{
	int x,y;

	for(y=0;y<h;y++) {
		for(x=0;x<w;x++) {
			u32 pixel = src[x + (y * srcp)];

			dest[x*3+0 + ((y*3+0) * destp)] = pixel;
			dest[x*3+1 + ((y*3+0) * destp)] = pixel;
			dest[x*3+2 + ((y*3+0) * destp)] = pixel;
			dest[x*3+0 + ((y*3+1) * destp)] = pixel;
			dest[x*3+1 + ((y*3+1) * destp)] = pixel;
			dest[x*3+2 + ((y*3+1) * destp)] = pixel;
			dest[x*3+0 + ((y*3+2) * destp)] = pixel;
			dest[x*3+1 + ((y*3+2) * destp)] = pixel;
			dest[x*3+2 + ((y*3+2) * destp)] = pixel;
		}
	}
}

static void draw4x(u32 *dest,int destp,u32 *src,int srcp,int w,int h)
{
	int x,y;

	for(y=0;y<h;y++) {
		for(x=0;x<w;x++) {
			u32 pixel = src[x + (y * srcp)];

			dest[x*4+0 + ((y*4+0) * destp)] = pixel;
			dest[x*4+1 + ((y*4+0) * destp)] = pixel;
			dest[x*4+2 + ((y*4+0) * destp)] = pixel;
			dest[x*4+3 + ((y*4+0) * destp)] = pixel;
			dest[x*4+0 + ((y*4+1) * destp)] = pixel;
			dest[x*4+1 + ((y*4+1) * destp)] = pixel;
			dest[x*4+2 + ((y*4+1) * destp)] = pixel;
			dest[x*4+3 + ((y*4+1) * destp)] = pixel;
			dest[x*4+0 + ((y*4+2) * destp)] = pixel;
			dest[x*4+1 + ((y*4+2) * destp)] = pixel;
			dest[x*4+2 + ((y*4+2) * destp)] = pixel;
			dest[x*4+3 + ((y*4+2) * destp)] = pixel;
			dest[x*4+0 + ((y*4+3) * destp)] = pixel;
			dest[x*4+1 + ((y*4+3) * destp)] = pixel;
			dest[x*4+2 + ((y*4+3) * destp)] = pixel;
			dest[x*4+3 + ((y*4+3) * destp)] = pixel;
		}
	}
}

void ntsc2x(u32 *dest,int destp,u32 *src,int srcp,int w,int h)
{
	int x,y;

	if(gui_active) {
		interpolate2x(dest,destp,src,srcp,w,h);
		return;
	}
	nes_ntsc_blit(ntsc,nesscreen+(8*256),256,0,256,224,dest,destp*4);
	for(y=224*2/2;--y>=0;) {
		u32 *in = dest + y * destp;
		u32 *out = dest + y * 2 * destp;

		for(x=0;x<256*2;x++) {
			u32 prev = *in;
			u32 next = *(in + destp);

			*out = prev;
			*(out + destp) = prev;
			in++;
			out++;
		}
	}
}

void ntsc3x(u32 *dest,int destp,u32 *src,int srcp,int w,int h)
{
	if(gui_active) {
		interpolate3x(dest,destp,src,srcp,w,h);
		return;
	}
	nes_ntsc_blit(ntsc,nesscreen,256,0,256,240,dest,destp*4);
}

void video_endframe()
{
	u32 *scr,t;
	int pitch;
	static int fastforward = 0;
	static u32 frames = 0;
	static u32 time1 = 0,time2;
	static char fpsstr[256] = "";

	SDL_LockSurface(screen);

	scr = (u32*)screen->pixels;
	pitch = screen->pitch / 4;

	frames++;
	if(config.showinfo) {
		time2 = SDL_GetTicks();
		if((t = time2 - time1) >= 1000) {
			fps = frames * 1000 / t;
			frames = 0;
			time1 = time2;
			sprintf(fpsstr,"%d fps",fps);
		}
		if(gui_active == 0) {
			if(nes && nes->rom) {
				char n[40];

				strncpy(n,nes->rom->name,33);
				if(strlen(nes->rom->name) > 33) {
					n[32] = n[33] = '.'; n[34] = 0;
				}
				gui_draw_text32(tmpscreen,256,GUI_TEXT,1,1,n);
			}
			gui_draw_text32(tmpscreen,256,GUI_TEXT,256-43,1,fpsstr);
		}
	}

	if((flags & SDL_FULLSCREEN)) {
		scr += (screenw - (256 * screenscale)) / 2;
		switch(config.fullscreen) {
			case 0:	//320x240
			case 1:	//640x480
			case 4:	//1280x1024
				break;
			case 2:	//800x600
			case 3:	//1024x768
				scr += 8 * screen->pitch;
				break;
		}
	}
	if(screenscale == 4) {
		draw4x(scr,screen->pitch / 4,tmpscreen,256,256,240);
	}
	else if(screenscale == 3) {
		if(config.filter == 0)
			draw3x(scr,screen->pitch / 4,tmpscreen,256,256,240);
		else if(config.filter == 1)
			scale(3,scr,screen->pitch,tmpscreen,256*4,4,256,240);
		else if(config.filter == 2)
			interpolate3x(scr,screen->pitch / 4,tmpscreen,256,256,240);
		else if(config.filter == 3)
		{}
		else
			ntsc3x(scr,screen->pitch / 4,tmpscreen,256,256,240);
	}
	else if(screenscale == 2) {
		if(config.filter == 0)
			draw2x(scr,screen->pitch / 4,tmpscreen,256,256,240);
		else if(config.filter == 1)
			scale(2,scr,screen->pitch,tmpscreen,256*4,4,256,240);
		else if(config.filter == 2)
//			hq2x_32((u8*)tmpscreen,(u8*)scr,256,240,256);
			interpolate2x(scr,screen->pitch / 4,tmpscreen,256,256,240);
		else
			ntsc2x(scr,screen->pitch / 4,tmpscreen,256,256,240);
	}
	else if(screenscale == 1)
		draw1x(scr,screen->pitch/4,tmpscreen,256,256,240);

	if(nes && nes->rom && nes->rom->mapper == 20 && fdsdrive) {
		//draw fds drive read/write icon...
		u32 *scr = (u32*)screen->pixels;
		int x,y,pitch = screen->pitch / 4;

		scr += (640 - (256 * 2)) / 2;
		scr += (8 * pitch) + 8;
		for(y=0;y<24;y++) {
			for(x=0;x<24;x++) {
				u8 p;

				if(fdsdrive & 2)
					p = disk_write[y][x];
				else
					p = disk_read[y][x];
				if(p)
					scr[x] = palette32[p];
			}
			scr += pitch;
		}
		fdsdrive = 0;
	}
	if(joykeys[config.gui_keys[5]] && fastforward-- > 0)
		return;
	else
		fastforward = 10;
	SDL_Flip(screen);
	SDL_UnlockSurface(screen);
#ifndef _PROFILE
	t = SDL_GetTicks();
	while((t - lasttime) < interval) {
#ifdef WIN32
		Sleep(interval - (t - lasttime) + 0);
#endif
		t = SDL_GetTicks();
	}
	lasttime = t;
#endif
}

void video_update(u8 *s,int spitch)
{
	int x,y;
	u32 *dest = tmpscreen;
	u8 *dest2 = nesscreen;

	for(y=0;y<240;y++) {
		for(x=0;x<256;x++) {
			u32 pixel = palette32[s[x]];

			if(s[x] == 0) {
				pixel = oldscreen[x + y * 256];
				pixel = (pixel & 0xFF) + (pixel >> 8 & 0xFF) + (pixel >> 16 & 0xFF);
				pixel /= 5;
				pixel = (pixel << 8) | pixel;
			}
			dest[x] = pixel;
			dest2[x] = s[x];
		}
		s += spitch;
		dest += 256;
		dest2 += 256;
	}
	video_endframe();
}

void video_copyscreen()
{
	int x,y;

	for(y=0;y<240;y++) {
		for(x=0;x<256;x++)
			oldscreen[x + y * 256] = tmpscreen[x + y * 256];
	}
}

u32 video_getpixel(int x,int y)
{
	return(tmpscreen[x + y * 256]);
}

u32 video_getpalette(int idx)
{
	return(palette32[idx]);
}

u32 *video_getscreen()
{
	return(tmpscreen);
}

u8 *video_getnesscreen()
{
	return(nesscreen);
}

void video_togglefullscreen()
{
	config.windowed ^= 1;
	video_reinit();
}

void video_minimize()
{
	SDL_WM_IconifyWindow();
}

#ifdef WIN32
void video_setwindowpos(int x,int y)
{
	SDL_SysWMinfo info;
	HWND hwnd;

	SDL_VERSION(&info.version);
	if(SDL_GetWMInfo(&info) == 0)
		return;
	hwnd = info.window;
	SetWindowPos(hwnd,0,x,y,0,0,SWP_NOSIZE);
}

void video_getwindowpos(int *x,int *y)
{
	SDL_SysWMinfo info;
	HWND hwnd;
	RECT r;

	SDL_VERSION(&info.version);
	if(SDL_GetWMInfo(&info) == 0)
		return;
	hwnd = info.window;
	GetWindowRect(hwnd,&r);
	*x = r.left;
	*y = r.top;
}
#elif defined(LINUX)
void video_setwindowpos(int x,int y)
{
/*	SDL_SysWMinfo info;
	XWindowAttributes attr;

	SDL_VERSION(&info.version);
	if(SDL_GetWMInfo(&info) == 0)
		return;
	info.info.x11.lock_func();
	XGetWindowAttributes(info.info.x11.display,info.info.x11.wmwindow,&attr); 
	XMoveWindow(info.info.x11.display,info.info.x11.wmwindow,x,y);
	info.info.x11.unlock_func();*/
}

void video_getwindowpos(int *x,int *y)
{
/*	SDL_SysWMinfo info;
	XWindowAttributes attr;

	SDL_VERSION(&info.version);
	if(SDL_GetWMInfo(&info) == 0)
		return;
	info.info.x11.lock_func();
	XGetWindowAttributes(info.info.x11.display,info.info.x11.wmwindow,&attr); 
//	XMoveWindow(info.info.x11.display,info.info.x11.wmwindow,x,y);
	info.info.x11.unlock_func();
	*x = attr.x;
	*y = attr.y;*/
}
#endif
///TODO: write these for osx/linux
