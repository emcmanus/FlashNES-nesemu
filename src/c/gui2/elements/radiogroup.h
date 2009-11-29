#ifndef __gui2_radiogroup_h__
#define __gui2_radiogroup_h__

#include "gui2/gui2.h"
#include "gui2/elements/text.h"
#include "gui2/elements/radio.h"

typedef struct radiogroup_s {
	//basic object information
	gui2_obj_t info;

	//group label
	text_t label;

	//radio buttons
	radio_t radios[16];

	//selection changed
	void (*changed)(void*,int);

	//selected buttons index
	int selected;

	//user data
	void *user;
} radiogroup_t;

void radiogroup_draw(radiogroup_t *m);
int radiogroup_event(radiogroup_t *m,int event,int data);
void radiogroup_create(radiogroup_t *m,int x,int y,char *label,int selected,char *captions[],void (*changed)(void*,int));

#endif
