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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "system/file.h"
#include "log.h"

static FILE *logfd = 0;

#ifdef LOGGER
	static char logfilename[1024] = "nesemu.log";
#endif

extern char curdir[];

void log_init()
{
#ifdef LOGGER
	sprintf(logfilename,"%s/nesemu.log",curdir);
	if(logfd)
		return;
	if((logfd = fopen(logfilename,"wt")) == 0) {
		printf("error opening log file\n");
		return;
	}
#else
	printf("logging disabled\n");
#endif
}

void log_kill()
{
	if(logfd)
		fclose(logfd);
	logfd = 0;
}

void log_print(char *str)
{
	//output message to console
	printf(str);

	//if log file isnt open, maybe logging is disabled
	if(logfd == 0)
		return;

	//write to log file
	fputs(str,logfd);

	//flush file
	fflush(logfd);
}

void log_message(char *str,...)
{
	char buffer[512];					//buffer to store vsprintf'd message in
	va_list argptr;					//argument data

	va_start(argptr,str);			//get data
	vsprintf(buffer,str,argptr);	//print to buffer
	va_end(argptr);					//done!
	log_print(buffer);
}

void log_warning(char *str,...)
{
	char buffer[512];					//buffer to store vsprintf'd message in
	va_list argptr;					//argument data

	va_start(argptr,str);			//get data
	vsprintf(buffer,str,argptr);	//print to buffer
	va_end(argptr);					//done!
	log_message("error: %s",buffer);
}

void log_error(char *str,...)
{
	char buffer[512];					//buffer to store vsprintf'd message in
	va_list argptr;					//argument data

	va_start(argptr,str);			//get data
	vsprintf(buffer,str,argptr);	//print to buffer
	va_end(argptr);					//done!
	log_message("error: %s",buffer);
}
