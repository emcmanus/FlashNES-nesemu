#ifndef __gui2_scrollbar_h__
#define __gui2_scrollbar_h__

#include "gui2/gui2.h"
//#include "gui2/scrollbar.h"

typedef struct scrollbar_s {
	//basic object information
	gui2_obj_t info;

	//scrollbar orientation (h or v)
	int orient;

	//scrollbar size (in pixels)
	int size;

	//scrollbar position
	int position;

	//max position
	int max;

	//when something is clicked
	int (*click)(void *,int,int);

	void *user;
} scrollbar_t;

void scrollbar_draw(scrollbar_t *m);
int scrollbar_event(scrollbar_t *m,int event,int data);
void scrollbar_create(scrollbar_t *m,int x,int y,int size,int o,int (*click)(void *,int,int));

#endif
