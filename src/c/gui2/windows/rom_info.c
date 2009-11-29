#include <stdio.h>
#include <string.h>
#include "gui2/windows/rom_info.h"
#include "nesemu.h"

/*

gui2 rom_info object

-------------------


contains the dropdown menu 'buttons' across the top
of the screen, located under the titlebar.

*/

void rom_info_draw(rom_info_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"ROM Info");
	button_draw(&m->donebtn);
	for(x=0;x<20;x++) {
		if(m->labels[x].info.w)
			text_draw(&m->labels[x]);
	}
}

int rom_info_event(rom_info_t *m,int event,int data)
{
	int i;

	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->donebtn,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->donebtn,event,data) > 0) {
				m->isshowing = 0;
				config_save();
			}
			break;
		case E_REFRESH:
			for(i=0;i<20;i++)
				strcpy(m->labels[i].text,"");
			if((nes == 0) || (nes->rom == 0)) {
				strcpy(m->labels[0].text,"No ROM loaded.");
				return(0);
			}

			//the rom is in our database
			if(nes->rom->indatabase == 1) {
				strcpy(m->labels[0].text,"File:");
				strcpy(m->labels[1].text,"Name:");
				strcpy(m->labels[2].text,"PRG Size:");
				strcpy(m->labels[3].text,"CHR Size:");
				strcpy(m->labels[4].text,"Board:");
				strcpy(m->labels[5].text,"Mirroring:");
				strncpy(m->labels[10].text,nes->rom->filename,30);
				strncpy(m->labels[11].text,nes->rom->name,30);
				sprintf(m->labels[12].text,"%d kb",nes->rom->prgsize / 1024);
				sprintf(m->labels[13].text,"%d kb",nes->rom->chrsize / 1024);
				sprintf(m->labels[14].text,"%s",nes->rom->board);
				switch(nes->rom->mirroring) {
					case 0:sprintf(m->labels[15].text,"Horizontal");break;
					case 1:sprintf(m->labels[15].text,"Vertical");break;
					default:sprintf(m->labels[15].text,"Four Screen");break;
				}
			}

			//display nsf info
			else if(nes->rom->mapper == NSF_MAPPER) {
				strcpy(m->labels[0].text,"File:");
				strcpy(m->labels[1].text,"Name:");
				strcpy(m->labels[2].text,"PRG Size:");
			}

			//display fds info
			//TODO: fancy feature: browse/view the files on the disk
			//TODO: contain extended information about these in the romdb
			else if(nes->rom->mapper == 20) {
				strcpy(m->labels[0].text,"File:");
				strcpy(m->labels[1].text,"Name:");
				strcpy(m->labels[2].text,"Size:");

				strncpy(m->labels[10].text,nes->rom->filename,30);
				strncpy(m->labels[11].text,nes->rom->indatabase==2?nes->rom->name:"<not in database>",30);
				sprintf(m->labels[12].text,"%d kb (%d x 65500)",nes->rom->disksides * 65500,nes->rom->disksides);
			}

			//not in rom database and mapper is -1...UNIF rom
			else if(nes->rom->mapper == -1) {
				strcpy(m->labels[0].text,"File:");
				strcpy(m->labels[1].text,"Name:");
				strcpy(m->labels[2].text,"PRG Size:");
				strcpy(m->labels[3].text,"CHR Size:");
				strcpy(m->labels[4].text,"Mapper:");
				strcpy(m->labels[5].text,"Mirroring:");
				strncpy(m->labels[10].text,nes->rom->filename,30);
				sprintf(m->labels[11].text,"<not in database>");
				sprintf(m->labels[12].text,"%d kb",nes->rom->prgsize / 1024);
				sprintf(m->labels[13].text,"%d kb",nes->rom->chrsize / 1024);
				sprintf(m->labels[14].text,"%d",nes->rom->mapper);
				switch(nes->rom->mirroring) {
					case 0:sprintf(m->labels[15].text,"Horizontal");break;
					case 1:sprintf(m->labels[15].text,"Vertical");break;
					default:sprintf(m->labels[15].text,"Four Screen");break;
				}
			}

			//regular ines rom image
			else {
				strcpy(m->labels[0].text,"File:");
				strcpy(m->labels[1].text,"Name:");
				strcpy(m->labels[2].text,"PRG Size:");
				strcpy(m->labels[3].text,"CHR Size:");
				strcpy(m->labels[4].text,"Mapper:");
				strcpy(m->labels[5].text,"Mirroring:");
				strncpy(m->labels[10].text,nes->rom->filename,30);
				sprintf(m->labels[11].text,"<not in database>");
				sprintf(m->labels[12].text,"%d kb",nes->rom->prgsize / 1024);
				sprintf(m->labels[13].text,"%d kb",nes->rom->chrsize / 1024);
				sprintf(m->labels[14].text,"%d",nes->rom->mapper);
				switch(nes->rom->mirroring) {
					case 0:sprintf(m->labels[15].text,"Horizontal");break;
					case 1:sprintf(m->labels[15].text,"Vertical");break;
					default:sprintf(m->labels[15].text,"Four Screen");break;
				}
			}
			break;
	}
	return(0);
}

void rom_info_create(rom_info_t *m)
{
	int x,y,i;

	memset(m,0,sizeof(rom_info_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 7;
	y = m->info.y = 30;
	m->info.w = 243;
	m->info.h = 120;
	m->info.draw = (drawfunc_t)rom_info_draw;
	m->info.event = (eventfunc_t)rom_info_event;

	button_create(&m->donebtn,"X",x+m->info.w-10,y,0);

	x += 3;
	y += 2 + 11;

	for(i=0;i<10;i++) {
		text_create(&m->labels[i],x,y,"A");
		text_create(&m->labels[i+10],x+(10*6),y,"B");
		y += 10;
	}
}
