
#include <stdio.h>
#include <stdlib.h>
#include "CB_io.h"
#include "CB_Time.h"
#include "CB_error.h"

int IntToBcd( int x ) {
	int h,l;
	l = x % 10;
	h = x / 10;
	return h*16+l ;
}


int GetTime() {	// ->   HHMMSS : BCD (0x  235959 etc)
	int hour,min,sec;

	if ( CPU_check() == 3 ) {	//SH3
		sec  = RTC3.RSECCNT ;
		min  = RTC3.RMINCNT ;
		hour = RTC3.RHRCNT  ;
	} else	{			//SH4
		sec  = RTC4.RSECCNT ;
		min  = RTC4.RMINCNT ;
		hour = RTC4.RHRCNT  ;
	}
	
	return ( hour << 16 ) + ( min << 8 ) +  sec ;
}

int GetDate() {	// -> YYYYMMDD : BCD (0x20161201 etc)
	int year,month,day;
	
	if ( CPU_check() == 3 ) {	//SH3
		day   = RTC3.RDAYCNT ;
		month = RTC3.RMONCNT ;
		year  = RTC3.RYRCONT ;
	} else	{			//SH4
		day   = RTC4.RDAYCNT ;
		month = RTC4.RMONCNT ;
		year  = RTC4.RYRCONT ;
	}
	
	return ( year << 16 ) + ( month << 8 ) +  day ;
}

void SetRtc( int data ) {	// data: ticks 1/128 count
	int hour,min,sec,tick;
	int a,year1,year2,month,day;
	unsigned char timestr[8];

	tick = data % 128 ;
	sec  = ( data / 128 ) % 60 ; 
	min  = ( data / 60/128 ) % 60 ; 
	hour = ( data / 60/60/128 )  ; 
	if ( hour >= 24 ) { CB_Error(RangeERR); return; }	// Range error

	
	a = GetDate();
	year1 = ( a >> 24 ) & 0xFF ;
	year2 = ( a >> 16 ) & 0xFF ;
	month = ( a >>  8 ) & 0xFF ;
	day   = ( a       ) & 0xFF ;

	timestr[0]=year1;
	timestr[1]=year2;
	timestr[2]=month;
	timestr[3]=day;
	timestr[4]=IntToBcd( hour );
	timestr[5]=IntToBcd( min  );
	timestr[6]=IntToBcd( sec  );
	timestr[7]=0;

	RTC_SetDateTime( timestr ) ;
}
