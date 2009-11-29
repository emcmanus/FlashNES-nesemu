#ifndef __gui2_supported_mappers_h__
#define __gui2_supported_mappers_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/list.h"

typedef struct supported_mappers_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//label
	text_t label;

	//label
	list_t ines_list,unif_list;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} supported_mappers_t;

void supported_mappers_draw(supported_mappers_t *m);
int supported_mappers_event(supported_mappers_t *m,int event,int data);
void supported_mappers_create(supported_mappers_t *m);

#endif
