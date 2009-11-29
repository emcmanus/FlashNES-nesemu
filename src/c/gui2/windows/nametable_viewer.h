#ifndef __gui2_nametable_viewier_h__
#define __gui2_nametable_viewier_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/scrollbar.h"

typedef struct nt_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//scrollbars
	scrollbar_t hscrollbar,vscrollbar;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} nt_t;

void nt_draw(nt_t *m);
int nt_event(nt_t *m,int event,int data);
void nt_create(nt_t *m);

#endif

