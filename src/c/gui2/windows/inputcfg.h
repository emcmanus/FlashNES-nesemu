#ifndef __gui2_input_h__
#define __gui2_input_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/edit.h"

typedef struct input_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;


	int needinput;

	//key button controls
	text_t key_label[16];
	edit_t key_edit[16];
	button_t key_button[16];

	text_t key_label2[16];
	edit_t key_edit2[16];
	button_t key_button2[16];

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} input_t;

void input_draw(input_t *m);
int input_event(input_t *m,int event,int data);
void input_create(input_t *m);

#endif
