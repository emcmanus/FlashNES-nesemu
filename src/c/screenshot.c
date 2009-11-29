#include <stdio.h>
#include <stdlib.h>
#include "system/system.h"
#ifdef USE_PNG
#include "png.h"

///TODO: fix all this, not very important...

static void
png_cexcept_error(png_structp png_ptr, png_const_charp msg)
{
   if(png_ptr)
     ;
   fprintf(stderr, "libpng error: %s\n", msg);
   {
//      Throw msg;
   }
}

int PngSaveImage (char *pstrFileName, png_byte *pDiData,
                   int iWidth, int iHeight, png_color bkgColor)
{
    const int           ciBitDepth = 8;
    const int           ciChannels = 3;

	static png_structp png_ptr = NULL;
	static png_infop info_ptr = NULL;
    static FILE        *pfFile;
    png_uint_32         ulRowBytes;
    static png_byte   **ppbRowPointers = NULL;
    int                 i;

    // open the PNG output file

    if (!pstrFileName)
        return 0;

    if (!(pfFile = fopen(pstrFileName, "wb")))
        return 0;

    // prepare the standard PNG structures

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
      (png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
    if (!png_ptr)
    {
        fclose(pfFile);
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(pfFile);
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        return 0;
    }

//    Try
//    {
        // initialize the png structure

#if !defined(PNG_NO_STDIO)
        png_init_io(png_ptr, pfFile);
#else
        png_set_write_fn(png_ptr, (png_voidp)pfFile, png_write_data, png_flush);
#endif

        // we're going to write a very simple 3x8 bit RGB image

        png_set_IHDR(png_ptr, info_ptr, iWidth, iHeight, ciBitDepth,
            PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
            PNG_FILTER_TYPE_BASE);

        // write the file header information

        png_write_info(png_ptr, info_ptr);

        // swap the BGR pixels in the DiData structure to RGB

        png_set_bgr(png_ptr);

        // row_bytes is the width x number of channels

        ulRowBytes = iWidth * ciChannels;

        // we can allocate memory for an array of row-pointers

        if ((ppbRowPointers = (png_bytepp) malloc(iHeight * sizeof(png_bytep))) == NULL)
            log_error( "Visualpng: Out of memory" );

        // set the individual row-pointers to point at the correct offsets

        for (i = 0; i < iHeight; i++)
            ppbRowPointers[i] = pDiData + i * (((ulRowBytes + 3) >> 2) << 2);

        // write out the entire image data in one call

        png_write_image (png_ptr, ppbRowPointers);

        // write the additional chunks to the PNG file (not really needed)

        png_write_end(png_ptr, info_ptr);

        // and we're done

        free (ppbRowPointers);
        ppbRowPointers = NULL;

        // clean up after the write, and free any memory allocated

        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

        // yepp, done
/*    }

    Catch (msg)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp) NULL);

        if(ppbRowPointers)
            free (ppbRowPointers);

        fclose(pfFile);

        return 0;
    }
  */
    fclose (pfFile);

    return 1;
}
#endif

typedef struct
	{
	u8 Manufacturer;
	u8 Version;
	u8 Encoding;
	u8 BitsPerPixel;
	u16 Xmin;
	u16 Ymin;
	u16 Xmax;
	u16 Ymax;
	u16 HDpi;
	u16 VDpi;
	u8 Colormap[48];
	u8 Reserved;
	u8 NPlanes;
	u16 BytesPerLine;
	u16 PaletteInfo;
	u16 HscreenSize;
	u16 VscreenSize;
	u8 Filler[54];
	} PCXHEADER;
typedef struct
	{
	u8 r,g,b;
	} PCXPALETTEENTRY;
typedef struct
	{
	u8 key; //must be 12
	PCXPALETTEENTRY p[256];
	} PCXPALETTE;

int savepcx(char *fn)
{
	PCXHEADER ph;
	u8 pd[256 * 224];
	PCXPALETTE pp;
	int i,j,fd;

	ph.Manufacturer = 10;
	ph.Version = 5;
	ph.Encoding = 1; //hmm, psp dont like it if you dont encode and set encoding to 0
	ph.BitsPerPixel = 8;
	ph.Xmin = 0;
	ph.Ymin = 0;
	ph.Xmax = 256-1;
	ph.Ymax = 224-1;
	ph.HDpi = 0;
	ph.VDpi = 0;
	memset(ph.Colormap,0,48);
	ph.Reserved = 0;
	ph.NPlanes = 1;
	ph.BytesPerLine = 256;
	ph.PaletteInfo = 1;
	ph.HscreenSize = 0;
	ph.VscreenSize = 0;
	memset(ph.Filler,0,54);
	for(i=0;i<256;i++) {
		pp.p[i].r = (video_getpalette(i) >> 16) & 0xFF;
		pp.p[i].g = (video_getpalette(i) >> 8) & 0xFF;
		pp.p[i].b = (video_getpalette(i) >> 0) & 0xFF;
	}
	for(i=0;i<224;i++) {
		u8 *sline = video_getnesscreen() + ((i + 8) * 256);
		u8 *bline = pd + (i * 256);

		for(j=0;j<256;j++)
			*bline++ = *sline++ & 0x1F;
	}
	pp.key = 12;
	if((fd = file_open(fn,"uwb")) >= 0) {
		file_write(fd,&ph,sizeof(PCXHEADER));
		file_write(fd,&pd,256 * 224);
		file_write(fd,&pp,sizeof(PCXPALETTE));
		file_close(fd);
	}
	return(0);
}

void screenshot()
{
	char filename[512];
	char shotext[8];
	int i,j,fd,shotnum = 0;
#ifdef USE_PNG
	png_color color0;
#endif
	u8 *d,*p;

	if(nes == 0 || nes->rom == 0)
		return;
	switch(config.shotformat) {
		default:
		case 0:
			strcpy(shotext,"pcx");
			break;
#ifdef USE_PNG
		case 1:
			strcpy(shotext,"png");
			break;
#endif
	}
	for(;;) {
		sprintf(filename,"%s/shot%04d.%s",config.path_shots,shotnum,shotext);
		if((fd = file_open(filename,"rb")) < 0) { //dont exist
			file_close(fd);
			break;
		}
		file_close(fd);
		shotnum++;
		if(shotnum > 9999) {
			log_warning("there is already 9999 screenshots in this directory, please pick another one in the configuration file.");
			return;
		}
	}
	switch(config.shotformat) {
		default:
		case 0:
			log_message("saving pcx...\n");
			savepcx(filename);
			break;
#ifdef USE_PNG
		case 1:
			//must be converted to 24 bit before saving
			p = d = (u8*)malloc(256*240*sizeof(u32));
			for(j=0;j<224;j++) {
				for(i=0;i<256;i++) {
					u32 pixel = *(video_getscreen() + i + (j + 8) * 256);
					*p++ = pixel >> 0 & 0xFF;
					*p++ = pixel >> 8 & 0xFF;
					*p++ = pixel >> 16 & 0xFF;
				}
			}
			color0.red = (video_getpalette(0) >> 16) & 0xFF;
			color0.green = (video_getpalette(0) >> 8) & 0xFF;
			color0.blue = (video_getpalette(0) >> 0) & 0xFF;
			PngSaveImage(filename,d,256,224,color0);
			free(d);
			break;
#endif
	}
	log_message("screenshot saved: %s\n",filename);
}
