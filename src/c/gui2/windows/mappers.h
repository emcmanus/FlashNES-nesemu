#ifndef __gui2_mappers_h__
#define __gui2_mappers_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/checkbox.h"

typedef struct mappers_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//label
	text_t label;

	//label
	text_t m105_label;

	//checkboxes
	checkbox_t m105_check[4];

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} mappers_t;

void mappers_draw(mappers_t *m);
int mappers_event(mappers_t *m,int event,int data);
void mappers_create(mappers_t *m);

#endif
