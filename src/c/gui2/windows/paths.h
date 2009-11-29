#ifndef __gui2_paths_h__
#define __gui2_paths_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/edit.h"

typedef struct paths_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//labels
	text_t labels[3];

	//path edit boxes
	edit_t statepath,srampath,diskchangepath;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} paths_t;

void paths_draw(paths_t *m);
int paths_event(paths_t *m,int event,int data);
void paths_create(paths_t *m);

#endif
