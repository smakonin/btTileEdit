/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#include <windows.h>
#include <string.h>
#include "c_error.h"
#include "c_timer.h"
#include "c_video.h"
#include "c_input.h"
#include "c_tiles.h"
#include "c_filelib.h"
#include "c_tile_view.h"
#include "resource.h"

#define GAME_TITLE "bt.TileEdit 5"

c_video video;
c_input input;
c_timer my_timer;
c_tiles tiles;
c_filelib filelib;
c_tile_view tile_view;

char *tile_name;

void help_screen()
{
	video.clear(0);
	video.text_out(0, 0, 0x00FFFFFF, TRUE, 'l', "\
HELP SCREEN - Copyright (C) 2000 Makonin Consulting Corp.\n\
==============================================================\n\n\
bt.TileEdit Commands:\n\n\
Arrow Key: (Both Select and Zoom Mode) Move Tile Display\n\
C - (Select Mode) Copy Tile, First Click Sorce, Second Click Target\n\
R - (Select Mode) Rotate Tile\n\
H - (Select Mode) Horizontal Tile Flip\n\
V - (Select Mode) Vertical Tile Flip\n\
T - (Select Mode) Transparent Copy Tile\n\
M - (Select Mode) Move Tile and Update Map\n\
Left Click - (Select Mode) Enter Zoom Mode\n\
R - (Zoom Mode) - Replace Clicked Colour With Selected Colour\n\
C - (Zoom Mode) - Clear Tile With Selected Colour\n\
S - (Zoom Mode) Scroll Tile Under Cursor, Use Arrow Keys Q to Exit Scroll Mode\n\
ESC - (Zoom Mode) Go Back to Select Mode\n\
Left Click - (Zoom Mode) Place Pixel/Select Colour From Bar\n\
Right Click - (Zoom Mode) Select Colour Under Cursor\n\n\
Press Space Key...");

	while(input.raw_key != ' ')
		input.update();
	while(input.raw_key == ' ')
		input.update();
}

long save_data()
{
	video.text_out(320, 200, 0x00FFFFFF, FALSE, 'c', "  SAVE YOUR EDITS? (Y/N)  ");
	while(input.raw_key != 'n')
	{
		input.update();

		if(input.raw_key == 'y') 
		{
			video.flip();
			video.text_out(320, 200, 0x00FFFFFF, FALSE, 'c', "  SAVING...  ");
			tiles.save(tile_name);
			break;
		}
	}
	while(input.raw_key == 'y' || input.raw_key == 'n')
		input.update();
	video.flip();

	video.text_out(320, 200, 0x00FFFFFF, FALSE, 'c', "  ARE YOUR SURE YOU WANT TO EXIT? (Y/N)  ");
	while(input.raw_key != 'n')
	{
		input.update();

		if(input.raw_key == 'y') 
		{
			video.flip();
			video.text_out(320, 200, 0x00FFFFFF, FALSE, 'c', "  EXITING...  ");
			input.game_shut_down = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev_inst, LPSTR lpcmdline, int ncmdshow) 
{
	MSG	msg;
	unsigned char cx = 0;
	unsigned char cy = 0;
	long i, j, k, l;
	long in_zoom_mode = FALSE;
	char zoom_factor = 5;
	unsigned char lbutton_colour = 16;
	unsigned char incro = +5;
	long mode = 0;
	unsigned short source, target;
	long dir_walk = 0;
	long dir_walk_x = 0;
	long dir_walk_y = 0;
	long sel_tile_mode = FALSE;
	long tile_x = 0;
	long tile_y = 0;
	
	try
	{
		video.create(640, 480, GAME_TITLE, hinst);
		input.create(video.hwnd, video.hinst, video.width, video.height, IDR_IDR_PCX_MC_DEFAULT, IDR_IDR_PCX_MC_HOURGLAS);
		
		tile_name = strtok(lpcmdline, " ");
		if(filelib.does_file_exist(tile_name))
			tiles.create(tile_name, video.width, video.height);
		else
			throw c_error("Usage:  btTileEdit.exe  [tiles.pcx]\r\n%s",lpcmdline);

		tile_view.create(&video, &tiles);

		video.clear(0);
		video.activate_colour(tiles.master_pal);
		video.activate_colour();
	}
	catch(c_error e)
	{
		video.remove();
		e.show(GAME_TITLE);
		goto clean_and_exit;
	}

	while(!input.game_shut_down)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{ 
			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		}

		try
		{
			input.update();

			if(in_zoom_mode)
			{
				k = zoom_factor * tiles.tile_len;
				cx = (unsigned char)((input.mouse_pos.x - 7) / k);
				cy = (unsigned char)((input.mouse_pos.y - 7) / k);

				if(mode == 's')
				{
					switch(input.key_press)
					{
					case e_key_up:
						tiles.slide(tiles.make_tile_no(tile_view.sel_x + cx, tile_view.sel_y + cy), 0, 1);
						break;
					case e_key_down:
						tiles.slide(tiles.make_tile_no(tile_view.sel_x + cx, tile_view.sel_y + cy), 0 , -1);
						break;
					case e_key_left:
						tiles.slide(tiles.make_tile_no(tile_view.sel_x + cx, tile_view.sel_y + cy), -1, 0);
						break;
					case e_key_right:
						tiles.slide(tiles.make_tile_no(tile_view.sel_x + cx, tile_view.sel_y + cy), 1, 0);
						break;
					}
				}
				else
				{
					switch(input.key_press)
					{
					case e_key_menu:
						in_zoom_mode = FALSE;
						mode = 0;
						while(input.key_press == e_key_menu)
							input.update();
						break;
					case e_key_up:
						tile_view.sel_y--;
						if(tile_view.sel_y < 0)
							tile_view.sel_y = 0;
						if(tile_view.sel_y > 60)
							tile_view.sel_y = 60;
						while(input.key_press == e_key_menu)
							input.update();
						break;
					case e_key_down:
						tile_view.sel_y++;
						if(tile_view.sel_y < 0)
							tile_view.sel_y = 0;
						if(tile_view.sel_y > 60)
							tile_view.sel_y = 60;
						while(input.key_press == e_key_menu)
							input.update();
						break;
					case e_key_left:
						tile_view.sel_x--;
						if(tile_view.sel_x < 0)
							tile_view.sel_x = 0;
						if(tile_view.sel_x > 60)
							tile_view.sel_x = 60;
						while(input.key_press == e_key_menu)
							input.update();
						break;
					case e_key_right:
						tile_view.sel_x++;
						if(tile_view.sel_x < 0)
							tile_view.sel_x = 0;
						if(tile_view.sel_x > 60)
							tile_view.sel_x = 60;
						while(input.key_press == e_key_menu)
							input.update();
						break;
					}
				}

				if(input.raw_key == 'p')
					video.save_pcx("save_scr.pcx");

				if(!mode && (input.raw_key == 'c' || input.raw_key == 'r' || input.raw_key == 's' || input.raw_key == 'k'))
				{
					mode = input.raw_key;
					if(mode == 'c')
					{
						while(input.raw_key == mode)
							input.update();
						source = target = 0xFFFF;
						input.raw_key = ' ';
					}
				}
				else
				{
					if(mode && input.raw_key == 'q')
					{
						while(input.raw_key == mode)
							input.update();
						mode = 0;
						input.raw_key = ' ';
					}
				}

				if(mode)
				{
					if(input.mouse_button_down == 'l')
					{
						if(cx >= 4 || cy >= 4)
							mode = 0;

						switch(mode)
						{
						case 'c':
							tiles.clear(tiles.make_tile_no(tile_view.sel_x + cx, tile_view.sel_y + cy), lbutton_colour);
							mode = 0;
							break;
						case 'r':
							tiles.relpace_colour(tiles.make_tile_no(tile_view.sel_x + cx, tile_view.sel_y + cy), video.get_pixel(input.mouse_pos.x, input.mouse_pos.y), lbutton_colour);
							mode = 0;
							break;
						}				
						while(input.mouse_button_down == 'l')
							input.update();
					}	
				}
				else
				{				
					if(input.mouse_button_down == 'l')
					{
						if(input.mouse_pos.x > 7 && input.mouse_pos.x < 632 && input.mouse_pos.y > 449 && input.mouse_pos.y < 471)
							lbutton_colour = video.get_pixel(input.mouse_pos.x, input.mouse_pos.y);

						if(input.mouse_pos.x > 7 && input.mouse_pos.x < 27 && input.mouse_pos.y > 420 && input.mouse_pos.y < 440)
							lbutton_colour = tiles.transparency;

						k = zoom_factor * tiles.tile_len;
						for(i = 0; i < 4; i++)
							for(j = 0; j < 4; j++)
								if(input.mouse_pos.x > 7 + (j * k) && input.mouse_pos.y > 7 + (i * k) && input.mouse_pos.x < 8 + ((j + 1) * k) && input.mouse_pos.y < 8 + ((i + 1) * k))
									tiles.put_pixel(tiles.make_tile_no(tile_view.sel_x + (unsigned char)j, tile_view.sel_y + (unsigned char)i), (input.mouse_pos.x - (8 + (j * k))) / zoom_factor, (input.mouse_pos.y - (8 + (i * k))) / zoom_factor, lbutton_colour);
					}
					
					if(input.mouse_button_down == 'r')
					{
						
						k = zoom_factor * tiles.tile_len;
						for(i = 0; i < 4; i++)
							for(j = 0; j < 4; j++)
								if(input.mouse_pos.x > 7 + (j * k) && input.mouse_pos.y > 7 + (i * k) && input.mouse_pos.x < 8 + ((j + 1) * k) && input.mouse_pos.y < 8 + ((i + 1) * k))
									lbutton_colour = tiles.get_pixel(tiles.make_tile_no(tile_view.sel_x + (unsigned char)j, tile_view.sel_y + (unsigned char)i), (input.mouse_pos.x - (8 + (j * k))) / zoom_factor, (input.mouse_pos.y - (8 + (i * k))) / zoom_factor);
					}
				}

				video.clear_buffer(0);
				k = zoom_factor * tiles.tile_len;
				l = 2 * tiles.tile_len;
				video.draw_box(439, 7, 440 + l * 4, 8 + l * 4, 255);
				video.draw_box(439, 199, 440 + l * 4, 200 + l * 4, 255);
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 4; j++)
					{
						video.draw_box(7 + (j * k), 7 + (i * k), 8 + ((j + 1) * k), 8 + ((i + 1) * k), 255);
						tiles.place(8 + (j * k), 8 + (i * k), &video, tiles.make_tile_no(tile_view.sel_x + (unsigned char)j, tile_view.sel_y + (unsigned char)i), zoom_factor, FALSE);
						tiles.place(440 + (j * l), 8 + (i * l), &video, tiles.make_tile_no(tile_view.sel_x + (unsigned char)j, tile_view.sel_y + (unsigned char)i), 2, FALSE);
						
						if(tile_view.sel_y == tiles.tiles_per_len - my_timer.max_frames)
							tiles.place(440 + (j * l), 200 + (i * l), &video, tiles.make_tile_no(1, 0), 2, FALSE);
						else
							tiles.place(440 + (j * l), 200 + (i * l), &video, tiles.make_tile_no(tile_view.sel_x, tile_view.sel_y), 2, FALSE);
					}
				}

				if(tile_view.sel_y == tiles.tiles_per_len - my_timer.max_frames)
				{
					if(my_timer.elaped(FALSE) >= 66)
					{
						switch(dir_walk)
						{
						case 0:								
							dir_walk_y += 4;
							if(dir_walk_y == 306)
								dir_walk = 2;
							break;
						case 1:								
							dir_walk_x -= 4;
							if(dir_walk_x == 450)
								dir_walk = 0;
							break;
						case 2:								
							dir_walk_x += 4;
							if(dir_walk_x == 546)
								dir_walk = 3;
							break;
						case 3:								
							dir_walk_y -= 4;
							if(dir_walk_y == 210)
								dir_walk = 1;
							break;
						}
						my_timer.elaped(TRUE);
					}
					
					tiles.place(dir_walk_x, dir_walk_y, &video, tiles.make_tile_no(tile_view.sel_x + my_timer.frame_no, tile_view.sel_y + (char)dir_walk), 2, TRUE);
				}

				video.draw_box(7, 420, 27, 440, 255);
				for(i = 421; i < 440; i++)
					video.draw_hline(8, 26, i, tiles.transparency);

				video.draw_box(7, 449, 632, 471, 255);
				for(i = 0; i < 624; i += 3)
				{
					if(lbutton_colour == 16 + i / 3)
					{
						video.draw_vline(8 + i, 444, 447, 255);
						video.draw_vline(9 + i, 444, 447, 255);
						video.draw_vline(10 + i, 444, 447, 255);
					}

					video.draw_vline(8 + i, 450, 470, (unsigned char)(16 + i / 3));
					video.draw_vline(9 + i, 450, 470, (unsigned char)(16 + i / 3));
					video.draw_vline(10 + i, 450, 470, (unsigned char)(16 + i / 3));
				}
			}
			else
			{
				switch(input.key_press)
				{
				case e_key_menu:
					if(save_data())
						goto clean_and_exit;
					break;
				case e_key_f1:
					while(input.key_press == e_key_f1)
						input.update();
					help_screen();
					break;
				case e_key_up:
					tile_view.corner_y--;
					if(tile_view.corner_y < 0)
						tile_view.corner_y = 0;
					break;
				case e_key_down:
					tile_view.corner_y++;
					if(tile_view.corner_y > tiles.tiles_per_len - tiles.tile_view_height)
						tile_view.corner_y = tiles.tiles_per_len - tiles.tile_view_height;
					break;
				case e_key_left:
					tile_view.corner_x--;
					if(tile_view.corner_x < 0)
						tile_view.corner_x = 0;
					break;
				case e_key_right:
					tile_view.corner_x++;
					if(tile_view.corner_x > tiles.tiles_per_len - tiles.tile_view_width)
						tile_view.corner_x = tiles.tiles_per_len - tiles.tile_view_width;
					break;
				}
			
				if(input.raw_key == 'p')
					video.save_pcx("save_scr.pcx");

				if(!mode && (input.raw_key == 'c' || input.raw_key == 'r' || input.raw_key == 'h' || input.raw_key == 'v' || input.raw_key == 't' || input.raw_key == 'm' || input.raw_key == 'k'))
				{
					mode = input.raw_key;
					if(mode == 'c' || mode == 't' || mode == 'm')
					{
						while(input.raw_key == mode)
							input.update();
						source = target = 0xFFFF;
					}
				}
				
				tile_view.render(mode);

				if(input.mouse_button_down == 'l')
				{
					tile_view.select(input.mouse_pos.x / tiles.tile_len, input.mouse_pos.y / tiles.tile_len);

					switch(mode)
					{
					case 'm':
					case 't':
					case 'c':
						if(source == 0xFFFF)
						{
							source = tiles.make_tile_no(tile_view.sel_x, tile_view.sel_y); 
						}
						else
						{
							if(target == 0xFFFF)
							{
								target = tiles.make_tile_no(tile_view.sel_x, tile_view.sel_y);
								if(mode == 't')
									tiles.copy(source, target, TRUE);
								else
									tiles.copy(source, target, FALSE);

								if(mode == 'm')
								{
									tiles.clear(source, 31);
								}

								mode = 0;
							}
						}
						break;
					case 'v':
						tiles.vflip(tiles.make_tile_no(tile_view.sel_x, tile_view.sel_y));
						mode = 0;
						break;
					case 'h':
						tiles.hflip(tiles.make_tile_no(tile_view.sel_x, tile_view.sel_y));
						mode = 0;
						break;
					case 'r':
						tiles.rotate(tiles.make_tile_no(tile_view.sel_x, tile_view.sel_y));
						mode = 0;
						break;
					default:
						if(tile_view.sel_x == 63)
							tile_view.sel_x = 60;
						if(tile_view.sel_y == 63)
							tile_view.sel_y = 60;
						dir_walk = 0;
						dir_walk_x = 450;
						dir_walk_y = 210;
						my_timer.elaped(TRUE);
						in_zoom_mode = TRUE;
						mode = 0;
						break;
					}				
					while(input.mouse_button_down == 'l')
						input.update();
				}	
			}
			
			video.master_pal[tiles.transparency].g = video.master_pal[tiles.transparency].g + incro;
			video.activate_colour();
			if(video.master_pal[tiles.transparency].g == 200)
				incro = -5;
			else if(video.master_pal[tiles.transparency].g == 50)
				incro = +5;

			video.put_image(input.mouse_pos.x, input.mouse_pos.y, &input.default_cursor, TRUE);
			video.flip();
			if(in_zoom_mode)
			{
				switch(mode)
				{
				case 'c':
					if(sel_tile_mode)
						video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "CLEAR TILE");
					break;
				case 'r':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "REPLACE COLOUR");
					break;
				case 's':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "SCROLL TILE");
					break;
				default:
					video.text_out(0, 460, 0x00FFFFFF, TRUE, 'l', "bt.TileEdit: F1 for Help - %s", my_timer.date_time);
					break;
				}
			}
			else
			{
				switch(mode)
				{
				case 'c':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "COPY TILE");
					break;
				case 't':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "TRANSPARENT COPY");
					break;
				case 'r':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "ROTATE TILE");
					break;
				case 'h':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "HORZ FLIP");
					break;
				case 'v':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "VERT FLIP");
					break;
				case 'm':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "MOVE TILE");
					break;
				case 'k':
					video.text_out(0, 460, 0x00FFFFFF, FALSE, 'l', "COUNT TILE USAGE");
					break;
				default:
					video.text_out(0, 460, 0x00FFFFFF, TRUE, 'l', "bt.TileEdit: F1 for Help - %s", my_timer.date_time);
					break;
				}
			}
			my_timer.update();
		}
		catch(c_error e)
		{
			video.remove();
			e.show(GAME_TITLE);
			goto clean_and_exit;
		}
	}

clean_and_exit:
	input.game_shut_down = TRUE;
	input.remove();
	tiles.remove();
	video.remove();	
	return 0;
}


