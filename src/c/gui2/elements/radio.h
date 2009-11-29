#ifndef __gui2_radio_h__
#define __gui2_radio_h__

#include "gui2/gui2.h"

typedef struct radio_s {
	//basic object information
	gui2_obj_t info;

	//text
	char text[512];

	//state
	int state;

	//user data
	void *user;

	//user clicks radio button
	void (*click)(void*,int);
} radio_t;

void radio_draw(radio_t *m);
int radio_event(radio_t *m,int event,int data);
void radio_create(radio_t *m,int x,int y,char *text,int state,void (*click)(void*,int));

#endif
