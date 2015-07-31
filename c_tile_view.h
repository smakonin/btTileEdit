/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_TILE_VIEW_H
#define C_TILE_VIEW_H

#include "c_video.h"
#include "c_tiles.h"

class c_tile_view
{
public:
	c_video *video;
	c_tiles *tiles;
	long corner_x;
	long corner_y;
	unsigned char sel_x;
	unsigned char sel_y;
	unsigned char sel_x2;
	unsigned char sel_y2;
	unsigned short sel_tile_no;
	long copied_width;
	long copied_height;
	unsigned short copied[32][24][2];
	long use_map_copy;
	long exact_copy;

	c_tile_view()
	{
		video = NULL;
		tiles = NULL;
		corner_x = 0;
		corner_y = 0;
		sel_x = 0;
		sel_y = 0;
		sel_x2 = 0;
		sel_y2 = 0;
		sel_tile_no = 0;
		use_map_copy = FALSE;
		exact_copy = TRUE;
		copied_width = 0;
		copied_height = 0;
		memset(copied, 0, sizeof(copied));		
	}

	void inline create(c_video *video, c_tiles *tiles)
	{
		c_tile_view::video = video;
		c_tile_view::tiles = tiles;
	}

	void inline render(long special_switch = 0)
	{
		long i, j, k, l;
		unsigned short tile_no;
		long darken;

		darken = (special_switch == 'w') ? 8 : 0;

		for(i = 0; i < video->height; i += tiles->tile_len)
		{
			for(j = 0; j < video->width; j += tiles->tile_len)
			{
				k = ((corner_x * tiles->tile_len) + j) / tiles->tile_len;
				l = ((corner_y * tiles->tile_len) + i) / tiles->tile_len;
				tile_no = tiles->make_tile_no((unsigned char)(k), (unsigned char)(l));
				tiles->place(j, i, video, tile_no, 1, FALSE);
			}
		}
	}

	void inline select(long x, long y, long x2 = 0, long y2 = 0)
	{
		sel_x = (unsigned char)(corner_x + x);
		sel_y = (unsigned char)(corner_y + y);
		sel_x2 = (x2) ? (unsigned char)(corner_x + x2) :sel_x;
		sel_y2 = (y2) ? (unsigned char)(corner_y + y2) :sel_y;
		sel_tile_no = tiles->make_tile_no(sel_x, sel_y);
		use_map_copy = FALSE;
	}
};

#endif //C_TILE_VIEW_H