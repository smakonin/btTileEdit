/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_ERROR_H
#define C_ERROR_H

#include <windows.h>
#include <stdio.h>
#include <string.h>

class c_error 
{
private:
	char desc[1024];

public:
	inline c_error()
	{
		LPVOID text_ptr;
		unsigned long number;

		number = GetLastError();
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, number, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &text_ptr, 0, NULL);
		sprintf(desc, "The following error has occurred:\r\n\r\n(%u)%s", number, (LPCTSTR)text_ptr);
		LocalFree( text_ptr );
	}

	inline c_error(char *text, ...)
	{
		va_list	msg;
		char buf[1024] = {'\0'};
   
		va_start(msg, text);
		_vsnprintf(buf, 1023, text, msg);	
		va_end(msg);
		
		sprintf(desc, "The following error has occurred:\r\n\r\n%s", buf);
	}

	void inline show(char *title)
	{
		FILE *fp;

		fp = fopen("ERROR.TXT", "wb");
		fwrite(desc, strlen(desc), 1, fp);
		fclose(fp);

		ShowCursor(TRUE);
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(NULL, desc, title, MB_ICONEXCLAMATION | MB_OK);
	}
};

#endif //C_ERROR_H