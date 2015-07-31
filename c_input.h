/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_INPUT_H
#define C_INPUT_H

#define DIRECTINPUT_VERSION 0x0700

#include <windows.h>
#include <dinput.h>
#include "c_image.h"

#define MOUSE_LEFT_BUTTON  0
#define MOUSE_RIGHT_BUTTON 1
#define KEY_MAP_YES        0x59
#define KEY_MAP_NO         0x4E
#define KEY_MAP_QUIT       0x51
#define KEY_MAP_F1		   DIK_F1
#define KEY_MAP_F2	       DIK_F2
#define KEY_MAP_F3	       DIK_F3

enum e_input_keys
{
	e_key_none = 0,
	e_key_up,
	e_key_down,
	e_key_left,
	e_key_right,
	e_key_select,
	e_key_cancel,
	e_key_menu,
	e_key_f1,
	e_key_f2,
	e_key_f3
};

class c_input
{
private:
	LPDIRECTINPUT direct_input;
	LPDIRECTINPUTDEVICE direct_keyboard;
	UCHAR keyboard_state[256];
	LPDIRECTINPUTDEVICE direct_mouse;
	DIMOUSESTATE mouse_state;
	long scr_width; 
	long scr_height;

public:
	c_image hourglass_cursor;
	c_image default_cursor;
	POINT old_pos;
	POINT start_drag_pos;
	POINT mouse_pos;
	POINT reletive_pos;
	char mouse_button_down;
	long game_shut_down;
	e_input_keys key_press;
	char raw_key;
	char is_draging;

	c_input();
	~c_input();
	void create(HWND hwnd, HINSTANCE hinst, long scr_width, long scr_height, long default_res_id, long hourglass_res_id);
	void remove();
	void update();
	char getch();
};

#endif //C_INPUT_H


