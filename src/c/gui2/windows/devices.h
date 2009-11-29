#ifndef __gui2_devices_h__
#define __gui2_devices_h__

#include "gui2/gui2.h"
#include "gui2/elements/button.h"
#include "gui2/elements/radiogroup.h"
#include "gui2/elements/radio.h"
#include "gui2/elements/text.h"

typedef struct devices_s {
	//basic object information
	gui2_obj_t info;

	//done button (save changes and close the dialog)
	button_t donebtn;

	//controller port radio buttons
	radio_t none[4];		//no controller connected
	radio_t joy[4];		//joypads
	radio_t zap[2];		//zappers
	radio_t power[2];		//power pads
	radio_t ark[2];		//arkanoid controllers

	//expansion port radio buttons
	radio_t exp_none;		//nothing connected
	radio_t exp_joy;		//two joypads (famicom 4player)
	radio_t exp_ark;		//famicom arkanoid controller
	radio_t exp_key;		//family basic keyboard

	//labels
//	text_t p[4],exp;

	radiogroup_t p[4];
	radiogroup_t exp;

	//flag, is the window being moved by the user?
	int ismoving;

	//is the window showing
	int isshowing;
} devices_t;

void devices_draw(devices_t *m);
int devices_event(devices_t *m,int event,int data);
void devices_create(devices_t *m);

#endif
