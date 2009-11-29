#ifndef __gui2_checkbox_h__
#define __gui2_checkbox_h__

#include "gui2/gui2.h"

typedef struct checkbox_s {
	//basic object information
	gui2_obj_t info;

	//text
	char text[512];

	//state
	int state;

	//user data
	void *user;

	//user clicks checkbox button
	void (*click)(void*,int);
} checkbox_t;

void checkbox_draw(checkbox_t *m);
int checkbox_event(checkbox_t *m,int event,int data);
void checkbox_create(checkbox_t *m,int x,int y,char *text,int state,void (*click)(void*,int));

#endif
