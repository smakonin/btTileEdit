/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_TIMER_H
#define C_TIMER_H

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <time.h>

class c_timer
{
public:    
	int fps;
	int cur_time;
	int start_time;
	char frame_no;
	char max_frames;
	int frames_int;
	char date_time[65];

	inline c_timer()
	{
		fps = 0;
		cur_time =0; 
		start_time = 0;
		srand(283743);
		frame_no = 0;
		max_frames = 4;
		frames_int = 90;//***1000 / max_frames / 2;
	}

	void inline update()
	{
		time_t ltime;
		struct tm *t_today;
		static int timelast = 0;
		static int count = 0;
		static int last_frames_int = 0;

		cur_time = timeGetTime();
		count++; 
		if(cur_time - timelast >= 1000)
		{
			fps = count;
			timelast = cur_time;
			count = 0;
		}

		if(cur_time - last_frames_int >= frames_int)
		{
			frame_no++;
			frame_no %= max_frames;
			last_frames_int = cur_time;
		}

		time(&ltime);
		t_today = localtime(&ltime);
		strftime(date_time, 64, "%a %b %d %H:%M:%S %Y", t_today);
	}

	int inline elaped(int reset)
	{
		if(reset)
			start_time = timeGetTime();

		return timeGetTime() - start_time;
	}

	float inline rand_no()
	{
		return (float)((rand() % 101) / 100.00);
	}

	int inline get()
	{
		return timeGetTime();
	}
};

#endif //C_TIMER_H