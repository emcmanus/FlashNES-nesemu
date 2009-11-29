#ifndef __gui2_tracer_h__
#define __gui2_tracer_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/text.h"
#include "gui2/elements/list.h"

typedef struct tracer_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//step button (run one cpu cycle)
	button_t stepbtn;

	//step line button (run one line)
	button_t steplinebtn;

	//all kinds of debug info labels
	text_t labels[32];

	//disassembly
	list_t disasm;

	//debug pc
	u32 pc;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} tracer_t;

void tracer_draw(tracer_t *m);
int tracer_event(tracer_t *m,int event,int data);
void tracer_create(tracer_t *m);

#endif
