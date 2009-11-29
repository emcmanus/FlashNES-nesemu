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

#include <SDL/SDL.h>
#include "defines.h"
#include "system/system.h"
#include "nesemu.h"
#include "gui2/gui2.h"

SDL_Joystick *joystick = 0;

void init_system()
{
	int i;

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		printf("error at sdl init!\n");
		exit(0);
	}
	if(SDL_NumJoysticks() > 0) {
		SDL_JoystickEventState(SDL_ENABLE);
		joystick = SDL_JoystickOpen(0);
	}
}

void kill_system()
{
	if(joystick)
		SDL_JoystickClose(joystick);
	SDL_QuitSubSystem(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
}

//from system/sdl/video.c
extern int screenw,screenscale;

void check_events()
{
	SDL_Event event; /* Event structure */
	mousemove_u mm;
	mousebutton_u mb;
	int x,y,xoff,yoff,d;
	static u32 downtime,lastdowntime = 0;

	if(config.windowed > 0) {
		xoff = 0;
		yoff = 0;
	}
	else {
		xoff = (screenw - (256 * screenscale)) / 2;
		yoff = 0;
		if(screenw == 800)
			yoff = 11 * 3;
		else if(screenw == 1024)
			yoff = 11 * 3;
	}
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				quit++;
				break;
			case SDL_MOUSEMOTION:
				if((event.motion.x - xoff) < 0) {
					x = 0;
					mm.info.xrel = 0;
				}
				else {
					x = (event.motion.x - xoff) / screenscale;
					mm.info.xrel = (s8)event.motion.xrel;
				}
				if((event.motion.y - yoff) < 0) {
					y = 0;
					mm.info.yrel = 0;
				}
				else {
					y = (event.motion.y - yoff) / screenscale;
					mm.info.yrel = (s8)event.motion.yrel;
				}
				if(x >= 256)
					x = 255;
				if(y >= 256)
					y = 255;
				mm.info.x = (u8)x;
				mm.info.y = (u8)y;
				//update zapper input
				joyx = mm.info.x;
				joyy = mm.info.y;
				gui2_event(E_MOUSEMOVE,mm.data);
				break;
			case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
				if (event.jaxis.axis == 0)
				{
					joystate[0] = 0;
					joystate[1] = 0;
					if (event.jaxis.value < -3200)
						joystate[0] = 1;
					else if (event.jaxis.value > 3200)
						joystate[1] = 1;
				}
				else if (event.jaxis.axis == 1)
				{
					joystate[2] = 0;
					joystate[3] = 0;
					if (event.jaxis.value < -3200)
						joystate[2] = 1;
					else if (event.jaxis.value > 3200)
						joystate[3] = 1;
				}
				break;
			case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
				joystate[event.jbutton.button + 4] = 1;
				break;
			case SDL_JOYBUTTONUP:  /* Handle Joystick Button Releases */
				joystate[event.jbutton.button + 4] = 0;
				break;
			case SDL_MOUSEBUTTONDOWN:
				downtime = SDL_GetTicks();
				//double click?
				if((downtime - lastdowntime) < 250)
					d = 1;
				else {
					d = 0;
					lastdowntime = downtime;
				}
			case SDL_MOUSEBUTTONUP:
				mb.info.x = (event.button.x - xoff) / screenscale;
				mb.info.y = (event.button.y - yoff) / screenscale;
				switch(event.button.button) {
					default: mb.info.btn = 0; break;
					case SDL_BUTTON_LEFT: mb.info.btn = 1; break;
					case SDL_BUTTON_RIGHT: mb.info.btn = 2; break;
					case SDL_BUTTON_WHEELUP: mb.info.btn = 3; break;
					case SDL_BUTTON_WHEELDOWN: mb.info.btn = 4; break;
				}
				gui2_event((event.button.type == SDL_MOUSEBUTTONDOWN) ? (d ? E_MOUSEDOWN2 : E_MOUSEDOWN) : E_MOUSEUP,mb.data);
				break;
		}
	}
}
