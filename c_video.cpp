/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "dxdefs.h"
#include "c_error.h"
#include "c_video.h"

LRESULT CALLBACK msg_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

c_video::c_video()
{
	direct_draw = NULL;
	display_mem = NULL;
	max_colours = 256;
}

c_video::~c_video()
{
	remove();
}

HRESULT WINAPI dd_callback(LPDDSURFACEDESC2 ddsd, LPVOID context)
{
	if(ddsd->dwRefreshRate)
	{
		*((long *)context) = ddsd->dwRefreshRate;
		return DDENUMRET_CANCEL;
	}
	else
	{
		return DDENUMRET_OK;
	}
}

void c_video::create(long width, long height, char *title, HINSTANCE hinst)
{
	WNDCLASSEX winclass;

	c_video::hinst = hinst;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = msg_proc;
	winclass.cbClsExtra	= 0;
	winclass.cbWndExtra	= 0;
	winclass.hInstance = hinst;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW); 
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = "game";
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&winclass))
		throw c_error();		

	if(!(hwnd = CreateWindowEx(NULL, "game", title, WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hinst, NULL)))
		throw c_error();
	
	if(FAILED(DirectDrawCreateEx(NULL, (void **)&direct_draw, IID_IDirectDraw7, NULL)))
		throw c_error("Could not create DirectDraw");

	if(FAILED(direct_draw->SetCooperativeLevel(hwnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX)))
		throw c_error("Could not set Cooperative Level");

	if(FAILED(direct_draw->SetDisplayMode(width, height, 8, 0, 0)))
		throw c_error("Could not create Video Mode: %ux%u 256 colours", width, height);

	DX_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if(FAILED(direct_draw->CreateSurface(&ddsd, &display_mem, NULL)))
		throw c_error("Could not create Primary Display Buffer");

	c_image::create(width, height);
	c_image::header.create(width, height);

	c_pal::create(direct_draw);
	c_pal::set(display_mem);
}

void c_video::remove()
{
	c_pal::remove();
	DX_REMOVE(display_mem);
	DX_REMOVE(direct_draw);
	DestroyWindow(hwnd);
}

void c_video::flip()
{
	unsigned char *draw_buffer;
	int mempitch;
	int row;

	DX_INIT_STRUCT(ddsd);
	display_mem->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	draw_buffer = (unsigned char *)ddsd.lpSurface;
	mempitch = ddsd.lPitch;
		
	direct_draw->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	
	for(row = 0; row < height; row++)
		memcpy(&draw_buffer[row * mempitch], &buffer[row * width], width);

	display_mem->Unlock(NULL);
}

void c_video::text_out(long x, long y, long colour, long transparent_bk, char align, char *text, ...)
{
	HDC hdc;
	char *tok_ptr;
	va_list	msg;
    char buf[1024] = {'\0'};
   
	va_start(msg, text);
	_vsnprintf(buf, 1023, text, msg);	
	va_end(msg);

	if(SUCCEEDED(display_mem->GetDC(&hdc)))
	{
		SetTextColor(hdc, colour);
		SetBkColor(hdc, 0x00000000);
		
		if(transparent_bk)
			SetBkMode(hdc, TRANSPARENT);
		else
			SetBkMode(hdc, OPAQUE);

		switch(align)
		{
		case 'C':
		case 'c':
			SetTextAlign(hdc, TA_CENTER);
			break;
		case 'R':
		case 'r':
			SetTextAlign(hdc, TA_RIGHT);
			break;
		default:
			SetTextAlign(hdc, TA_LEFT);
			break;
		}

		tok_ptr = strtok(buf, "\n");
		while(tok_ptr)
		{
			ExtTextOut(hdc, x, y, 0, NULL, tok_ptr, strlen(tok_ptr), NULL);
			y += 20;
			tok_ptr = strtok(NULL, "\n");
		}
		display_mem->ReleaseDC(hdc);
	}
}

void c_video::clear(long colour)
{
	DDBLTFX ddfx;

	DX_INIT_STRUCT(ddfx);
	ddfx.dwFillColor = colour;
	direct_draw->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	display_mem->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddfx);
}

void c_video::clear_buffer(unsigned char colour)
{
	c_image::clear(colour);
}

LRESULT CALLBACK msg_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
	switch(msg)
	{	
		case WM_CLOSE: 
			PostQuitMessage(0);
			return 0;
		
		case WM_DESTROY: 
			PostQuitMessage(0);
			return 0;
    }

	return DefWindowProc(hwnd, msg, wparam, lparam);
}









