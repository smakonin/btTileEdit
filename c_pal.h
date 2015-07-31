/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_PAL_H
#define C_PAL_H

#include <ddraw.h>

class c_colour
{
public:
     unsigned char r, g, b;
};

class c_pal
{
protected:
    PALETTEENTRY cur_pal[256];
	LPDIRECTDRAWPALETTE direct_palette;
	void create(LPDIRECTDRAW7 direct_draw);
	void set(LPDIRECTDRAWSURFACE7 buffer);
	void remove();

public:
    c_colour master_pal[256];

	c_pal();
	~c_pal();
    void fade_colour(long fade_in, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
	void activate_colour(long val = -1, long sav2mastr = FALSE);
	void activate_colour(c_colour *pal);
};

#endif //C_PAL_H