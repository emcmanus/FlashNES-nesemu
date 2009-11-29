/***************************************************************************
 *   Copyright (C) 2006-2009 by Dead_Body   *
 *   jamesholodnak@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <libpad.h>
#include <kernel.h>
#include "defines.h"
#include "nes/nes.h"
#include "system/input.h"
#include "system/config.h"
#include "gui2/gui2.h"

//badly needs a rewrite, very messy code
//some...ALL code borrowed from the 'pad' ps2sdk sample
#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif

/*
 * Global var's
 */
// pad_dma_buf is provided by the user, one buf for each pad
// contains the pad's current state
static char padBuf[256] __attribute__((aligned(64)));
static char padBuf2[256] __attribute__((aligned(64)));

static char actAlign[6];
static int actuators;

static int p1_connected = 0,p2_connected = 0;

/*
 * waitPadReady()
 */
int waitPadReady(int port, int slot)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            printf("Please wait, pad(%d,%d) is in state %s (%d)\n", port, slot, stateString,state);
            if(state == 0 && port)
            	return(1);
        }
        lastState = state;
        state=padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
        printf("Pad OK!\n");
    }
    return 0;
}

/*
 * initializePad()
 */
static int initializePad(int port, int slot)
{

    int ret;
    int modes;
    int i;

    if(waitPadReady(port, slot) == 1)
    	return(0);

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0) {
    		//this is ok, add support for ps1 control pads
        printf("This is a digital controller?\n");
//        return 1;
    }
//    else {
/*
    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
//        printf("This is no Dual Shock controller\n");
        return 1;
    }
*/
    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        printf("This is no Dual Shock controller??\n");
//        return 1;
	    padSetMainMode(port, slot, PAD_MMODE_DIGITAL, 0);
    }
		else
    // When using MMODE_LOCK, user cant change mode with Select button
	    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady(port, slot);
//    printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    waitPadReady(port, slot);
//    printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    waitPadReady(port, slot);
    actuators = padInfoAct(port, slot, -1, 0);
//    printf("# of actuators: %d\n",actuators);

    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady(port, slot);
        printf("padSetActAlign: %d\n",padSetActAlign(port, slot, actAlign));
    }
    else {
        printf("Did not find any actuators.\n");
    }

    waitPadReady(port, slot);

    return 1;
}


int joyx,joyy;		//x and y coords for paddle/mouse
u8 joyzap;			//zapper trigger
u8 joykeys[320];	//keyboard state

void input_init()
{
	int ret;
	int port = 0; // 0 -> Connector 1, 1 -> Connector 2
	int slot = 0; // Always zero if not using multitap

	//need to automatically init/deinit when controllers are inserted/removed
	padInit(0);
	printf("PortMax: %d\n", padGetPortMax());
	printf("SlotMax: %d\n", padGetSlotMax(port));

	if((ret = padPortOpen(port, slot, padBuf)) == 0) {
		printf("padOpenPort port 1 failed: %d\n", ret);
		SleepThread();
	}
	else if(!initializePad(port, slot)) {
		printf("pad 1 initalization failed!\n");
		SleepThread();
	}
	else {
		printf("controller 1 init ok\n");
		p1_connected = 1;
	}

	port++;

	if((ret = padPortOpen(port, slot, padBuf2)) == 0) {
		printf("padOpenPort port 2 failed: %d\n", ret);
//		SleepThread();
	}
	else if(!initializePad(port, slot)) {
		printf("pad 2 initalization failed!\n");
//		SleepThread();
	}
	else {
		printf("controller 2 init ok\n");
		p2_connected = 1;
	}
}

void input_kill()
{
//	TerminateThread(inputthreadid);
//	printf("input thread killed\n");
}

//poll controller 2
void input_poll2()
{
	int ret;
	int port = 1; // 0 -> Connector 1, 1 -> Connector 2
	int slot = 0; // Always zero if not using multitap
	static struct padButtonStatus buttons;
	static u32 paddata;
	static u32 new_pad,new_pad2;
	static u32 old_pad = 0;

	ret = padGetState(port,slot);
	while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
		if(ret == PAD_STATE_DISCONN) {
			printf("Pad(%d, %d) is disconnected\n", port, slot);
		}
      ret = padGetState(port,slot);
	printf("unstable\n");
	}
	ret = padRead(port,slot,&buttons); // port, slot, buttons
	if(ret != 0) {
		paddata = 0xffff ^ buttons.btns;

		new_pad = paddata;
		new_pad2 = paddata & ~old_pad;
		old_pad = paddata;
		if(new_pad & PAD_UP)			joykeys[config.joy_keys[1][4]] = 1;
		if(new_pad & PAD_DOWN)		joykeys[config.joy_keys[1][5]] = 1;
		if(new_pad & PAD_LEFT)		joykeys[config.joy_keys[1][6]] = 1;
		if(new_pad & PAD_RIGHT)		joykeys[config.joy_keys[1][7]] = 1;
		if(new_pad & PAD_SQUARE)	joykeys[config.joy_keys[1][1]] = 1;
		if(new_pad & PAD_CROSS)		joykeys[config.joy_keys[1][0]] = 1;
		if(new_pad & PAD_TRIANGLE)	joykeys[config.joy_keys[1][1]] = 1;
		if(new_pad & PAD_CIRCLE)	joykeys[config.joy_keys[1][0]] = 1;
		if(new_pad & PAD_SELECT)	joykeys[config.joy_keys[1][2]] = 1;
		if(new_pad & PAD_START)		joykeys[config.joy_keys[1][3]] = 1;
	}
	else {
		printf("error: padRead returned %d\n",ret);
	}
}

static u32 frames = 0;

void input_poll1()
{
	int ret;
	int port = 0; // 0 -> Connector 1, 1 -> Connector 2
	int slot = 0; // Always zero if not using multitap
	static struct padButtonStatus buttons;
	static u32 paddata;
	static u32 new_pad,new_pad2;
	static u32 old_pad = 0;
	static u32 downtime,lastdowntime = 0;

	ret = padGetState(port,slot);
	while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
		if(ret == PAD_STATE_DISCONN) {
			printf("Pad(%d, %d) is disconnected\n", port, slot);
		}
      ret = padGetState(port,slot);
	printf("unstable\n");
	}
	ret = padRead(port,slot,&buttons); // port, slot, buttons
	if(ret != 0) {
		mousemove_u mm;
		mousebutton_u mb;
		int mousemove = 0;

		paddata = 0xffff ^ buttons.btns;
		new_pad = paddata;
		new_pad2 = paddata & ~old_pad;
		old_pad = paddata;
		if(new_pad & PAD_UP)		{	joykeys[config.joy_keys[0][4]] = 1; joyy--;	mousemove = 1; }
		if(new_pad & PAD_DOWN)	{	joykeys[config.joy_keys[0][5]] = 1; joyy++;	mousemove = 1; }
		if(new_pad & PAD_LEFT)	{	joykeys[config.joy_keys[0][6]] = 1; joyx--;	mousemove = 1; }
		if(new_pad & PAD_RIGHT)	{	joykeys[config.joy_keys[0][7]] = 1; joyx++;	mousemove = 1; }
		if(new_pad & PAD_SQUARE)	joykeys[config.joy_keys[0][1]] = 1;
		if(new_pad & PAD_CROSS)		joykeys[config.joy_keys[0][0]] = 1;
		if(new_pad & PAD_TRIANGLE)	joykeys[config.joy_keys[0][1]] = 1;
		if(new_pad & PAD_CIRCLE)	joykeys[config.joy_keys[0][0]] = 1;
		if(new_pad & PAD_SELECT)	joykeys[config.joy_keys[0][2]] = 1;
		if(new_pad & PAD_START)		joykeys[config.joy_keys[0][3]] = 1;
		if(new_pad & PAD_R3)			joykeys[config.gui_keys[3]] = 1;
		if(new_pad & PAD_L3) {
			if(new_pad2 & PAD_R1)	joykeys[config.gui_keys[1]] = 1;
			if(new_pad2 & PAD_L1)	joykeys[config.gui_keys[2]] = 1;
		}
		else {
//			if(new_pad2 & PAD_L1)	joy1 |= INPUT_PAGEUP;
//			if(new_pad2 & PAD_R1)	joy1 |= INPUT_PAGEDOWN;
			if(new_pad & PAD_L2)		joykeys[config.gui_keys[5]] = 1;
			if(new_pad2 & PAD_R2)	joykeys[config.gui_keys[0]] = 1;
		}
		//right
		if(buttons.ljoy_h > 0xF0) {			joyx+=2; mousemove = 1;	}
		else if(buttons.ljoy_h > 0xB0) {		joyx+=1; mousemove = 1;	}
		else if(buttons.ljoy_h < 0x10) {		joyx-=2; mousemove = 1;	}
		else if(buttons.ljoy_h < 0x50) {		joyx-=1; mousemove = 1;	}
		if(buttons.ljoy_v > 0xF0) {			joyy+=2; mousemove = 1;	}
		else if(buttons.ljoy_v > 0xB0) {		joyy+=1; mousemove = 1;	}
		else if(buttons.ljoy_v < 0x10) {		joyy-=2; mousemove = 1;	}
		else if(buttons.ljoy_v < 0x50) {		joyy-=1; mousemove = 1;	}

		//'click' down
		if(new_pad2 & PAD_CROSS) {
			int d = 0;

			mb.info.x = joyx;
			mb.info.y = joyy;
			mb.info.btn = 1;
			if((frames - lastdowntime) < 20) {
				d = 1;
				lastdowntime = 0;
			}
			else
				lastdowntime = frames;
			gui2_event(d ? E_MOUSEDOWN2 : E_MOUSEDOWN,mb.data);
		}
		//'click' up
		if(((new_pad2 & PAD_CROSS) == 0) && (old_pad & PAD_CROSS)) {
			mb.info.x = joyx;
			mb.info.y = joyy;
			mb.info.btn = 1;
			gui2_event(E_MOUSEUP,mb.data);
		}
		if(joyx < 0) joyx = 0;
		if(joyy < 0) joyy = 0;
		if(joyx > 255) joyx = 255;
		if(joyy > 255) joyy = 255;
		if(mousemove) {
			mm.data = 0;
			mm.info.x = joyx;
			mm.info.y = joyy;
			gui2_event(E_MOUSEMOVE,mm.data);
		}
	}
	else {
		printf("error: padRead returned %d\n",ret);
	}
}

void input_poll()
{
	memset(joykeys,0,320 * sizeof(u8));

	if(p1_connected)
		input_poll1();
	if(p2_connected)
		input_poll2();
	frames++;
}
