/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_FILELIB_H
#define C_FILELIB_H

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <memory.h>

class c_filelib 
{
public:
	long inline file_size(char *filename)
	{
		long length;
		int file;

		file = open(filename, O_RDONLY + O_BINARY);
		length = filelength(file);
		close(file);

		return(length);
	} 

	long inline does_file_exist(char *filename)
	{
		_finddata_t data;

		if(_findfirst(filename, &data) == -1)
			return false;
		else
			return true;
	}

	long inline load_file(char *filename, unsigned char **buf_ptr, long *size)
	{
		FILE *fp;

		if(!does_file_exist(filename))
			return FALSE;

		*size = file_size(filename);
		*buf_ptr = (unsigned char *)malloc(*size);

		fp = fopen(filename, "rb");
		fread(*buf_ptr, *size, 1, fp);
		fclose(fp);

		return TRUE;
	}

	long inline save_file(char *filename, unsigned char *buf_ptr, long size)
	{
		FILE *fp;

		fp = fopen(filename, "wb");
		fwrite(buf_ptr, size, 1, fp);
		fclose(fp);

		return TRUE;
	}
};

#endif //C_FILELIB_H