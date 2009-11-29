#ifndef __gui2_edit_h__
#define __gui2_edit_h__

#include "gui2/gui2.h"

typedef struct edit_s {
	//basic object information
	gui2_obj_t info;

	//current text
	char text[512];

	//callback when text inside editor changes
	void (*change)(char*);
} edit_t;

void edit_draw(edit_t *m);
int edit_event(edit_t *m,int event,int data);
void edit_create(edit_t *m,int x,int y,int w,int h,void (*change)(char*));

#endif


