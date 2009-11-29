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

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libmc.h>
#include <graph.h>
#include <stdio.h>

extern u16 size_sjpcm_irx;
extern u8 *sjpcm_irx;
extern u16 size_usbd_irx;
extern u8 *usbd_irx;
extern u16 size_usb_mass_irx;
extern u8 *usb_mass_irx;
/*
extern u16 size_fileXio_irx;
extern u8 *fileXio_irx;
extern u16 size_ps2hdd_irx;
extern u8 *ps2hdd_irx;
extern u16 size_ps2fs_irx;
extern u8 *ps2fs_irx;
extern u16 size_ps2atad_irx;
extern u8 *ps2atad_irx;
*/
static void LoadModules()
{
	int ret;
	char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
	char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40" /*"\0" "-debug"*/;

#ifdef TYPE_MC
	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: SIO2MAN");
		SleepThread();
	}

	ret = SifLoadModule("rom0:MCMAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: MCMAN");
		SleepThread();
	}

	ret = SifLoadModule("rom0:MCSERV", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: MCSERV");
		SleepThread();
	}
#else
	ret = SifLoadModule("rom0:XSIO2MAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: SIO2MAN");
		SleepThread();
	}

	ret = SifLoadModule("rom0:XMCMAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: MCMAN");
		SleepThread();
	}

	ret = SifLoadModule("rom0:XMCSERV", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: MCSERV");
		SleepThread();
	}
#endif
	ret = SifLoadModule("rom0:PADMAN", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: PADMAN");
		SleepThread();
	}

	ret = SifLoadModule("rom0:LIBSD", 0, NULL);
	if (ret < 0) {
		printf("Failed to load module: LIBSD");
		SleepThread();
	}

	if((ret = SifLoadModule("mc0:nesemu/fileXio.irx", 0, NULL)) < 0) {
		printf("Load failed: fileXio.irx");
		SleepThread();
	}

	if((ret = SifLoadModule("mc0:nesemu/ps2atad.irx", 0, NULL)) < 0) {
		printf("Load failed: ps2atad.irx");
		SleepThread();
	}

	if((ret = SifLoadModule("mc0:nesemu/ps2hdd.irx", sizeof(hddarg), hddarg)) < 0) {
		printf("Load failed: ps2hdd.irx");
		SleepThread();
	}

	if((ret = SifLoadModule("mc0:nesemu/ps2fs.irx", sizeof(pfsarg), pfsarg)) < 0) {
		printf("Load failed: ps2fs.irx");
		SleepThread();
	}

	SifExecModuleBuffer(&sjpcm_irx, size_sjpcm_irx, 0, NULL, &ret);
	if (ret < 0) {
		printf("Failed to load module: sjpcm_irx");
		SleepThread();
	}
	SifExecModuleBuffer(&usbd_irx, size_usbd_irx, 0, NULL, &ret);
	if (ret < 0) {
		printf("Failed to load module: usbd_irx");
		SleepThread();
	}
	SifExecModuleBuffer(&usb_mass_irx, size_usb_mass_irx, 0, NULL, &ret);
	if (ret < 0) {
		printf("Failed to load module: usb_mass_irx");
		SleepThread();
	}

/*	SifExecModuleBuffer(&fileXio_irx, size_fileXio_irx, 0, NULL, &ret);
	if (ret < 0) {
		printf("Failed to load module: fileXio_irx");
		SleepThread();
	}

	SifExecModuleBuffer(&ps2atad_irx, size_ps2atad_irx, 0, NULL, &ret);
	if (ret < 0) {
		printf("Failed to load module: ps2atad_irx");
		SleepThread();
	}

	SifExecModuleBuffer(&ps2hdd_irx, size_ps2hdd_irx, sizeof(hddarg), hddarg, &ret);
	if (ret < 0) {
		printf("Failed to load module: ps2hdd_irx");
		SleepThread();
	}

	SifExecModuleBuffer(&ps2fs_irx, size_ps2fs_irx, sizeof(pfsarg), pfsarg, &ret);
	if (ret < 0) {
		printf("Failed to load module: ps2fs_irx");
		SleepThread();
	}
*/
	fileXioInit();

}

void init_system()
{
	SifInitRpc(0);
	LoadModules();

#ifdef TYPE_MC
	if(mcInit(MC_TYPE_MC) < 0) {
		printf("Failed to initialise memcard server!\n");
		SleepThread();
	}
#else
	if(mcInit(MC_TYPE_XMC) < 0) {
		printf("Failed to initialise memcard server!\n");
		SleepThread();
	}
#endif

	//seed random number generator
	srand(time(0));

}

void kill_system()
{
	SifExitRpc();
	LoadExecPS2("",0,NULL);
	SleepThread();
}

void check_events()
{
}

extern u16 size_nesemu_icn;
extern u8 *nesemu_icn;

int setupsave_mc(int port)
{
	int mc_fd;
	int icon_fd,icon_size;
	mcIcon icon_sys;
	char base[32],path[128];

	static iconIVECTOR bgcolor[4] = {
		{  68,  23, 116,  0 }, // top left
		{ 255, 255, 255,  0 }, // top right
		{ 255, 255, 255,  0 }, // bottom left
		{  68,  23, 116,  0 }, // bottom right
	};

	static iconFVECTOR lightdir[3] = {
		{ 0.5, 0.5, 0.5, 0.0 },
		{ 0.0,-0.4,-0.1, 0.0 },
		{-0.5,-0.5, 0.5, 0.0 },
	};

	static iconFVECTOR lightcol[3] = {
		{ 0.3, 0.3, 0.3, 0.00 },
		{ 0.4, 0.4, 0.4, 0.00 },
		{ 0.5, 0.5, 0.5, 0.00 },
	};

	static iconFVECTOR ambient = { 0.50, 0.50, 0.50, 0.00 };


	sprintf(base,"mc%d:NESEMU",port & 1);

	if(fioMkdir(base) < 0) return -1;

	// Set up icon.sys. This is the file which controls how our memory card save looks
	// in the PS2 browser screen. It contains info on the bg colour, lighting, save name
	// and icon filenames. Please note that the save name is sjis encoded.

	memset(&icon_sys, 0, sizeof(mcIcon));
	strcpy(icon_sys.head, "NESE"); //"PS2D");
	strcpy_sjis((short *)&icon_sys.title, "NESEMU\n(c) James Holodnak");
	icon_sys.nlOffset = 16;
	icon_sys.trans = 0x60;
	memcpy(icon_sys.bgCol, bgcolor, sizeof(bgcolor));
	memcpy(icon_sys.lightDir, lightdir, sizeof(lightdir));
	memcpy(icon_sys.lightCol, lightcol, sizeof(lightcol));
	memcpy(icon_sys.lightAmbient, ambient, sizeof(ambient));
	strcpy(icon_sys.view, "nesemu.icn"); // these filenames are relative to the directory
	strcpy(icon_sys.copy, "nesemu.icn"); // in which icon.sys resides.
	strcpy(icon_sys.del, "nesemu.icn");

	// Write icon.sys to the memory card (Note that this filename is fixed)
	sprintf(path,"%s/icon.sys",base);
	mc_fd = fioOpen(path,O_WRONLY | O_CREAT);
	if(mc_fd < 0) return -2;

	fioWrite(mc_fd, &icon_sys, sizeof(icon_sys));
	fioClose(mc_fd);
	printf("icon.sys written sucessfully.\n");

	sprintf(path,"%s/nesemu.icn",base);
	icon_fd = fioOpen(path,O_WRONLY | O_CREAT);
	if(icon_fd < 0) return -6;

	fioWrite(icon_fd,nesemu_icn,size_nesemu_icn);
	fioClose(icon_fd);
	printf("nesemu.icn written sucessfully.\n");

	return 0;

}

int system_setupsavelocation(int loc)
{
	/*
	memory card saving

	setup the save files inside a NESEMU ps2 save.  must be
	a real valid save so the ps2 doesnt complain about it being
	corrupt data.
	*/
	if(loc <= 1) {
		return(setupsave_mc(loc));
	}

	/*
	hard disk saving

	a partition is created for nesemu saves, it is also where
	nesemu looks for roms, a partition called +NES.  need to
	be able to ask to create it, find roms on mc/usb and copy to
	it, and other useful tasks...
	*/
//	else if(loc == 2) {
//		return(setupsave_hdd(loc));
//	}
}
