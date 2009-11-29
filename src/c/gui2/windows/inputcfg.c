#include <stdio.h>
#include <string.h>
#include "gui2/windows/inputcfg.h"

/*

gui2 input object

-------------------

contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void input_draw(input_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Input Configuration");
	button_draw(&m->donebtn);
	for(x=0;x<16;x++) {
		text_draw(&m->key_label[x]);
		edit_draw(&m->key_edit[x]);
		button_draw(&m->key_button[x]);
		text_draw(&m->key_label2[x]);
		edit_draw(&m->key_edit2[x]);
		button_draw(&m->key_button2[x]);
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
				config.joy_keys[0][m->needinput - 1] = i;
				sprintf(m->key_edit[m->needinput - 1].text,"%d",i);
				m->needinput = 0;
				break;
			}
		}
	}
}

int input_event(input_t *m,int event,int data)
{
	int x;

	if(m->isshowing == 0)
		return(0);
	//reading input
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
				edit_event(&m->key_edit2[x],event,data);
				button_event(&m->key_button2[x],event,data);
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

static void click_key0(void *u)	{	((input_t*)u)->needinput = 1;		}
static void click_key1(void *u)	{	((input_t*)u)->needinput = 2;		}
static void click_key2(void *u)	{	((input_t*)u)->needinput = 3;		}
static void click_key3(void *u)	{	((input_t*)u)->needinput = 4;		}
static void click_key4(void *u)	{	((input_t*)u)->needinput = 5;		}
static void click_key5(void *u)	{	((input_t*)u)->needinput = 6;		}
static void click_key6(void *u)	{	((input_t*)u)->needinput = 7;		}
static void click_key7(void *u)	{	((input_t*)u)->needinput = 8;		}

static void click_key20(void *u)	{	((input_t*)u)->needinput = 9;		}
static void click_key21(void *u)	{	((input_t*)u)->needinput = 10;	}
static void click_key22(void *u)	{	((input_t*)u)->needinput = 11;	}
static void click_key23(void *u)	{	((input_t*)u)->needinput = 12;	}
static void click_key24(void *u)	{	((input_t*)u)->needinput = 13;	}
static void click_key25(void *u)	{	((input_t*)u)->needinput = 14;	}
static void click_key26(void *u)	{	((input_t*)u)->needinput = 15;	}
static void click_key27(void *u)	{	((input_t*)u)->needinput = 16;	}

void input_create(input_t *m)
{
	int i,x,y;
	char *btns[8] = {"A","B","Select","Start","Up","Down","Left","Right"};

	memset(m,0,sizeof(input_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 15;
	y = m->info.y = 40;
	m->info.w = 206;
	m->info.h = 132;
	m->info.draw = (drawfunc_t)input_draw;
	m->info.event = (eventfunc_t)input_event;

	button_create(&m->donebtn,"Done",x+m->info.w-(6*6),160,0);

	x += 3;
	y += 2 + 11;

	text_create(&m->key_label[12],x,y,"Player 1");
	text_create(&m->key_label[13],x+105,y,"Player 2");

	y += 10;

	for(i=0;i<8;i++) {
		text_create(&m->key_label[i],x,y+12*i+2,btns[i]);
		edit_create(&m->key_edit[i],x+6*7,y+12*i,21,10,0);
		sprintf(m->key_edit[i].text,"%d",config.joy_keys[0][i]);

		text_create(&m->key_label2[i],x+105,y+12*i+2,btns[i]);
		edit_create(&m->key_edit2[i],x+6*7+105,y+12*i,21,10,0);
		sprintf(m->key_edit2[i].text,"%d",config.joy_keys[1][i]);
	}
	x += 20 + 6 + 6 * 7;
	button_create(&m->key_button[0],"Set",x,y+12*0,click_key0);
	button_create(&m->key_button[1],"Set",x,y+12*1,click_key1);
	button_create(&m->key_button[2],"Set",x,y+12*2,click_key2);
	button_create(&m->key_button[3],"Set",x,y+12*3,click_key3);
	button_create(&m->key_button[4],"Set",x,y+12*4,click_key4);
	button_create(&m->key_button[5],"Set",x,y+12*5,click_key5);
	button_create(&m->key_button[6],"Set",x,y+12*6,click_key6);
	button_create(&m->key_button[7],"Set",x,y+12*7,click_key7);

	x += 105;
	button_create(&m->key_button2[0],"Set",x,y+12*0,click_key20);
	button_create(&m->key_button2[1],"Set",x,y+12*1,click_key21);
	button_create(&m->key_button2[2],"Set",x,y+12*2,click_key22);
	button_create(&m->key_button2[3],"Set",x,y+12*3,click_key23);
	button_create(&m->key_button2[4],"Set",x,y+12*4,click_key24);
	button_create(&m->key_button2[5],"Set",x,y+12*5,click_key25);
	button_create(&m->key_button2[6],"Set",x,y+12*6,click_key26);
	button_create(&m->key_button2[7],"Set",x,y+12*7,click_key27);
	for(x=0;x<8;x++) {
		m->key_button[x].user = m;
		m->key_button2[x].user = m;
	}
}
