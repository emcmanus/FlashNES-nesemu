#include <stdio.h>
#include <string.h>
#include "gui2/windows/supported_mappers.h"
#include "nesemu.h"

/*

gui2 supported_mappers object

-------------------


contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void supported_mappers_draw(supported_mappers_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Supported Mappers");
	button_draw(&m->donebtn);
	list_draw(&m->ines_list);
	list_draw(&m->unif_list);
}

int supported_mappers_event(supported_mappers_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			list_event(&m->ines_list,event,data);
			list_event(&m->unif_list,event,data);
			break;
		case E_REFRESH:
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

static char *refresh_ines(list_t *l,int m)
{
	static int curmapper;
	static char ret[32];
	mapper_ines_t *map;

	//begin refresh
	if(m == 1) {
		curmapper = 0;
		//success
		return(0);
	}
	//get next item
	else if(m == 0) {
		do {
			map = mapper_init_ines(curmapper++);
			if(curmapper > 256)
				return(0);
		} while(map == 0);
		sprintf(ret,"%d",curmapper - 1);
		return(ret);
	}
	else
		strcpy(ret,"???");
	return(ret);
}

static char *refresh_unif(list_t *l,int m)
{
	static int curmapper;
	static char ret[32];
	mapper_unif_t *map;

	//begin refresh
	if(m == 1) {
		//success
		return(0);
	}
	//get next item
	else if(m == 0) {
		if((map = mapper_init_unif(0)) == 0)
			return(0);
		return(map->board);
	}
	else
		strcpy(ret,"???");
	return(ret);
}

void supported_mappers_create(supported_mappers_t *m)
{
	int x,y;
	u8 dip = 0;

	memset(m,0,sizeof(supported_mappers_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 20;
	y = m->info.y = 40;
	m->info.w = 200;
	m->info.h = 180;
	m->info.draw = (drawfunc_t)supported_mappers_draw;
	m->info.event = (eventfunc_t)supported_mappers_event;

	button_create(&m->donebtn,"X",x+200-10,y,0);

	x += 3;
	y += 2 + 11;

	list_create(&m->ines_list,x,y,40,160,refresh_ines);
	list_create(&m->unif_list,x+50,y,140,160,refresh_unif);

	m->ines_list.user = m;
	m->unif_list.user = m;

	//tell lists to refresh
	list_event(&m->ines_list,E_REFRESH,0);
	list_event(&m->unif_list,E_REFRESH,0);
}
