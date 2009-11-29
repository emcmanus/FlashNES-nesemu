#ifndef __gui2_sound_h__
#define __gui2_sound_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/checkbox.h"
#include "gui2/elements/text.h"

typedef struct sound_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//sound enabled
	checkbox_t enabled;

	//enabled channels
	checkbox_t sq1,sq2,tri,noise,dmc,ext;

	//some text
	text_t tc;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} sound_t;

void sound_draw(sound_t *m);
int sound_event(sound_t *m,int event,int data);
void sound_create(sound_t *m);

#endif

