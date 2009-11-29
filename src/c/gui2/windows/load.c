#include <stdio.h>
#include <string.h>
#include "gui2/windows/load.h"
#include "nesemu.h"

/*
gui2 load object
-------------------
load roms
*/

void load_draw(load_t *m)
{
	int x,y;

	x = m->info.x;
	y = m->info.y;
	if(m->isshowing == 0)
		return;
	gui_draw_border(GUI_COLOR_DARKBLUE,x,y,m->info.w,m->info.h);
	gui_draw_border(GUI_COLOR_GREY,x,y,m->info.w,9);
	gui_draw_text(GUI_TEXT,x+2,y+2,"Load ROM");
	button_draw(&m->closebtn);
	button_draw(&m->loadbtn);
	list_draw(&m->romlist);
	list_draw(&m->dirlist);
	edit_draw(&m->edit);
	text_draw(&m->pathtext);
	text_draw(&m->rominfo[0]);
	text_draw(&m->rominfo[1]);
}

int load_event(load_t *m,int event,int data)
{
	if(m->isshowing == 0)
		return(0);
	switch(event) {
		case E_MOUSEMOVE:
		case E_MOUSEDOWN:
		case E_MOUSEDOWN2:
			if(mouseinrange(&m->info,data) == 0)
				return(0);
			button_event(&m->closebtn,event,data);
			button_event(&m->loadbtn,event,data);
			if(list_event(&m->romlist,event,data) > 0)
				return(1);
			if(list_event(&m->dirlist,event,data) > 0)
				return(1);
			edit_event(&m->edit,event,data);
			break;
		case E_MOUSEUP:
			if(button_event(&m->closebtn,event,data) > 0)
				m->isshowing = 0;
			button_event(&m->loadbtn,event,data);
			if(list_event(&m->romlist,event,data) > 0)
				return(1);
			if(list_event(&m->dirlist,event,data) > 0)
				return(1);
			edit_event(&m->edit,event,data);
			break;
	}
	return(0);
}

static int isrom(char *f)
{
	if(stricmp(f + strlen(f) - 4,".nes") == 0 ||
		stricmp(f + strlen(f) - 4,".fds") == 0 ||
		stricmp(f + strlen(f) - 4,".nsf") == 0 ||
		stricmp(f + strlen(f) - 4,".unf") == 0 ||
		stricmp(f + strlen(f) - 5,".unif") == 0 ||
		stricmp(f + strlen(f) - 7,".nes.gz") == 0 ||
		stricmp(f + strlen(f) - 7,".fds.gz") == 0 ||
		stricmp(f + strlen(f) - 7,".nsf.gz") == 0 ||
		stricmp(f + strlen(f) - 7,".unf.gz") == 0 ||
		stricmp(f + strlen(f) - 8,".unif.gz") == 0)
		return(1);
//	log_message("'%s' isnt a rom\n",f);
	return(0);
}

char *refresh_rom(list_t *m,int mode)
{
	file_info_t *fi;
	static int findfirst;

	//begin refresh
	if(mode == 1) {
		findfirst = 1;
		//success
		return(0);
	}
	//get next item
	else if(mode == 0) {
		do {
			//find first/next file
			if(findfirst) {
				fi = filesystem_findfirst(m->path);
				findfirst = 0;
			}
			else
				fi = filesystem_findnext();
			//no more files
			if(fi == 0) {
//				log_message("found last file\n");
				filesystem_findclose();
				return(0);
			}
		} while((fi->flags != FI_FLAG_FILE) || (isrom(fi->name) == 0));
		return(fi->name);
	}
	return(0);
}

char *refresh_root(list_t *m,int mode)
{
	file_info_t *fi;
	static int findfirst = 0;
	static char path[] = "/";

	//begin refresh
	printf("refresh root: mode = %d\n",mode);
	if(mode == 1) {
		findfirst = 1;
		//success
		return(0);
	}
	//get next item
	else if(mode == 0) {
		do {
			//find first/next file
			if(findfirst) {
				fi = filesystem_findfirst(path);
				findfirst = 0;
			}
			else
				fi = filesystem_findnext();
			//no more files
			if(fi == 0) {
				filesystem_findclose();
				return(0);
			}
		} while((fi->flags != FI_FLAG_DIRECTORY) || (strcmp(fi->name,".") == 0) || (strcmp(fi->name,"..") == 0));
		return(fi->name);
	}
	return(0);
}

char *refresh_dir(list_t *m,int mode)
{
	file_info_t *fi;
	static int findfirst = 0;

	//begin refresh
	if(mode == 1) {
		findfirst = 1;
		//success
		return(0);
	}

	//get next item
	else if(mode == 0) {
		do {
			//find first/next file
			if(findfirst) {
				fi = filesystem_findfirst(m->path);
				findfirst = 0;
			}
			else
				fi = filesystem_findnext();
			//no more files
			if(fi == 0) {
				filesystem_findclose();
				return(0);
			}
		} while((fi->flags != FI_FLAG_DIRECTORY) || (strcmp(fi->name,".") == 0) || (strcmp(fi->name,"..") == 0));
		return(fi->name);
	}
#if (!defined(LINUX) && !defined(OSX))// && !defined(PS2))
	else
		return(refresh_root(m,mode - 2));
#endif

	return(0);
}

void change_filename(char *s)
{

}

static void click_romlist(void *u,char *s)
{
	char str[1024];
	load_t *l = (load_t*)u;
	int fp;

	if(s == 0) {
		strcpy(l->edit.text,"");
		strcpy(l->rominfo[0].text,"");
		strcpy(l->rominfo[1].text,"");
		return;
	}

	strcpy(l->edit.text,s);

	filesystem_translate_path(l->pathtext.text,str,1024);

	strcat(str,l->edit.text);

	if((fp = file_open(str,"rb")) != 0) {
		u8 header[16];
		int n;

		//read first 16 bytes of file
		file_read(fp,header,16);

		//ines rom
		if(memcmp(header,"NES\x1A",4) == 0) {
			n = (header[6] & 0xF0) >> 4;
			n |= header[7] & 0xF0;
			sprintf(l->rominfo[0].text,"iNES ROM Image, Mapper %d",n);
			sprintf(l->rominfo[1].text,"%d PRG Banks, %d CHR Banks",header[4],header[5]);
		}
		else if(memcmp(header,"FDS\x1A",4) == 0) {
			sprintf(l->rominfo[0].text,"FDS Disk Image");
			sprintf(l->rominfo[1].text,"%d Disk Sides",header[4]);
		}
		else if(memcmp(header,"NESM\x1A",5) == 0) {
			nsfheader_t nsfheader;
			char strs[2][33];

			file_seek(fp,0,SEEK_SET);
			file_read(fp,&nsfheader,sizeof(nsfheader_t));
			sprintf(l->rominfo[0].text,"NES Sound File, %d Songs, %s",nsfheader.totalsongs,nsfheader.ntscpal?"PAL":"NTSC");
			memcpy(strs[0],nsfheader.name,32);
			memcpy(strs[1],nsfheader.artist,32);
			strs[0][32] = strs[1][32] = 0;
			sprintf(l->rominfo[1].text,"%s%s%s",strs[0],strlen(strs[1])?", ":"",strs[1]);
		}
		else if(memcmp(header,"UNIF",4) == 0) {
			sprintf(l->rominfo[0].text,"UNIF ROM Image");
			sprintf(l->rominfo[1].text,"No additional info");
		}
		else {
			strcpy(l->rominfo[0].text,"unknown rom image type");
			strcpy(l->rominfo[1].text,"");
		}
		file_close(fp);
	}
}

static void click_load(void *u);

static void click2_romlist(void *u,char *s)
{
	load_t *l = (load_t*)u;

	strcpy(l->edit.text,s);
	click_load(u);
}

static void click2_dirlist(void *u,char *s)
{
	char str[1024],*p;

	if(s == 0)
		return;

//	log_message("dirlist clicked (%s)\n",s);
	if(strcmp(s,"..") == 0) {
		strcpy(str,config.path_rom);
		p = strrchr(str,'/');
		if(p == 0)
			log_error("cannot change to directory '%s' + '../'\n",str);
		else
			*p = 0;
		p = strrchr(str,'/');
		if(p == 0)
			log_error("cannot change to directory '%s' + '../'\n",str);
		else
			*p = 0;
	}
	else if(s[0] == '[' && s[2] == ':' && s[3] == ']' && s[4] == 0) {
		sprintf(str,"/%c:",s[1]);
	}
	else
		sprintf(str,"%s%s",config.path_rom,s);
	strcat(str,"/");
	strcpy(((load_t*)u)->pathtext.text,str);
	strcpy(((load_t*)u)->romlist.path,str);
	strcpy(((load_t*)u)->dirlist.path,str);
	strcpy(config.path_rom,str);

	//tell lists to refresh
	list_event(&((load_t*)u)->romlist,E_REFRESH,0);
	list_event(&((load_t*)u)->dirlist,E_REFRESH,1);
}

static void click_load(void *u)
{
	load_t *l = (load_t*)u;
	char str[1024];

//	log_message("load clicked (%s -- %s)\n",l->pathtext.text,l->edit.text);

	filesystem_translate_path(l->pathtext.text,str,1024);

	strcat(str,l->edit.text);

	if(loadrom(str) == 0) {
		//exit gui
		joykeys[config.gui_keys[0]] = 1;
		l->isshowing = 0;
	}
}

void load_create(load_t *m)
{
	int x,y;

	memset(m,0,sizeof(load_t));

	m->info.type = G_WINDOW;
	x = m->info.x = 8;
	y = m->info.y = 16;
	m->info.w = 240;
	m->info.h = 215;
	m->info.draw = (drawfunc_t)load_draw;
	m->info.event = (eventfunc_t)load_event;

	button_create(&m->closebtn,"X",x+240-10,y,0);

	x += 3;
	y += 2 + 11;
	list_create(&m->romlist,x,y,160,160,refresh_rom);
	list_create(&m->dirlist,x+162,y,72,160,refresh_dir);

	m->romlist.click = click_romlist;
	m->romlist.click2 = click2_romlist;
	m->dirlist.click2 = click2_dirlist;

	m->romlist.user = m;
	m->dirlist.user = m;

	//tell lists to refresh
	list_event(&m->romlist,E_REFRESH,0);
	list_event(&m->dirlist,E_REFRESH,1);

	y += 160 + 4;
	text_create(&m->pathtext,x,y,config.path_rom);

	y += 8;
	edit_create(&m->edit,x,y,234,14,change_filename);

	button_create(&m->loadbtn,"Load",x + 200,y + 17,click_load);
	m->loadbtn.user = m;

	y += 14;
	text_create(&m->rominfo[0],x,y,"");
	y += 8;
	text_create(&m->rominfo[1],x,y,"");

}
