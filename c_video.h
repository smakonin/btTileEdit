/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_VIDEO_H
#define C_VIDEO_H

#include <ddraw.h>
#include "c_image.h"

class c_video : public c_image
{
public:
	HWND hwnd;
	HINSTANCE hinst;
	long refresh_rate;
	long max_colours;
	LPDIRECTDRAW7 direct_draw;
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 display_mem;

	c_video();
	~c_video();
	void create(long width, long height, char *title, HINSTANCE hinst);
	void remove();
    void flip();
	void text_out(long x, long y, long colour, long transparent_bk, char align, char *text, ...);
	void clear(long colour);
	void clear_buffer(unsigned char colour);
};

#endif //C_VIDEO_H