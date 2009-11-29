#ifndef __gui2_button_h__
#define __gui2_button_h__

#include "gui2/gui2.h"
//#include "gui2/button.h"

typedef struct button_s {
	//basic object information
	gui2_obj_t info;

	//menu item caption
	char caption[64];

	//is button pressed down
	int ispressed;

	//when item is clicked, call this function
	void (*click)(void*);

	//user data
	void *user;
} button_t;

void button_draw(button_t *m);
int button_event(button_t *m,int event,int data);
void button_create(button_t *m,char *cap,int x,int y,void (*click)(void*));

#endif
