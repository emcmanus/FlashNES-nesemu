#ifndef __gui2_gui_input_h__
#define __gui2_gui_input_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/edit.h"

typedef struct gui_input_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//reassign input key mapping
	int needinput;

	//key button controls
	text_t key_label[16];
	edit_t key_edit[16];
	button_t key_button[16];

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} gui_input_t;

void gui_input_draw(gui_input_t *m);
int gui_input_event(gui_input_t *m,int event,int data);
void gui_input_create(gui_input_t *m);

#endif
