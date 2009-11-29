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

#ifndef __filesystem_h__
#define __filesystem_h__

#define FI_FLAG_FILE			0x01
#define FI_FLAG_DIRECTORY	0x02
#define FI_FLAG_LINK			0x04

typedef struct file_info_s {
	u8		flags;		//flags
	char	*name;		//path
} file_info_t;

typedef struct file_info_static_s {
	u8		flags;			//flags
	char	name[512];		//file name
} file_info_static_t;

void filesystem_init();
void filesystem_kill();
void filesystem_translate_path(char *in,char *out,int outsize);
file_info_t *filesystem_findfirst(char *path);
file_info_t *filesystem_findnext();
void filesystem_findclose();

#endif
