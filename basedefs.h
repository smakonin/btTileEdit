/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#define WIN32_LEAN_AND_MEAN

#define LIMIT(val,l,h)              ((val) < (l) ? (l) : (val) > (h) ? (h) : (val))
#define LIMIT_CIRCULAR(val,l,h)     ((val) < (l) ? (h - (l - val)) : (val) > (h) ? ((val - h) + l) : (val))
#define IN_LIMIT(val,l,h)           ((val) < (l) ? (FALSE) : (val) > (h) ? (FALSE) : (TRUE))
