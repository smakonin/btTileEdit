/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#include <stdio.h>
#include "dxdefs.h"
#include "c_error.h"
#include "c_pal.h"

c_pal::c_pal()
{
	direct_palette = NULL;
	activate_colour(-2);
}

c_pal::~c_pal()
{
	remove();
}

void c_pal::fade_colour(long fade_in, unsigned char r, unsigned char g, unsigned char b)
{
	long index;
	long incro;
	
	if(fade_in)
	{
		for(incro = 0; incro < 256; incro++)
		{
			for(index = 0; index < 256; index++)
			{
				if(cur_pal[index].peRed < master_pal[index].r)
					cur_pal[index].peRed++;
				else if(cur_pal[index].peRed > master_pal[index].r)
					cur_pal[index].peRed--;

				if(cur_pal[index].peGreen < master_pal[index].g)
					cur_pal[index].peGreen++;
				else if(cur_pal[index].peGreen > master_pal[index].g)
					cur_pal[index].peGreen--;

				if(cur_pal[index].peBlue < master_pal[index].b)
					cur_pal[index].peBlue++;
				else if(cur_pal[index].peBlue > master_pal[index].b)
					cur_pal[index].peBlue--;

				cur_pal[index].peFlags = NULL;
			}
			direct_palette->SetEntries(0, 0, 256, cur_pal);
			Sleep(5);
		}
	}
	else
	{
		for(incro = 0; incro < 256; incro++)
		{
			for(index = 0; index < 256; index++)
			{
				if(cur_pal[index].peRed < r)
					cur_pal[index].peRed++;
				else if(cur_pal[index].peRed > r)
					cur_pal[index].peRed--;

				if(cur_pal[index].peGreen < g)
					cur_pal[index].peGreen++;
				else if(cur_pal[index].peGreen > g)
					cur_pal[index].peGreen--;

				if(cur_pal[index].peBlue < b)
					cur_pal[index].peBlue++;
				else if(cur_pal[index].peBlue > b)
					cur_pal[index].peBlue--;

				cur_pal[index].peFlags = NULL;
			}
			direct_palette->SetEntries(0, 0, 256, cur_pal);	
			Sleep(5);
		}
	}

}

void c_pal::create(LPDIRECTDRAW7 direct_draw)
{
	if(FAILED(direct_draw->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE, cur_pal, &direct_palette, NULL)))
		throw c_error("Not able to create a 256 Colour Palette");
}

void c_pal::set(LPDIRECTDRAWSURFACE7 buffer)
{
	buffer->SetPalette(direct_palette);
}

void c_pal::remove()
{
	DX_REMOVE(direct_palette);
}

void c_pal::activate_colour(long val, long sav2mastr)
{
	long index;

	if(sav2mastr)
	{
		for(index = 0; index < 256; index++)
		{
			master_pal[index].r = cur_pal[index].peRed;
			master_pal[index].g = cur_pal[index].peGreen;
			master_pal[index].b = cur_pal[index].peBlue;
		}
	}

	if(val == -1)
	{
		for(index = 0; index < 256; index++)
		{
			cur_pal[index].peRed = master_pal[index].r;
			cur_pal[index].peGreen = master_pal[index].g;
			cur_pal[index].peBlue = master_pal[index].b;
			cur_pal[index].peFlags = NULL;
		}

		direct_palette->SetEntries(0, 0, 256, cur_pal);	
	}
	else if(val == -2)
	{
		for(index = 0; index < 256; index++)
		{
			cur_pal[index].peRed = (unsigned char)index;
			cur_pal[index].peGreen = (unsigned char)index;
			cur_pal[index].peBlue = (unsigned char)index;
			cur_pal[index].peFlags = NULL;
		}

		if(direct_palette)
			direct_palette->SetEntries(0, 0, 256, cur_pal);	
	}
	else
	{
		for(index = 0; index < 256; index++)
		{
			cur_pal[index].peRed = (unsigned char)val;
			cur_pal[index].peGreen = (unsigned char)val;
			cur_pal[index].peBlue = (unsigned char)val;
			cur_pal[index].peFlags = NULL;
		}

		direct_palette->SetEntries(0, 0, 256, cur_pal);	
	}
}

void c_pal::activate_colour(c_colour *pal)
{
	long index;

	for(index = 0; index < 256; index++)
	{
		master_pal[index].r = pal[index].r;
		master_pal[index].g = pal[index].g;
		master_pal[index].b = pal[index].b;
	}
}
