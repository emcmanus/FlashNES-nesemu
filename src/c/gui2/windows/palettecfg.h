#ifndef __gui2_palette_h__
#define __gui2_palette_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/edit.h"
#include "gui2/elements/text.h"

typedef struct palette_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//edit boxes
	edit_t hueedit,satedit;

	//labels
	text_t huetext,sattext;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} palette_t;

void palette_draw(palette_t *m);
int palette_event(palette_t *m,int event,int data);
void palette_create(palette_t *m);

#endif
