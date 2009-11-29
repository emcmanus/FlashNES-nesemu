#ifndef __gui2_memory_viewer_h__
#define __gui2_memory_viewer_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/list.h"
#include "gui2/elements/radiogroup.h"

typedef struct memory_viewer_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//label
	text_t label;

	//label
	list_t list;

	//source
	radiogroup_t source;

	//start addr
	u32 addr;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} memory_viewer_t;

void memory_viewer_draw(memory_viewer_t *m);
int memory_viewer_event(memory_viewer_t *m,int event,int data);
void memory_viewer_create(memory_viewer_t *m);

#endif
