#ifndef __gui2_menu_h__
#define __gui2_menu_h__

#include "gui2/gui2.h"
#include "gui2/elements/menuitem.h"

typedef struct menuitems_s {
	char *caption;
	void (*click)();
} menuitems_t;

typedef struct menu_s {
	//basic object information
	gui2_obj_t info;

	//menu caption
	char caption[128];

	//position in the menu bar
	int y;

	//is menu button currently down
	int ispressed;

	//when menu is clicked
	void (*click)(void*);

	//user data
	void *user;

	//menu items
	menuitem_t items[32];
} menu_t;

void menu_draw(menu_t *m);
int menu_event(menu_t *m,int event,int data);
void menu_create(menu_t *m,char *cap,int x,menuitems_t *items);

#endif

