#ifndef __gui2_text_h__
#define __gui2_text_h__

#include "gui2/gui2.h"

typedef struct text_s {
	//basic object information
	gui2_obj_t info;

	//current text
	char text[512];
} text_t;

void text_draw(text_t *m);
int text_event(text_t *m,int event,int data);
void text_create(text_t *m,int x,int y,char *text);

#endif



