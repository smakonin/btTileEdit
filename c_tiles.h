/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_TILES_H
#define C_TILES_H

#include "basedefs.h"
#include "c_video.h"
#include "c_filelib.h"

class c_tiles : public c_image
{
public:
	c_filelib filelib;
	long tile_len;
	long tile_size;
	long tiles_per_len;
	long max_tiles;
	long tile_view_width;
	long tile_view_height;
	long tile_view_pwidth;
	long tile_view_pheight;

	void inline create(char *filename, long screen_width, long screen_height)
	{
		char *filename2;
		long size;

		
		if(strstr(filename, ".pcx"))
		{
			filename2 = strdup(filename);
			strcpy(strstr(filename2, ".pcx"), ".walk");

			load_pcx(filename);
			recalc_dims(screen_width, screen_height);

			size = max_tiles;
		}
		else
		{
			filename2 = strdup(filename);
			strcpy(strstr(filename2, "_p"), "_b");
			
			load_gfat(filename);
			recalc_dims(screen_width, screen_height);

			size = max_tiles;
		}

		free(filename2);
	}
	
	void inline create(HINSTANCE hinst, long res_id, long screen_width, long screen_height)
	{
		load_res(hinst, res_id);
		recalc_dims(screen_width, screen_height);
	}

	void inline recalc_dims(long screen_width, long screen_height)
	{
		tile_len = 20;
		tile_size = tile_len * tile_len;
		tiles_per_len = width / 20;
		max_tiles = tiles_per_len * tiles_per_len;
		tile_view_width = screen_width / tile_len;
		tile_view_height = screen_height / tile_len;
		tile_view_pwidth = tile_view_width * 20;
		tile_view_pheight = tile_view_height * 20;
	}
	
	void inline remove()
	{
		c_image::remove();
	}

	void inline save(char *filename)
	{
		char *filename2;

		filename2 = strdup(filename);
		strcpy(strstr(filename2, "."), ".walk");

		save_pcx(filename);
	}

	void inline put_pixel(unsigned short tile_no, long x, long y, unsigned char colour)
	{
		unsigned char sx;
		unsigned char sy;

		break_tile_no(tile_no, &sx, &sy);
		x += sx * tile_len;
		y += sy * tile_len;

		buffer[x + y * width] = colour;
	}
	
	unsigned char inline get_pixel(unsigned short tile_no, long x, long y)
	{
		unsigned char sx;
		unsigned char sy;

		break_tile_no(tile_no, &sx, &sy);
		x += sx * tile_len;
		y += sy * tile_len;

		return buffer[x + y * width];
	}

    void inline place(long x, long y, c_image *target, unsigned short tile_no, unsigned char zoom_factor, long use_transparency, long darken = 0)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;
		unsigned char zx;
		unsigned char zy;
		unsigned char maxinslide;
		unsigned char mininslide;
		unsigned char colour;

		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
		{
			for(zy = 0; zy < zoom_factor; zy++)
			{
				for(col = 0; col < tile_len; col++)
				{
					for(zx = 0; zx < zoom_factor; zx++)
					{
						colour = buffer[(sx + row) + (sy + col) * width];
						if((colour != transparency && use_transparency) || !use_transparency)
						{
							if(colour != transparency)
							{
								mininslide = (colour / 16) * 16;
								maxinslide = ((colour + 16) / 16) * 16 - 1;
								colour = (unsigned char)LIMIT(colour + darken, mininslide, maxinslide);
							}
							target->buffer[(x + (col * zoom_factor) + zx) + (y + (row * zoom_factor) + zy) * target->width] = colour;
						}
					}
				}
			}
		}
	}

    void inline place(long x, long y, c_video *target, unsigned short tile_no, unsigned char zoom_factor, long use_transparency, long darken = 0)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;
		long tx;
		long ty;
		unsigned char zx;
		unsigned char zy;
		unsigned char maxinslide;
		unsigned char mininslide;
		unsigned char colour;

		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
		{
			for(zy = 0; zy < zoom_factor; zy++)
			{
				for(col = 0; col < tile_len; col++)
				{
					for(zx = 0; zx < zoom_factor; zx++)
					{
						colour = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];
						if((colour != transparency && use_transparency) || !use_transparency)
						{
							if(colour != transparency)
							{
								mininslide = (colour / 16) * 16;
								maxinslide = ((colour + 16) / 16) * 16 - 1;
								colour = (unsigned char)LIMIT(colour + darken, mininslide, maxinslide);
							}
							
							tx = (x + (col * zoom_factor) + zx);
							ty = (y + (row * zoom_factor) + zy);
							
							if(IN_LIMIT(tx, 0, tile_view_width*tile_len) && IN_LIMIT(ty, 0, tile_view_height*tile_len))
								target->buffer[tx + ty * target->width] = colour;
						}
					}
				}
			}
		}
	}

	void copy(unsigned short source_tile_no, unsigned short target_tile_no, long use_transparency)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;
		unsigned char tx;
		unsigned char ty;

		break_tile_no(source_tile_no, &sx, &sy);
		break_tile_no(target_tile_no, &tx, &ty);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				if(!use_transparency  || (use_transparency && buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] != transparency))
					buffer[((tx * tile_len) + col) + ((ty * tile_len) + row) * width] = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];
	}

	void clear(unsigned short tile_no, unsigned char colour)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;

		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] = colour;
	}
	
	void relpace_colour(unsigned short tile_no, unsigned char source_colour, unsigned char target_colour)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;

		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				if(buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] == source_colour)
					buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] = target_colour;
	}

	void vflip(unsigned short tile_no)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;
		unsigned char *temp_tile;

		temp_tile = (unsigned char *)malloc(tile_size);
		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				temp_tile[col + (tile_len - 1 - row) * tile_len] = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] = temp_tile[col + row * tile_len];

		free(temp_tile);
	}

	void hflip(unsigned short tile_no)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;
		unsigned char *temp_tile;

		temp_tile = (unsigned char *)malloc(tile_size);
		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				temp_tile[(tile_len - 1 - col) + row * tile_len] = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] = temp_tile[col + row * tile_len];

		free(temp_tile);
	}

	void slide(unsigned short tile_no, char x, char y)
	{
		long row;
		long col;
		long tmp;
		unsigned char sx;
		unsigned char sy;
		unsigned char *temp_tile;

		temp_tile = (unsigned char *)malloc(tile_size);
		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				temp_tile[(tile_len - 1 - col) + row * tile_len] = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];

		if(x == -1)
			tmp = 1;
		else if(x == 1)
			tmp = tile_len - 1;
		else
			tmp = 0;

		for(row = 0; row < tile_len; row++)
		{
			for(col = 0; col < tile_len; col++, tmp++)
			{
				if(tmp == -1)
					tmp = tile_len - 1;
				else if(tmp == tile_len)
					tmp = 0;

				buffer[((sx * tile_len) + tmp) + ((sy * tile_len) + row) * width] = temp_tile[col + row * tile_len];
			}
		}
			
		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				temp_tile[(tile_len - 1 - col) + row * tile_len] = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];

		if(y == -1)
			tmp = 1;
		else if(y == 1)
			tmp = tile_len - 1;
		else
			tmp = 0;
		
		for(row = 0; row < tile_len; row++, tmp++)
		{
			if(tmp == -1)
				tmp = tile_len - 1;
			else if(tmp == tile_len)
				tmp = 0;

			for(col = 0; col < tile_len; col++)
			{
				buffer[((sx * tile_len) + col) + ((sy * tile_len) + tmp) * width] = temp_tile[col + row * tile_len];
			}
		}

		free(temp_tile);
	}

	void rotate(unsigned short tile_no)
	{
		long row;
		long col;
		unsigned char sx;
		unsigned char sy;
		unsigned char *temp_tile;

		temp_tile = (unsigned char *)malloc(tile_size);
		break_tile_no(tile_no, &sx, &sy);

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				temp_tile[row + (tile_len - 1 - col) * tile_len] = buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width];

		for(row = 0; row < tile_len; row++)
			for(col = 0; col < tile_len; col++)
				buffer[((sx * tile_len) + col) + ((sy * tile_len) + row) * width] = temp_tile[col + row * tile_len];

		free(temp_tile);
	}

	unsigned short inline make_tile_no(unsigned char x, unsigned char y)
	{
		return (unsigned short)(y * tiles_per_len) + x;
	}

	void inline break_tile_no(unsigned short tile_no, unsigned char *x, unsigned char *y)
	{
		*y = (unsigned char)(tile_no / tiles_per_len);
		*x = (unsigned char)(tile_no % tiles_per_len);
	}
};

#endif //C_TILES_H