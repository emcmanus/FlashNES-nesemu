#ifndef __gui2_load_h__
#define __gui2_load_h__

#include "gui2/gui2.h"
#include "gui2/elements/menu.h"
#include "gui2/elements/button.h"
#include "gui2/elements/list.h"
#include "gui2/elements/edit.h"
#include "gui2/elements/text.h"

typedef struct load_s {
	//basic object information
	gui2_obj_t info;

	//close button (to close the dialog)
	button_t closebtn;

	//load button
	button_t loadbtn;

	//misc labels
	text_t filetext,dirtext,pathtext,rominfo[2];

	//lists of roms and directories user can scroll thru
	list_t romlist,dirlist;

	//filename edit
	edit_t edit;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} load_t;

void load_draw(load_t *m);
int load_event(load_t *m,int event,int data);
void load_create(load_t *m);

#endif
