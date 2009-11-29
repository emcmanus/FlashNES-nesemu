#ifndef __gui2_list_h__
#define __gui2_list_h__

#include "gui2/gui2.h"
#include "gui2/elements/scrollbar.h"

//TODO: linked list retard!

#ifdef WIN32
//24k items max, LOTS of memory used
#define MAX_LISTITEMS	(1024 * 24)
#else
//7k items max
#define MAX_LISTITEMS	(1024 * 7)
#endif

typedef struct list_s {
	//basic object information
	gui2_obj_t info;

	//scrollbar
	scrollbar_t scrollbar;

	//current list items
	char items[MAX_LISTITEMS][512];

	//selected item index
	int selected;

	//view start index
	int viewstart;

	//number of items
	int numitems;

	//current path (TODO: need to make subclassed filelist_t or something)
	char path[1024];

	//callback to refresh the list
	char *(*refresh)(struct list_s*,int);

	//item clicked (selected)
	void (*click)(void *,char *);

	//item double clicked
	void (*click2)(void *,char *);

	//user data
	void *user;
} list_t;

void list_draw(list_t *m);
int list_event(list_t *m,int event,int data);
void list_create(list_t *m,int x,int y,int w,int h,char *(*refresh)(list_t*,int));

#endif
