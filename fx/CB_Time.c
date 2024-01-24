
#include <stdio.h>
#include <stdlib.h>
#include "CB_Time.h"
#include "CB_error.h"

int IntToBcd( int x ) {
	int h,l;
	l = x % 10;
	h = x / 10;
	return h*16+l ;
}

void SetRtc( int data ) {
	int hour,min,sec,tick;

	tick = data % 128 ;
	sec  = ( data / 128 ) % 60 ; 
	min  = ( data / 60/128 ) % 60 ; 
	hour = ( data / 60/60/128 )  ; 
	if ( hour > 24 ) { CB_Error(RangeERR); return; }	// Syntax error

	if (*(volatile unsigned short*)0xFFFFFF80 != 0) {	//SH3
		RTC3.RCR2.BIT.RESET=1;
		RTC3.RCR2.BIT.START=0;
		RTC3.RSECCNT =IntToBcd( sec  );
		RTC3.RMINCNT =IntToBcd( min  );
		RTC3.RHRCNT  =IntToBcd( hour );
//		RTC3.RWKCNT  = 0x0;
//		RTC3.RDAYCNT = 0x01;
//		RTC3.RMONCNT = 0x01;
//		RTC3.RYRCONT = 0x2016;
		RTC3.RCR2.BIT.START=1;
//		RTC3.RCR2.BIT.ADJ=1;
	} else	{			//SH4
		RTC4.RCR2.BIT.RESET=1;
		RTC4.RCR2.BIT.START=0;
		RTC4.RSECCNT =IntToBcd( sec  );
		RTC4.RMINCNT =IntToBcd( min  );
		RTC4.RHRCNT  =IntToBcd( hour );
//		RTC4.RWKCNT  = 0x0;
//		RTC4.RDAYCNT = 0x01;
//		RTC4.RMONCNT = 0x01;
//		RTC4.RYRCONT = 0x2016;
		RTC4.RCR2.BIT.START=1;
//		RTC4.RCR2.BIT.ADJ=1;
	}

}

