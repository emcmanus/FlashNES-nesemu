#include <stdio.h>
#include <string.h>
#include "gui2/windows/videocfg.h"

/*

gui2 video object

-------------------


contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void video_draw(video_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Video Configuration");
	button_draw(&m->donebtn);
	radiogroup_draw(&m->filter);
	radiogroup_draw(&m->videomode);
}

int video_event(video_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			radiogroup_event(&m->filter,event,data);
			radiogroup_event(&m->videomode,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			break;
	}
	return(0);
}

static void changed_videomode(void *u,int s)
{
	//windowed modes
	if(s <= 3) {
		config.windowed = 1;
		config.windowscale = s + 1;
	}
	else {
		config.windowed = 0;
		config.fullscreen = s - 4;

	}
	video_reinit();
}

static void changed_filter(void *u,int s)
{
	config.filter = s;
}

static char *filter_caps[] = {
	"No Filter",
	"Scale",
	"Interpolate",
//	"NTSC",
//	"HQ2X",
	0
};

static char *videomode_caps[] = {
	"256x240 Windowed",
	"512x480 Windowed",
	"768x720 Windowed",
	"1024x960 Windowed",
	"320x240 Fullscreen",
	"640x480 Fullscreen",
	"800x600 Fullscreen",
	"1024x768 Fullscreen",
	"1280x1024 Fullscreen",
	0
};

void video_create(video_t *m)
{
	int i,x,y;

	memset(m,0,sizeof(video_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 15;
	y = m->info.y = 40;
	m->info.w = 220;
	m->info.h = 110;
	m->info.draw = (drawfunc_t)video_draw;
	m->info.event = (eventfunc_t)video_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6)+2,136,0);

	radiogroup_create(&m->filter,x+2,y+12,"Video Filter",config.filter,filter_caps,changed_filter);

	if(config.windowed == 0)
		i = config.fullscreen + 4;
	else
		i = config.windowscale - 1;
	radiogroup_create(&m->videomode,x+2+84,y+12,"Video Mode",i,videomode_caps,changed_videomode);

	m->filter.user = m;

	x += 3;
	y += 2 + 11;
}
