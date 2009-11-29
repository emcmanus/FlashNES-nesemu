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

#include "defines.h"
#include "system/sound.h"
#include "sjpcm.h"

static u16 soundbuffer[48000];
static int soundbuflen = 48000 / 60;
static void (*audio_callback)(void *buffer, int length) = 0;

static void null_callback(void *buffer,int length)
{}

void sound_init()
{
	audio_callback = null_callback;
	SjPCM_Init(0);
	SjPCM_Clearbuff();
	SjPCM_Setvol(0x3FFF);
	SjPCM_Play();
}

void sound_kill()
{
	SjPCM_Quit();
}

void sound_play()
{
	SjPCM_Play();
}

void sound_pause()
{
	SjPCM_Pause();
}

void sound_setcallback(void (*cb)(void *buffer, int length))
{
	audio_callback = cb;
}

void sound_update()
{
	audio_callback((void*)soundbuffer,soundbuflen);
	SjPCM_Enqueue(soundbuffer,soundbuffer,soundbuflen,1);
}
