//---------------------------------------------------------------------------
// File:	g2.h
// Author:	Tony Saveski, t_saveski@yahoo.com
// Notes:	Simple 'High Level' 2D Graphics Library
//---------------------------------------------------------------------------
#ifndef G2_H
#define G2_H

#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	 PAL_256_256_32=0
	,PAL_320_256_32
	,PAL_384_256_32
	,PAL_512_256_32
	,PAL_640_256_32

	,NTSC_256_224_32
	,NTSC_320_224_32
	,NTSC_384_224_32
	,NTSC_512_224_32
	,NTSC_640_224_32
} g2_video_mode;

extern int g2_init(g2_video_mode mode);
extern void g2_end(void);

extern u16 g2_get_max_x(void);
extern u16 g2_get_max_y(void);

extern void g2_set_color(u8 r, u8 g, u8 b);
extern void g2_set_fill_color(u8 r, u8 g, u8 b);
extern void g2_get_color(u8 *r, u8 *g, u8 *b);
extern void g2_get_fill_color(u8 *r, u8 *g, u8 *b);

extern void g2_put_pixel(s16 x, s16 y);
extern void g2_line(s16 x0, s16 y0, s16 x1, s16 y1);
extern void g2_rect(s16 x0, s16 y0, s16 x1, s16 y1);
extern void g2_fill_rect(s16 x0, s16 y0, s16 x1, s16 y1);
extern void g2_put_image(s16 x, s16 y, s16 w, s16 h, u32 *data);

extern void g2_set_viewport(u16 x0, u16 y0, u16 x1, u16 y1);
extern void g2_get_viewport(u16 *x0, u16 *y0, u16 *x1, u16 *y1);

extern void g2_set_visible_frame(u8 frame);
extern void g2_set_active_frame(u8 frame);
extern u8 g2_get_visible_frame(void);
extern u8 g2_get_active_frame(void);

extern void g2_wait_vsync(void);
extern void g2_wait_hsync(void);

extern void g2_set_font(u32 *data, u16 w, u16 h, u16 *tc);
extern void g2_out_text(s16 x, s16 y, char *str);
extern void g2_set_font_spacing(u16 s);
extern u16 g2_get_font_spacing(void);
extern void g2_set_font_mag(u16 m);
extern u16 g2_get_font_mag(void);
extern void g2_set_font_color(u8 red, u8 green, u8 blue);

#ifdef __cplusplus
}
#endif

#endif // G2_H

