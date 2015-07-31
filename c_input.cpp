/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#include "dxdefs.h"
#include "c_error.h"
#include "c_input.h"

c_input::c_input()
{
	ShowCursor(FALSE);

	direct_input = NULL;
	direct_keyboard = NULL;
	direct_mouse = NULL;
}

c_input::~c_input()
{
	remove();
}

void c_input::create(HWND hwnd, HINSTANCE hinst, long scr_width, long scr_height, long default_res_id, long hourglass_res_id)
{
	game_shut_down = FALSE;

	c_input::scr_width = scr_width;
	c_input::scr_height = scr_height;
	mouse_pos.x = scr_width / 2;
	mouse_pos.y = scr_height / 2;

	default_cursor.load_res(hinst, default_res_id);
	hourglass_cursor.load_res(hinst, hourglass_res_id);

	if(DirectInputCreate(hinst, DIRECTINPUT_VERSION, &direct_input, NULL) != DI_OK)
		throw c_error("Unable to create Direct Input");

	if(direct_input->CreateDevice(GUID_SysKeyboard, &direct_keyboard, NULL) != DI_OK)
		throw c_error("Unable to create DInput Keyboard");

	if(direct_keyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) != DI_OK)
		throw c_error("Unable to Set Keyboard Cooperative Level");

	if(direct_keyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		throw c_error("Unable to Set Keyboard Data Format");

	if(direct_keyboard->Acquire() != DI_OK)
		throw c_error("Unable to Acquire Keyboard");

	if(direct_input->CreateDevice(GUID_SysMouse, &direct_mouse, NULL) != DI_OK)
		throw c_error("Unable to create DInput Mouse");

	if(direct_mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) != DI_OK)
		throw c_error("Unable to Set Mouse Cooperative Level");

	if(direct_mouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		throw c_error("Unable to Set Mouse Data Format");

	if(direct_mouse->Acquire() != DI_OK)
		throw c_error("Unable to Acquire Mouse");
}

void c_input::remove()
{
	if(direct_mouse)
	{
		direct_mouse->Unacquire();
		direct_mouse->Release();
	}
	direct_mouse = NULL;

	if(direct_keyboard)
	{
		direct_keyboard->Unacquire();
		direct_keyboard->Release();
	}
	direct_keyboard = NULL;

	if(direct_input)
		direct_input->Release();
	direct_input = NULL;

	ShowCursor(TRUE);
}

void c_input::update()
{
	direct_mouse->Acquire();
	direct_keyboard->GetDeviceState(256, (LPVOID)keyboard_state);

	direct_keyboard->Acquire();
	direct_mouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);

	old_pos.x = mouse_pos.x;
	old_pos.y = mouse_pos.y;
	reletive_pos.x = mouse_state.lX;
	reletive_pos.y = mouse_state.lY;
	mouse_pos.x += reletive_pos.x;
	mouse_pos.y += reletive_pos.y;

	if(mouse_pos.x < 0) mouse_pos.x = 0;
	if(mouse_pos.y < 0) mouse_pos.y = 0;
	if(mouse_pos.x >= scr_width) mouse_pos.x = scr_width - 1;
	if(mouse_pos.y >= scr_height) mouse_pos.y = scr_height - 1;

	if(mouse_state.rgbButtons[MOUSE_LEFT_BUTTON] & 0x80)
	{
		if(mouse_button_down == ' ' || (mouse_button_down == 'l' && (start_drag_pos.x > mouse_pos.x || start_drag_pos.y > mouse_pos.y)))
		{
			start_drag_pos.x = mouse_pos.x;
			start_drag_pos.y = mouse_pos.y;
			is_draging = TRUE;
		}
		mouse_button_down = 'l';
	}
	else if(mouse_state.rgbButtons[MOUSE_RIGHT_BUTTON] & 0x80 || (mouse_button_down == 'r' && (start_drag_pos.x > mouse_pos.x || start_drag_pos.y > mouse_pos.y)))
	{
		if(mouse_button_down == ' ')
		{
			start_drag_pos.x = mouse_pos.x;
			start_drag_pos.y = mouse_pos.y;
			is_draging = TRUE;
		}
		mouse_button_down = 'r';
	}
	else
	{
		mouse_button_down = ' ';	
		is_draging = FALSE;
	}

	raw_key = getch();

	if(keyboard_state[DIK_UP] & 0x80 || keyboard_state[DIK_NUMPAD8] & 0x80) 
		key_press = e_key_up;
	else if(keyboard_state[DIK_DOWN] & 0x80 || keyboard_state[DIK_NUMPAD2] & 0x80) 
		key_press = e_key_down;
	else if(keyboard_state[DIK_LEFT] & 0x80 || keyboard_state[DIK_NUMPAD4] & 0x80) 
		key_press = e_key_left;
	else if(keyboard_state[DIK_RIGHT] & 0x80 || keyboard_state[DIK_NUMPAD6] & 0x80) 
		key_press = e_key_right;
	else if(keyboard_state[DIK_RCONTROL] & 0x80 || keyboard_state[DIK_LCONTROL] & 0x80) 
		key_press = e_key_select;
	else if(keyboard_state[DIK_RMENU] & 0x80 || keyboard_state[DIK_LMENU] & 0x80) 
		key_press = e_key_cancel;
	else if(keyboard_state[DIK_ESCAPE] & 0x80) 
		key_press = e_key_menu;
	else if(keyboard_state[DIK_F1] & 0x80) 
		key_press = e_key_f1;
	else if(keyboard_state[DIK_F2] & 0x80) 
		key_press = e_key_f2;
	else if(keyboard_state[DIK_F3] & 0x80) 
		key_press = e_key_f3;
	else 
		key_press = e_key_none;
}

char c_input::getch()
{
	if(keyboard_state[DIK_A] & 0x80)
		return 'a';
	else if(keyboard_state[DIK_B] & 0x80)
		return 'b';
	else if(keyboard_state[DIK_C] & 0x80)
		return 'c';
	else if(keyboard_state[DIK_D] & 0x80)
		return 'd';
	else if(keyboard_state[DIK_E] & 0x80)
		return 'e';
	else if(keyboard_state[DIK_F] & 0x80)
		return 'f';
	else if(keyboard_state[DIK_G] & 0x80)
		return 'g';
	else if(keyboard_state[DIK_H] & 0x80)
		return 'h';
	else if(keyboard_state[DIK_I] & 0x80)
		return 'i';
	else if(keyboard_state[DIK_J] & 0x80)
		return 'j';
	else if(keyboard_state[DIK_K] & 0x80)
		return 'k';
	else if(keyboard_state[DIK_L] & 0x80)
		return 'l';
	else if(keyboard_state[DIK_M] & 0x80)
		return 'm';
	else if(keyboard_state[DIK_N] & 0x80)
		return 'n';
	else if(keyboard_state[DIK_O] & 0x80)
		return 'o';
	else if(keyboard_state[DIK_P] & 0x80)
		return 'p';
	else if(keyboard_state[DIK_Q] & 0x80)
		return 'q';
	else if(keyboard_state[DIK_R] & 0x80)
		return 'r';
	else if(keyboard_state[DIK_S] & 0x80)
		return 's';
	else if(keyboard_state[DIK_T] & 0x80)
		return 't';
	else if(keyboard_state[DIK_U] & 0x80)
		return 'u';
	else if(keyboard_state[DIK_V] & 0x80)
		return 'v';
	else if(keyboard_state[DIK_W] & 0x80)
		return 'w';
	else if(keyboard_state[DIK_X] & 0x80)
		return 'x';
	else if(keyboard_state[DIK_Y] & 0x80)
		return 'y';
	else if(keyboard_state[DIK_Z] & 0x80)
		return 'z';
	else if(keyboard_state[DIK_1] & 0x80)
		return '1';
	else if(keyboard_state[DIK_2] & 0x80)
		return '2';
	else if(keyboard_state[DIK_3] & 0x80)
		return '3';
	else if(keyboard_state[DIK_4] & 0x80)
		return '4';
	else if(keyboard_state[DIK_5] & 0x80)
		return '5';
	else if(keyboard_state[DIK_6] & 0x80)
		return '6';
	else if(keyboard_state[DIK_7] & 0x80)
		return '7';
	else if(keyboard_state[DIK_8] & 0x80)
		return '8';
	else if(keyboard_state[DIK_9] & 0x80)
		return '9';
	else if(keyboard_state[DIK_0] & 0x80)
		return '0';	
	else if(keyboard_state[DIK_PERIOD] & 0x80)
		return '.';
	else if(keyboard_state[DIK_RETURN] & 0x80)
		return '\r';
	else if(keyboard_state[DIK_BACKSLASH] & 0x80)
		return '\\';
	else if(keyboard_state[DIK_BACKSPACE] & 0x80)
		return '\b';
	else if(keyboard_state[DIK_SPACE] & 0x80)
		return ' ';
	else
		return 0;
}




