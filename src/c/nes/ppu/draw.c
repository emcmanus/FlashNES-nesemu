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

#include <string.h>
#include "nes/ppu/event.h"
#include "nes/nes.h"

#ifdef PS2
#define C_RENDER
#endif

static void drawbackground(u8 *dest)
{
	u64 shiftleft = (8 - nes->ppu.scrollx) * 8;
	u64 shiftright = nes->ppu.scrollx * 8;
	cache_t pixels,offset,mask = CACHE_MASK,*cache;
	u64 *scr64 = (u64*)dest;
	u32 scroll = nes->ppu.scroll & 0x3FF;
	u8 *nametableptr;
	u8 nametable,attrib,tile,n;
	int t = 0;

	//get pointer to visible nametable
	nametable = (nes->ppu.scroll >> 10) & 3;
	nametableptr = nes->ppu.readpages[8 | nametable];

	//draw 33 tiles
	for(t=0;t<=33;t++) {
		//get tile index
		tile = nametableptr[scroll];

		//if tile draw callback, do it
		if(nes->mapper->tile)
			nes->mapper->tile(tile,((nes->ppu.ctrl0 & 0x10) >> 2) >> 2);

		//fetch attribute byte for this tile
		n = nametableptr[0x3c0 + ((scroll >> 2) & 7) + (((scroll >> (2 + 5)) & 7) << 3)];

		//shift out the attribute bits used from the attribute byte
		attrib = (n >> (((scroll & 2) | (((scroll >> 5) & 2) << 1)))) & 3;

		//tile bank cache pointer
		cache = nes->ppu.cachepages[(tile >> 6) | ((nes->ppu.ctrl0 & 0x10) >> 2)];

		//index to the tile data start, then the tile half (upper or lower half)
		cache += ((tile & 0x3F) * 2) + ((nes->ppu.scroll >> 14) & 1);

		//generate attribute bits for eight pixels
		offset = attrib * 0x0404040404040404LL;

		//retreive the tile pixels used
		pixels = *cache >> (((nes->ppu.scroll >> 12) & 3) << 1);

		//get pixel qword
		pixels = (pixels & mask) + offset;

		//draw the tile line
#ifdef C_RENDER
#ifdef __GNUC__
		if(nes->ppu.scrollx) {
#endif
			scr64[0] |= pixels << shiftleft;
			scr64[1] = pixels >> shiftright;
#ifdef __GNUC__
		}
		else
			scr64[1] = pixels;
#endif
#else
#ifdef __MSVC__
		__asm {
			push	edi

			mov	edi,scr64

			movq	mm0,[pixels]
			movq	mm4,[shiftleft]
			movq	mm1,mm0
			movq	mm5,[shiftright]

			psllq	mm0,mm4
			psrlq	mm1,mm5

			por	mm0,[edi]
			movq	[edi+8],mm1
			movq	[edi  ],mm0

			emms

			pop	edi
		}
		//end non-gcc code
#elif defined(__GNUC__)
#ifdef PS2
		__asm __volatile__(
			".align	8\n"
//			".set		noat\n"
			"ld		$0,0(%0)\n"

			"ld		$4,%2\n"
			"ld		$5,%3\n"

			"ld		$2,0(%1)\n"

			"dsrlv	$1,$0,$5\n"
			"dsllv	$0,$0,$4\n"

			"or		$0,$0,$2\n"
			"sd		$1,8(%1)\n"
			"sd		$0,0(%1)\n"
//			".set		at\n"
				:: "r"(&pixels),"r"(scr64),"m"(shiftleft),"m"(shiftright) : "memory"
			);
#else
		__asm __volatile__(
			"movq		%0,%%mm0\n"
			"movq		%%mm0,%%mm1\n"

			"movq		%2,%%mm4\n"
			"movq		%3,%%mm5\n"
			"psllq	%%mm4,%%mm0\n"
			"psrlq	%%mm5,%%mm1\n"

			"por		(%1),%%mm0\n"
			"movq		%%mm1,8(%1)\n"
			"movq		%%mm0,(%1)\n"

			"emms\n"
				:
				: "m"(pixels),"r"(scr64),"m"(shiftleft),"m"(shiftright)
				: "memory"
			);
#endif
#else
#error no background draw for this platform/compiler
#endif
#endif
		scr64++;
		//update scroll registers
		if((scroll & 0x1F) == 0x1F) {
			scroll &= ~0x1F;
			nametable ^= 1;
			nametableptr = nes->ppu.readpages[8 | nametable];
		}
		else
			scroll++;
	}

	//alternative sprite0 hit check
	//TODO: use the cache for sprite0 tile data
	if(nes->rom->sprite0hack) {
		u8 sprite0x = nes->sprmem[3];
		u8 sprite0y = nes->sprmem[0] + 1;
		int i,spritesize = 8;

		if(nes->ppu.ctrl0 & 0x20)
			spritesize = 16;
		if((int)sprite0y <= nes->scanline && 
			(int)(sprite0y + spritesize) > nes->scanline && 
			(nes->ppu.status & 0x40) == 0) {
			u8 *bg = dest + sprite0x + 8;
			int sprline = nes->scanline - sprite0y;
			u8 attrib = nes->sprmem[2];
			u8 tile = nes->sprmem[1];
			u8 *tileptr,b0,b1;
			u8 s0tileline[8];

			if(spritesize == 16) {
				tileptr = nes->ppu.readpages[(tile >> 6) | ((tile & 1) << 2)] + ((tile & 0x3e) << 4) + ((attrib & 0x80) >> 3);
				if(sprline >= 8) {
					if(attrib & 0x80)
						tileptr -= 16;
					else
						tileptr += 16;
				}
			}
			else
				tileptr = nes->ppu.readpages[(tile >> 6) | ((nes->ppu.ctrl0 & 8) >> 1)] + ((tile & 0x3f) << 4);
			sprline &= 7;
			if(attrib & 0x80 && attrib & 0x40) {
				tileptr += 7 - sprline;
				b0 = ((tileptr[0] >> 1) & 0x55) | (tileptr[8] & 0xaa);
				b1 = (tileptr[0] & 0x55) | ((tileptr[8] << 1) & 0xaa);
				s0tileline[7] = (b0 >> 6) & 3;
				s0tileline[6] = (b1 >> 6) & 3;
				s0tileline[5] = (b0 >> 4) & 3;
				s0tileline[4] = (b1 >> 4) & 3;
				s0tileline[3] = (b0 >> 2) & 3;
				s0tileline[2] = (b1 >> 2) & 3;
				s0tileline[1] = b0 & 3;
				s0tileline[0] = b1 & 3;
			}
			else if(attrib & 0x80) {
				tileptr += 7 - sprline;
				b0 = ((tileptr[0] >> 1) & 0x55) | (tileptr[8] & 0xaa);
				b1 = (tileptr[0] & 0x55) | ((tileptr[8] << 1) & 0xaa);
				s0tileline[0] = (b0 >> 6) & 3;
				s0tileline[1] = (b1 >> 6) & 3;
				s0tileline[2] = (b0 >> 4) & 3;
				s0tileline[3] = (b1 >> 4) & 3;
				s0tileline[4] = (b0 >> 2) & 3;
				s0tileline[5] = (b1 >> 2) & 3;
				s0tileline[6] = b0 & 3;
				s0tileline[7] = b1 & 3;
			}
			else if(attrib & 0x40) {
				tileptr += sprline;
				b0 = ((tileptr[0] >> 1) & 0x55) | (tileptr[8] & 0xaa);
				b1 = (tileptr[0] & 0x55) | ((tileptr[8] << 1) & 0xaa);
				s0tileline[7] = (b0 >> 6) & 3;
				s0tileline[6] = (b1 >> 6) & 3;
				s0tileline[5] = (b0 >> 4) & 3;
				s0tileline[4] = (b1 >> 4) & 3;
				s0tileline[3] = (b0 >> 2) & 3;
				s0tileline[2] = (b1 >> 2) & 3;
				s0tileline[1] = b0 & 3;
				s0tileline[0] = b1 & 3;
			}
			else {
				tileptr += sprline;
				b0 = ((tileptr[0] >> 1) & 0x55) | (tileptr[8] & 0xaa);
				b1 = (tileptr[0] & 0x55) | ((tileptr[8] << 1) & 0xaa);
				s0tileline[0] = (b0 >> 6) & 3;
				s0tileline[1] = (b1 >> 6) & 3;
				s0tileline[2] = (b0 >> 4) & 3;
				s0tileline[3] = (b1 >> 4) & 3;
				s0tileline[4] = (b0 >> 2) & 3;
				s0tileline[5] = (b1 >> 2) & 3;
				s0tileline[6] = b0 & 3;
				s0tileline[7] = b1 & 3;
			}
			for(i=0;i<8;i++) {
				if(s0tileline[i] && (bg[i] != 0)) {
					nes->ppu.status |= 0x40;
					break;
				}
			}
		}
		if((sprite0y + spritesize) < nes->scanline && (nes->ppu.status & 0x40) == 0)
			nes->ppu.status |= 0x40;
	}
}

typedef struct {
	u64 line;											//cache line data
	u8 attr;												//attrib bits
	u8 x;													//x coord
	u8 flags;											//flags
	} sprtemp_t;										//sprite temp entry

sprtemp_t sprtemp[256];

//process all sprites that belong to the next scanline
void ppu_getsprites()
{
	int i,h,sprinrange;
	u8 *s;

	//clear the sprite temp memory
	memset(sprtemp,0,sizeof(sprtemp_t) * 8);

	//determine sprite height
	h = 8 + ((nes->ppu.ctrl0 & 0x20) >> 2);

	//loop thru all 64 visible sprites, keeping note of the first eight visible
	for(sprinrange=0,i=0;i<64;i++) {
		int sprline;
		cache_t *cache;
		u8 patternbank;

		//sprite data pointer
		s = &nes->sprmem[i * 4];

		//get the sprite tile line to draw
		sprline = nes->scanline - s[0];

		//check for visibility on this line
		if(sprline >= 0 && sprline < h) {

			//if we have 8 sprites already, skip it
			if(sprinrange < 8) {

				//copy sprite data to temp memory
				sprtemp[sprinrange].attr = (s[2] & 3) | ((s[2] & 0x20) >> 3);
				sprtemp[sprinrange].x = s[3];
				sprtemp[sprinrange].flags = 1;

				//if sprite0 check is needed
				if((i == 0) && (nes->rom->sprite0hack == 0))
					sprtemp[sprinrange].flags = 2;

				//call mapper callback
				if(nes->mapper->tile && s[1] >= 0xFD && s[1] <= 0xFE)
					nes->mapper->tile(s[1],(nes->ppu.ctrl0 & 8) >> 3);

				//determine pattern bank used (upper or lower)
				if(nes->ppu.ctrl0 & 0x20)
					patternbank = (s[1] & 1) << 2;
				else
					patternbank = (nes->ppu.ctrl0 & 8) >> 1;

				//if sprite is to be flipped horizontally
				if(s[2] & 0x40)
					cache = (cache_t*)nes->ppu.cachepages_hflip[(s[1] >> 6) | patternbank];
				else
					cache = (cache_t*)nes->ppu.cachepages[(s[1] >> 6) | patternbank];

				//select tile for 8x16 sprites
				if(nes->ppu.ctrl0 & 0x20) {
					cache += ((s[1] & 0x3E) * 2) + ((s[2] & 0x80) >> 6);
					if(sprline >= 8) {
						if(s[2] & 0x80)
							cache -= 2;
						else
							cache += 2;
						sprline &= 7;
					}
				}

				//select tile for 8x8 sprites
				else
					cache += (s[1] & 0x3F) * 2;

				//if sprite is to be flipped vertically
				if((s[2] & 0x80) != 0)
					sprline = 7 - sprline;

				//select tile line
				cache += (sprline / 4);

				//store sprite tile line
				sprtemp[sprinrange].line = *cache >> ((sprline & 3) * 2);

				//increment sprite in range counter
				sprinrange++;
			}

			//8+ sprites are visible, set the flag and exit the loop
			else {
				nes->ppu.status |= 0x20;
				break;
			}
		}
	}
}

static void drawspriteline(u8 *dest)
{
	u8 spriteline[256 + 8];
	sprtemp_t *spr = (sprtemp_t*)sprtemp + 7;
	u64 *spriteline64 = (u64*)spriteline;
	int n;

	dest += 8;

	//clear sprite line
	for(n=0;n<(256 / 8);n++)
		spriteline64[n] = 0x10;

	//loop thru all eight possible sprites
	for(n=0;n<8;n++,spr--) {
		cache_t offs,mask = CACHE_MASK;
		u64 *scr64,sp0,sp1,pmask0,pmask1,pmaskxor = 0xFFFFFFFFFFFFFFFFLL;
		u64 shiftright = (8 - (spr->x & 7)) * 8;
		u64 shiftleft = (spr->x & 7) * 8;

		if(spr->flags == 0)
			continue;

		scr64 = ((u64*)spriteline) + (spr->x / 8);

		//draw the pixel
		offs = spr->attr * 0x0404040404040404LL;
		sp0 = sp1 = spr->line & mask;
		pmask0 = pmask1 = (0x8080808080808080LL - sp0) >> 2;
#ifdef C_RENDER
#ifdef __GNUC__
		if(spr->x & 7) {
#endif
			sp0 <<= shiftleft;
			sp1 >>= shiftright;
			pmask0 <<= shiftleft;
			pmask1 >>= shiftright;
			scr64[0] = ((sp0 + offs) & pmask0) | (scr64[0] & ~pmask0);
			scr64[1] = ((sp1 + offs) & pmask1) | (scr64[1] & ~pmask1);
#ifdef __GNUC__
		}
		else
			scr64[0] = ((sp0 + offs) & pmask0) | (scr64[0] & ~pmask0);
#endif
#else
#ifdef __MSVC__
		__asm {
			//init screen variable
			push	edi
			mov	edi,scr64

			//init our variables
			movq	mm0,[sp0]
			movq	mm1,[sp1]
			movq	mm2,[pmask0]
			movq	mm3,[pmask1]
			movq	mm4,[shiftleft]
			movq	mm5,[shiftright]
			movq	mm6,[offs]
			movq	mm7,[pmaskxor]

			//shift out the pixels between the two qwords
			psllq	mm0,mm4
			psrlq	mm1,mm5
			psllq	mm2,mm4
			psrlq	mm3,mm5

			//add the attribute bytes
			paddq	mm0,mm6
			paddq	mm1,mm6

			//mask with pixel mask
			pand	mm0,mm2
			pand	mm1,mm3

			//invert mask to mask the screen pixels
			pxor	mm2,mm7
			pxor	mm3,mm7

			//get the old screen pixels
			movq	mm6,[edi  ]
			movq	mm7,[edi+8]

			//and them with the inverted pixel mask
			pand	mm6,mm2
			pand	mm7,mm3

			//merge the old pixels with new pixels
			por	mm0,mm6
			por	mm1,mm7

			//draw pixels to buffer
			movq	[edi  ],mm0
			movq	[edi+8],mm1

			//done
			emms
			pop	edi
		}
		//end non-gcc code
#elif defined(__GNUC__)
#ifdef PS2
//#error no background draw for this platform
#else
		__asm__ __volatile__(
			//init our variables
			"movq	%1,%%mm0\n"
			"movq	%2,%%mm1\n"
			"movq	%3,%%mm2\n"
			"movq	%4,%%mm3\n"
			"movq	%5,%%mm4\n"
			"movq	%6,%%mm5\n"
			"movq	%7,%%mm6\n"
			"movq	%8,%%mm7\n"

			//shift out the pixels between the two qwords
			"psllq	%%mm4,%%mm0\n"
			"psrlq	%%mm5,%%mm1\n"
			"psllq	%%mm4,%%mm2\n"
			"psrlq	%%mm5,%%mm3\n"

			//add the attribute bytes
			"paddq	%%mm6,%%mm0\n"
			"paddq	%%mm6,%%mm1\n"

			//mask with pixel mask
			"pand	%%mm2,%%mm0\n"
			"pand	%%mm3,%%mm1\n"

			//invert mask to mask the screen pixels
			"pxor	%%mm7,%%mm2\n"
			"pxor	%%mm7,%%mm3\n"

			//get the old screen pixels
			"movq	(%0),%%mm6\n"
			"movq	8(%0),%%mm7\n"

			//and them with the inverted pixel mask
			"pand	%%mm2,%%mm6\n"
			"pand	%%mm3,%%mm7\n"

			//merge the old pixels with new pixels
			"por	%%mm6,%%mm0\n"
			"por	%%mm7,%%mm1\n"

			//draw pixels to buffer
			"movq	%%mm0,(%0)\n"
			"movq	%%mm1,8(%0)\n"

			//done
			"emms\n"
				:
				: "r"(scr64),
					"m"(sp0),"m"(sp1),"m"(pmask0),"m"(pmask1),
					"m"(shiftleft),"m"(shiftright),"m"(offs),"m"(pmaskxor)
				: "memory"
			);
#endif
#else
#error no background draw for this platform
#endif
#endif
		//check for sprite0 hit
		if((spr->flags & 2) && (nes->ppu.ctrl1 & 8) && (spr->x >= (((nes->ppu.ctrl1 ^ 4) & 4) << 1))) {
			u8 *sprite = spriteline + spr->x;
			u8 *bg = dest + spr->x;
			int j;

			for(j=0;j<8 && (spr->x + j) < 255;j++) {
				if((sprite[j] & 3) && ((bg[j] & 0xF) != 0)) {
					nes->ppu.status |= 0x40;
					spr->flags = 0;
					break;
				}
			}
		}
	}

	//clear left 8 background pixels if needed
	if((nes->ppu.ctrl1 & 2) == 0)
		*(((u64*)dest) + 0) = 0;

	//draw the sprite line to the buffer
	for(n=(((nes->ppu.ctrl1 ^ 4) & 4) << 1);n<256;n++) {
		u8 pixel = spriteline[n];

		if(pixel & 3) {
			if((pixel & 0x10) == 0)    //foreground sprite
				dest[n] = pixel | 0x10;
			else if((dest[n] & 3) == 0)      //background sprite that is visible
				dest[n] = pixel | 0x10;
		}
	}
}

//draw visible scanlines (lines 21-260)
void ppu_drawline(u8 *dest)
{
	//if background is visible, draw the line
	if(nes->ppu.ctrl1 & 0x08)
		drawbackground(dest);

	//else clear the line
	else {
		int i;

		for(i=0;i<((256+8)/8);i++)
			*(((u64*)dest) + i) = 0;
	}

	//render sprites if they are visible
	if(nes->ppu.ctrl1 & 0x10) {
		drawspriteline(dest);

		//refresh sprites for next line
//		ppu_getsprites();
	}
}
