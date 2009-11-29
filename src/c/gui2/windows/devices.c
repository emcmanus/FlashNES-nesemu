#include <stdio.h>
#include <string.h>
#include "gui2/windows/devices.h"

/*

gui2 devices object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void devices_draw(devices_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Devices Configuration");
	button_draw(&m->donebtn);
	radiogroup_draw(&m->p[0]);
	radiogroup_draw(&m->p[1]);
	radiogroup_draw(&m->p[2]);
	radiogroup_draw(&m->p[3]);
	radiogroup_draw(&m->exp);
}

int devices_event(devices_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			radiogroup_event(&m->p[0],event,data);
			radiogroup_event(&m->p[1],event,data);
			radiogroup_event(&m->p[2],event,data);
			radiogroup_event(&m->p[3],event,data);
			radiogroup_event(&m->exp,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0)
				m->isshowing = 0;
			break;
	}
	return(0);
}

static void changed_p0(void *u,int s)
{
	config.devices[0] = s;
	nes_setinput(0,s);
}

static void changed_p1(void *u,int s)
{
	config.devices[1] = s;
	nes_setinput(1,s);
}

static void changed_p2(void *u,int s)
{
	//unplugged
	if(s == 0) {
		config.devices[2] = s;
		//put in one joypad for port 1
		nes_setinput(0,1);
	}
	//player 3 plugged in joypad
	else {
		config.devices[0] = s;
		config.devices[2] = s;
		//put in two joypads for port 1
		nes_setinput(0,5);
	}
}

static void changed_p3(void *u,int s)
{
	config.devices[3] = s;
}

static void changed_exp(void *u,int s)
{
	config.expdevice = s;
	nes_setexp(s);
}

static char *p01_caps[] = {"None","Joypad","Zapper","Powerpad","Arkanoid",0};
static char *p23_caps[] = {"None","Joypad",0};
static char *exp_caps[] = {
	"None","Fami 4Player","Fami Arkanoid",
	"Basic Keyboard","Study Keyboard",
	"Barcode Reader",0
};

void devices_create(devices_t *m)
{
	int x,y;

	memset(m,0,sizeof(devices_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 6;
	y = m->info.y = 45;
	m->info.w = 244;
	m->info.h = 93;
	m->info.draw = (drawfunc_t)devices_draw;
	m->info.event = (eventfunc_t)devices_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),130-6,0);
	x += 4;
	y += 12;
	radiogroup_create(&m->p[0],x,y,"Player 1:",config.devices[0],p01_caps,changed_p0);
	radiogroup_create(&m->p[1],x,y+8*7,"Player 3:",config.devices[2],p23_caps,changed_p2);
	x += 6 * 12;
	radiogroup_create(&m->p[2],x,y,"Player 2:",config.devices[1],p01_caps,changed_p1);
	radiogroup_create(&m->p[3],x,y+8*7,"Player 4:",config.devices[3],p23_caps,changed_p3);
	x += 6 * 12;
	radiogroup_create(&m->exp,x,y,"Expansion:",config.expdevice,exp_caps,changed_exp);
}
