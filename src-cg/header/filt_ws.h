/*
*SHDSP Library
*Copyright (c) Hitachi,Ltd. 1998
*Licensed material of Hitachi,Ltd
*/
/***********************************************************
* File:     filt_ws.h
* Purpose:  Filter
* Create:   1998.03.20 Rev. 1.0
***********************************************************/

#ifndef FILT_WS
#define FILT_WS

#ifndef WS_SIZE
#define WS_SIZE 1024          /* Set Global workspace buffer size=1024 shorts (2048 bytes) */
#endif

#define MEMSIZE WS_SIZE*sizeof(short)
#pragma section Y                  /*need to equate DYRAM with link.cmd file*/
short ws_mem[WS_SIZE];
short * ws_ptr=ws_mem;
long ws_left=MEMSIZE;
#pragma section

#endif
