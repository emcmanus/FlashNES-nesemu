#ifndef __gui2_menuitem_h__
#define __gui2_menuitem_h__

#include "gui2/gui2.h"

typedef struct menuitem_s {
	//basic object information
	gui2_obj_t info;

	//menu item caption
	char caption[128];

	//is menu item hightlighted
	int ishighlight;

	//when item is clicked, call this function
	void (*click)();
} menuitem_t;

void menuitem_draw(menuitem_t *m);
int menuitem_event(menuitem_t *m,int event,int data);
void menuitem_create(menuitem_t *m,char *cap,int x,int y,void (*click)());

#endif
