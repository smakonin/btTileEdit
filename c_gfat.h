/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_GFAT_H
#define C_GFAT_H

#include <memory.h>
#include <stdio.h>
#include "c_error.h"
#include "c_filelib.h"

class c_fat_entry
{
public:
	char name[17];
	char filler[3];
	long start_pos;
	long size;
};

class c_gfat
{
private:
	c_fat_entry entry;
	c_filelib filelib;
	FILE *fp_index;
	FILE *fp_data;

public:
	inline c_gfat()
	{
		fp_index = NULL;
		fp_data = NULL;
	}

	long inline find(char *filename)
	{
		long found;

		fp_index = fopen("gfat.i", "rb");

		if(!fp_index) 
			return FALSE;

		found = FALSE;
		while(!feof(fp_index))
		{
			fread(&entry, sizeof(entry), 1, fp_index);
			if(!strcmp(filename, entry.name))
			{
				found = TRUE;
				break;
			}
		}

		if(fp_index)
			fclose(fp_index);
		fp_index = NULL;

		return found;
	}

	void inline open(char *filename)
	{
		if(!find(filename))
			throw c_error("Uable to extract %s", filename);

		close();		
		fp_data = fopen("gfat.d", "rb");
		fseek(fp_data, entry.start_pos, SEEK_SET);
	}

	void inline read(unsigned char **buf_ptr, long *size)
	{
		*size = entry.size;
		*buf_ptr = (unsigned char *)malloc(*size);
		fread(*buf_ptr, *size, 1, fp_data);
	}

	void inline close()
	{
		if(fp_data)
			fclose(fp_data);
		fp_data = NULL;
	}

	void inline add(char *filename, void *buf, long size)
	{
		if(filename)
		{
			if(find(filename))
				throw c_error("Entry already exists %s", filename);

			memset(entry.name, 0, sizeof(entry.name));
			memcpy(entry.name, filename, 16);

			close();
			fp_data = fopen("gfat.d", "ab");

			entry.start_pos = filelib.file_size("gfat.d");
			entry.size = 0;
		}

		if(buf)
		{
			fwrite(buf, size, 1, fp_data);
			entry.size += size;
		}
		else
		{
			close();			
			fp_index = fopen("gfat.i", "ab");
			fwrite(&entry, sizeof(entry), 1, fp_index);
			if(fp_index)
				fclose(fp_index);
			fp_index = NULL;
		}
	}
};

#endif //C_GFAT_H