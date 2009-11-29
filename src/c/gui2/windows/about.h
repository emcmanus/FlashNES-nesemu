#ifndef __gui2_about_h__
#define __gui2_about_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"

typedef struct about_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	text_t text[20];

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} about_t;

void about_draw(about_t *m);
int about_event(about_t *m,int event,int data);
void about_create(about_t *m);

#endif
