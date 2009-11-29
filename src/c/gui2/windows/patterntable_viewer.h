#ifndef __gui2_patterntable_viewier_h__
#define __gui2_patterntable_viewier_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/scrollbar.h"

typedef struct pt_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} pt_t;

void pt_draw(pt_t *m);
int pt_event(pt_t *m,int event,int data);
void pt_create(pt_t *m);

#endif
