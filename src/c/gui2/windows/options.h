#ifndef __gui2_options_h__
#define __gui2_options_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/radiogroup.h"
#include "gui2/elements/checkbox.h"

typedef struct options_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//fds bios picker
	radiogroup_t fdsbios;

	//show info at top checkbox
	checkbox_t showinfo;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} options_t;

void options_draw(options_t *m);
int options_event(options_t *m,int event,int data);
void options_create(options_t *m);

#endif
