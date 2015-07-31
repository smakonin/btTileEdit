/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#ifndef C_RLE_H
#define C_RLE_H

#include <stdio.h>

class c_rle
{
public:
	inline decode_byte(unsigned char *byte, unsigned char *count, FILE *fp)
	{
		int i;
    
		*count = 1;
        
		if(EOF == (i = getc(fp)))
			return EOF;
    
		if(0xC0 == (0xC0 & i))
		{
			*count = 0x3F & i;        
			if(EOF == (i = getc(fp)))
				return EOF;
		}
    
		*byte = i;
		return 0;
	}

	long inline encode_byte(unsigned char byte, unsigned char count, FILE *fp)
	{
		if(count) 
		{
			if((count == 1) && (0xC0 != (0xC0 & byte)))
			{
				if(EOF == putc((int )byte, fp))
					return 0;
				return 1;
			}
			else
			{
				if(EOF == putc((int )0xC0 | count, fp))
					return 0;
				
				if(EOF == putc((int )byte, fp))
					return 0;
				
				return 2;
			}
		}
		return 0;
	}

	long inline encode_line(unsigned char *buf, int len, FILE *fp)
	{
		unsigned char cur, last;
		int scr_index, i;
		register int total;
		register unsigned char count;

		total = 0;
		count = 1;
		last = *(buf);

		for(scr_index = 1; scr_index < len; scr_index++)
		{
			cur = *(++buf);
			
			if(cur == last)
			{
				count++;
				if(count == 63)
				{
					if(!(i = encode_byte(last, count, fp)))
						return 0;
					total += i;
					count = 0;
				}
			}
			else
			{
				if(count)
				{
					if(!(i = encode_byte(last, count, fp)))
						return 0;
					total += i;
				}			
				last = cur;
				count = 1;
			}
		}

		if(count)
		{
			if(!(i = encode_byte(last, count, fp)))
				return 0;		
			return total + i;
		}

		return total;
	}

	long inline decode_file(unsigned char *buf, long size, FILE *fp)
	{
		unsigned char i, count, byte;
		unsigned char *ptr;
		long l;
		
		l = 0;
		ptr = buf;
		while(l < size)
		{
			if(EOF == decode_byte(&byte, &count, fp))
				return 0;                
			for(i = 0; i < count; i++)
				*ptr++ = byte;                
			l += count;
		}
		return l;
	}

	long inline encode_file(unsigned char *buf, long scanline_size, long lines, FILE *fp)
	{
		long k, l, len;
		unsigned char *ptr;

		len = 0;
		ptr = buf;
		for(l = 0; l < lines; l++)
		{
			if(!(k = encode_line(ptr, scanline_size, fp)))
				return 0;
			len += k;
			ptr += scanline_size;
		}
		return len;
	}
};

#endif //C_RLE_H