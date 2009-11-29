#include <stdio.h>
#include <string.h>
#include "gui2/windows/gui_input.h"

/*

gui2 gui_input object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void gui_input_draw(gui_input_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"GUI Input Config");
	button_draw(&m->donebtn);
	for(x=0;x<16;x++) {
		text_draw(&m->key_label[x]);
		edit_draw(&m->key_edit[x]);
		button_draw(&m->key_button[x]);
	}
	if(m->needinput) {
		int i,x,y;

		x = 92;
		y = 70;
		gui_draw_border(GUI_COLOR_DARKBLUE,x,y,90,40);
		gui_draw_border(GUI_COLOR_GREY,x,y,90,9);
		gui_draw_text(GUI_TEXT,x+2,y+2,"Press New Key");
		for(i=0;i<370;i++) {
			if(joykeys[i]) {
				config.gui_keys[m->needinput - 1] = i;
				sprintf(m->key_edit[m->needinput - 1].text,"%d",i);
				m->needinput = 0;
				break;
			}
		}
	}
}

int gui_input_event(gui_input_t *m,int event,int data)
{
	int x;

	if(m->isshowing == 0)
		return(0);
	//reading gui_input
	if(m->needinput) {
		return(1);
	}
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			for(x=0;x<16;x++) {
				edit_event(&m->key_edit[x],event,data);
				button_event(&m->key_button[x],event,data);
			}
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			else {
				for(x=0;x<16;x++) {
					edit_event(&m->key_edit[x],event,data);
					button_event(&m->key_button[x],event,data);
				}
			}
			break;
	}
	return(0);
}

static void click_key0(void *u)	{	((gui_input_t*)u)->needinput = 1;		}
static void click_key1(void *u)	{	((gui_input_t*)u)->needinput = 2;		}
static void click_key2(void *u)	{	((gui_input_t*)u)->needinput = 3;		}
static void click_key3(void *u)	{	((gui_input_t*)u)->needinput = 4;		}
static void click_key4(void *u)	{	((gui_input_t*)u)->needinput = 5;		}
static void click_key5(void *u)	{	((gui_input_t*)u)->needinput = 6;		}
static void click_key6(void *u)	{	((gui_input_t*)u)->needinput = 7;		}
static void click_key7(void *u)	{	((gui_input_t*)u)->needinput = 8;		}

static void click_key20(void *u)	{	((gui_input_t*)u)->needinput = 9;		}
static void click_key21(void *u)	{	((gui_input_t*)u)->needinput = 10;	}
static void click_key22(void *u)	{	((gui_input_t*)u)->needinput = 11;	}
static void click_key23(void *u)	{	((gui_input_t*)u)->needinput = 12;	}
static void click_key24(void *u)	{	((gui_input_t*)u)->needinput = 13;	}
static void click_key25(void *u)	{	((gui_input_t*)u)->needinput = 14;	}
static void click_key26(void *u)	{	((gui_input_t*)u)->needinput = 15;	}
static void click_key27(void *u)	{	((gui_input_t*)u)->needinput = 16;	}

void gui_input_create(gui_input_t *m)
{
	int i,x,y;
	char *btns[8] = {"Menu","Load State","Save State","Disk Flip","Screenshot","Fast Forward"};

	memset(m,0,sizeof(gui_input_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 65;
	y = m->info.y = 40;
	m->info.w = 130;
	m->info.h = 132;
	m->info.draw = (drawfunc_t)gui_input_draw;
	m->info.event = (eventfunc_t)gui_input_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),157,0);

	x += 3;
	y += 2 + 11;

	for(i=0;i<6;i++) {
		text_create(&m->key_label[i],x,y+12*i+2,btns[i]);
		edit_create(&m->key_edit[i],x+11*7-3,y+12*i,21,10,0);
		sprintf(m->key_edit[i].text,"%d",config.gui_keys[i]);
	}
	x += 20 + 3 + 11 * 7;
	button_create(&m->key_button[0],"Set",x,y+12*0,click_key0);
	button_create(&m->key_button[1],"Set",x,y+12*1,click_key1);
	button_create(&m->key_button[2],"Set",x,y+12*2,click_key2);
	button_create(&m->key_button[3],"Set",x,y+12*3,click_key3);
	button_create(&m->key_button[4],"Set",x,y+12*4,click_key4);
	button_create(&m->key_button[5],"Set",x,y+12*5,click_key5);

	for(x=0;x<8;x++)
		m->key_button[x].user = m;
}
