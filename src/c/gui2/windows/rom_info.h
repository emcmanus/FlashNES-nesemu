#ifndef __gui2_rom_info_h__
#define __gui2_rom_info_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"

typedef struct rom_info_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//label
	text_t labels[20];

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} rom_info_t;

void rom_info_draw(rom_info_t *m);
int rom_info_event(rom_info_t *m,int event,int data);
void rom_info_create(rom_info_t *m);

#endif
