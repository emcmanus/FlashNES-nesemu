#include <stdio.h>
#include <string.h>
#include "gui2/windows/soundcfg.h"

/*

gui2 sound object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void sound_draw(sound_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Sound Configuration");
	button_draw(&m->donebtn);
	checkbox_draw(&m->enabled);
	checkbox_draw(&m->sq1);
	checkbox_draw(&m->sq2);
	checkbox_draw(&m->tri);
	checkbox_draw(&m->noise);
	checkbox_draw(&m->dmc);
	checkbox_draw(&m->ext);
	text_draw(&m->tc);
}

int sound_event(sound_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
			break;
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			checkbox_event(&m->enabled,event,data);
			checkbox_event(&m->sq1,event,data);
			checkbox_event(&m->sq2,event,data);
			checkbox_event(&m->tri,event,data);
			checkbox_event(&m->noise,event,data);
			checkbox_event(&m->dmc,event,data);
			checkbox_event(&m->ext,event,data);
			text_event(&m->tc,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0)
				m->isshowing = 0;
			break;
	}
	return(0);
}

static void click_soundenabled(void *u,int s)
{
	config.soundenabled = s;
	if(config.soundenabled == 0)
		apu_disable();
	else
		apu_enable();
	if(nes) {
		nes->apu->mix_enable = s ? config.soundchannels : 0;
		apu_setcontext(nes->apu);
	}
}

static void toggle_channel(int c)
{
	config.soundchannels ^= c;
	if(nes) {
		nes->apu->mix_enable = config.soundenabled ? config.soundchannels : 0;
		apu_setcontext(nes->apu);
	}
}

static void click_sq1(void *u,int s)	{	toggle_channel(s << 0);	}
static void click_sq2(void *u,int s)	{	toggle_channel(s << 1);	}
static void click_tri(void *u,int s)	{	toggle_channel(s << 2);	}
static void click_noise(void *u,int s)	{	toggle_channel(s << 3);	}
static void click_dmc(void *u,int s)	{	toggle_channel(s << 4);	}
static void click_ext(void *u,int s)	{	toggle_channel(s << 5);	}

void sound_create(sound_t *m)
{
	int x,y;
	u8 channels = config.soundchannels;

	memset(m,0,sizeof(sound_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 50;
	y = m->info.y = 50;
	m->info.w = 150;
	m->info.h = 93;
	m->info.draw = (drawfunc_t)sound_draw;
	m->info.event = (eventfunc_t)sound_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),y + m->info.h - 15,0);

	x += 3;
	y += 3;

	checkbox_create(&m->enabled,x+2,y+12,"Enabled",config.soundenabled,click_soundenabled);

	y += 8 + 5;

	text_create(&m->tc,x+2,y+12+8*0,"Channels enabled:");
	x += 5;
	checkbox_create(&m->sq1,x+2,y+12+9*1,"Square 1",(channels & 1) ? 1 : 0,click_sq1);
	checkbox_create(&m->sq2,x+2,y+12+9*2,"Square 2",(channels & 2) ? 1 : 0,click_sq2);
	checkbox_create(&m->tri,x+2,y+12+9*3,"Triangle",(channels & 4) ? 1 : 0,click_tri);
	checkbox_create(&m->noise,x+2,y+12+9*4,"Noise",(channels & 8) ? 1 : 0,click_noise);
	checkbox_create(&m->dmc,x+2,y+12+9*5,"DMC",(channels & 0x10) ? 1 : 0,click_dmc);
	checkbox_create(&m->ext,x+2,y+12+9*6,"External",(channels & 0x20) ? 1 : 0,click_ext);

	m->enabled.user = m;
	m->sq1.user = m;
	m->sq2.user = m;
	m->tri.user = m;
	m->noise.user = m;
	m->dmc.user = m;
	m->ext.user = m;

	x += 3;
	y += 2 + 11;
}

