/*----------------------------------------------------------*/
/*	time measurement for fx-9860G */
/*----------------------------------------------------------*/

int t_t0,t_t1,t_t2,t_tt;

int RTC_GetTicks();  // 1/128s counter

int gettimes()
{ 	
	return RTC_GetTicks();
}

void start_timer()	// initialize
{
	int cnt;

	cnt=0;
	t_t0=gettimes();
	while(gettimes()==t_t0);
	t_t1=t_t0+1;
	while(gettimes()==t_t1) cnt++;
	t_tt=cnt;
}

int get_timer()		// 1ms time resolution
{
	int cnt;

	cnt=0;
	t_t1=gettimes();
	while(gettimes()==t_t1) cnt++;
	t_t2=t_t1+1;

	return( ( (t_t2-t_t0-2)*1000-cnt*1000/t_tt )/128 );
}
