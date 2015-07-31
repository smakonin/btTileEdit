/*******************************************************************
*                                                                  *
*       Copyright (C) 2001, Stephen Makonin. All Rights Reserved.  *
*                                                                  *
********************************************************************/ 

#define WIN32_LEAN_AND_MEAN

#define DX_INIT_STRUCT(ddstruct) {memset(&ddstruct, 0, sizeof(ddstruct)); ddstruct.dwSize = sizeof(ddstruct);}
#define DX_REMOVE(dxo) {if(dxo) dxo->Release(); dxo = NULL;}
