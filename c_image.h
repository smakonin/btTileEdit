/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_IMAGE_H
#define C_IMAGE_H

#include <windows.h>
#include "c_gfat.h"
#include "c_filelib.h"
#include "c_pal.h"
#include "c_rle.h"

class c_pcx_hdr
{
public:
	unsigned char manufacturer;
	unsigned char version;
	unsigned char encoding;
	unsigned char bits_per_pixel;
	unsigned short x1;
	unsigned short y1;
	unsigned short x2;
	unsigned short y2;
	unsigned short width;
	unsigned short height;
	unsigned char ega_palette[48];
	unsigned char reserved;
	unsigned char num_color_planes;
	unsigned short bytes_per_line;
	unsigned short palette_type;
	unsigned short hscreen_size;
	unsigned short vscreen_size;
	unsigned char padding[54];

	void create(long width, long height)
	{
		manufacturer = 0x0A;
		version = 5;
		encoding = 1;
		bits_per_pixel = 8;
		x1 = 0;
		y2 = 0;
		x2 = (unsigned short)width - 1;
		y2 = (unsigned short)height - 1;
		width = (unsigned short)width;
		height = (unsigned short)height;			
		memset(ega_palette, 0, sizeof(ega_palette));
		reserved = 0;
		num_color_planes = 1;
		bytes_per_line = (unsigned short)width;
		palette_type = 1;
		hscreen_size = 0;
		vscreen_size = 0;
		memset(padding, 0, sizeof(padding));
	}
};

class c_image : public c_pal
{
protected:
	c_rle rle;
	c_gfat gfat;

public:
	c_pcx_hdr header;
	unsigned char *buffer;
    long width;
    long height;
	long size;
	RECT rect;
	unsigned char transparency;
    
    c_image()
	{
		buffer = NULL;
		width = 0;
		height = 0;
		size = 0;
		transparency = 254;
	}

	~c_image()
	{
		remove();
	}

	void create(long width, long height)
	{
		c_image::width = width;
		c_image::height = height;
		size = width * height;
		SetRect(&rect, 0, 0, width, height);
		remove();
		buffer = (unsigned char *)malloc(size);
		memset(buffer, 0, size);	  
	}

	void remove()
	{
		if(buffer)
			free(buffer);
		buffer = NULL;
	}

	void load_pcx(char *filename, FILE *fp = NULL)
	{
		long is_ext, index;

		is_ext = (long)fp;
		if(!is_ext)
			if((fp = fopen(filename, "rb")) == NULL)
				throw c_error("Unable to open %s", filename);
   
		fread(&header, sizeof(header), 1, fp);
		create(header.x2 + 1, header.y2 + 1);
		rle.decode_file(buffer, size, fp);

		fseek(fp, -768L, SEEK_END);
		for(index = 0; index < 256; index++)
		{
		  master_pal[index].r = fgetc(fp);
		  master_pal[index].g = fgetc(fp);
		  master_pal[index].b = fgetc(fp);
		}

		if(!is_ext)
			fclose(fp);  
	}

	void save_pcx(char *filename)
	{
		FILE *fp;

		if((fp = fopen(filename, "wb")) == NULL)
			throw c_error("Unable to write to %s", filename);

		fwrite(&header, sizeof(header), 1, fp);
		rle.encode_file(buffer, width, height, fp);
		putc(0x0C, fp);
		fwrite(master_pal, sizeof(c_colour), 256, fp);

		fclose(fp);            
	}

	void load_res(HINSTANCE hinst, long res_id)
	{
		HGLOBAL res_gptr;
		unsigned char *res_ptr;
		DWORD res_size;
		HRSRC h_res;
		FILE *fp;
		char *filename = "temp file";

		h_res = FindResource(hinst, MAKEINTRESOURCE(res_id), "pcx");
		if(!(res_size = SizeofResource(hinst, h_res)))
			throw c_error();
		if(!(res_gptr = LoadResource(hinst, h_res)))
			throw c_error();
		if(!(res_ptr = (unsigned char *)LockResource(res_gptr)))
			throw c_error();

		if((fp = tmpfile()) == NULL)
			 throw c_error("Unable to create temp files");

		fwrite(res_ptr, res_size, 1, fp);
		fseek(fp, 0, SEEK_SET);

		load_pcx(filename, fp);

		_rmtmp();
	}
	
	void load_gfat(char *filename)
	{
		unsigned char *buf;
		long size;
		FILE *fp;

		gfat.open(filename);
		gfat.read(&buf, &size);
		gfat.close();

		if((fp = tmpfile()) == NULL)
			 throw c_error("Unable to create temp files");

		fwrite(buf, size, 1, fp);
		fseek(fp, 0, SEEK_SET);
		free(buf);

		load_pcx(filename, fp);
		_rmtmp();
	}

	void inline put_pixel(long x, long y, unsigned char colour)
	{
		buffer[x + y * width] = colour;
	}
	
	unsigned char inline get_pixel(long x, long y)
	{
		return buffer[x + y * width];
	}

	void inline clear(unsigned char colour)
	{
		memset(buffer, colour, size);
	}

	void inline put_image(long x, long y, c_image *source, long use_transparency = false)
	{
		long row, col;

		if(!use_transparency)
		{
			for(row = 0; row < source->height; row++)
				memcpy(&buffer[x + (y + row) * width], &source->buffer[row * source->width], source->width);	
		}
		else
		{
			for(row = 0; row < source->height; row++)
				for(col = 0; col < source->width; col++)
					if(source->transparency != source->buffer[col + row * source->width])
						if(x + col > -1 && x + col < width && y + row > -1 && y + row < height)
							buffer[x + col + (y + row) * width] = source->buffer[col + row * source->width];
		}
	}

	void inline draw_hline(long x1, long x2, long y, unsigned char colour)
	{
		memset(&buffer[x1 + y * width], colour, x2 - x1 + 1);
	}

	void inline draw_vline(long x, long y1, long y2, unsigned char colour)
	{
		unsigned char *memptr;
		long num;

		memptr = &buffer[x + y1 * width];
		num = y2 - y1 +1;

		while(num--)
		{
			*memptr = colour;
			memptr += width;
		}
	}

	void inline draw_box(long x1, long y1, long x2, long y2, unsigned char colour)	
	{
		draw_hline(x1, x2, y1, colour);
		draw_vline(x2, y1, y2, colour);
		draw_hline(x1, x2, y2, colour);
		draw_vline(x1, y1, y2, colour);
	}
};

#endif //C_IMAGE_H