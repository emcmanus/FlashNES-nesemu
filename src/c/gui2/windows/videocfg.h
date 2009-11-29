#ifndef __gui2_video_h__
#define __gui2_video_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/radiogroup.h"

typedef struct video_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	radiogroup_t filter;
	radiogroup_t videomode;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} video_t;

void video_draw(video_t *m);
int video_event(video_t *m,int event,int data);
void video_create(video_t *m);

#endif
