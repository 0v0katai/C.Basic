extern "C" {

#include "prizm.h"
#include "CBP.h"

#include "JIStoGB.h"

//----------------------------------------------------------------------------------------------
//int inpObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int inpObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int inpObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
/*
double log10int(double x){
	int i=0;
	while( x >= 10.0 ) {
		x/=10.0;
		i++;
	}
	return (double)i;
}
double powint(double x, int y){
	int i;
	double r=x;
	for ( i=1; i<y; i++ ) r*=x;
	return r;
}
*/
void RoundZero( double num, int digit ){	//
	if ( num < pow(10,-digit) ) num=0;
}
double RoundSub( double fabsnum, int digit ){
	double	tmp,num;
	tmp=pow(10,digit);
	num=floor(fabsnum*tmp+.5)/tmp;
	return num;
}	
double Round( double num, int round_mode, int digit){
	int minus=0,exp,ex2;
	double fabsnum=fabs(num),tmp,tmp2,tmp3,exf;

	if ( num < 0 ) minus=-1;

	switch ( round_mode ) {
		case Fix: // fix
			if (fabsnum<1e15) {
				num=RoundSub( fabsnum, digit );
				if ( minus ) num=-num;
			}
			break;
		case Norm:	// normal
			if ( digit==1 ) digit=10;
			if ( digit==2 ) digit=10;
		case Sci: // sci
			if ( digit== 0 ) break;
			if ( digit>=16 ) break;
			exf=log10(fabsnum);
			exp=exf;
			if ( exf > 0 ) exp++;
			exf=pow(10,exp);
			tmp3=RoundSub( fabsnum/exf, digit );
			num=tmp3*exf;
			if ( minus ) num=-num;
			break;
	}
	return num;
}
complex Cplx_Round( complex num, int round_mode, int digit){
	return Dbl2Cplx2( Round( num.real, round_mode, digit ),Round( num.imag, round_mode, digit ) );
}

void RightAlign( char *buffer, int width ){
	int i;
	char buffer2[64];
	for (i=0;i<32;i++) buffer2[i]=' ';
	buffer2[i]='\0';
	strcat( buffer2, buffer );
	StrRight( buffer, buffer2, width );
}

int sprintGRSi( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit, int cplx ) { // + round
	int i,j,w,adj,minus=0,p,digit=round_digit,shift=0,eflag=0;
	char buffer2[32],fstr[16],tmp[16];
	double fabsnum,pw,log10fabsnum;
	unsigned char c;
	char *nptr;
	char *dptr,*eptr;
	double dpoint=0.01;

	if ( num < 0 ) minus=-1;
	if ( cplx ) { width--; minus=-1; }
	fabsnum=fabs(num);
	log10fabsnum=log10(fabsnum);
	switch ( round_mode ) {
		case Norm:
		  norm:
			if ( round_digit==1 ) { dpoint=0.01;        digit=10; }
			if ( round_digit==2 ) { dpoint=0.000000001; digit=10; }
			if ( round_digit==0 ) digit=16;
//			if ( round_digit==2 ) { dpoint=0.000000001; digit=10; i=11; c='f'; break; }
//			num = Round( num, round_mode, digit);
		 	w=15; if ( w > width )  w=width;
		 	pw=pow(10,w+minus);
			if ( ( fabsnum==0 ) || ( ( dpoint <= fabsnum ) && ( fabsnum < pw ) ) ) {
				w = floor(log10fabsnum) + (15-digit);
				if ( digit >= width ) w= w+(digit-width);
				i=14-w;
				if ( i >= 18 ) i=18;
				c='f';
				if ( i <  0  ) { i=digit-1; c='e'; }
				if ( i <  0  ) i=15;
				if ( round_digit==2 ) if ( i>11 ) i=11;
			} else {
				adj = 1 - minus+ floor(log10(fabs(log10fabsnum)))-1;
				if ( ( 1e-10 <= fabsnum ) && ( fabsnum < dpoint )) adj++;
				i=width-adj-5;
				if ( ( 1.0e-10 < fabsnum ) && ( fabsnum < 1.0 ) ) i++;	// adjust  1.23e-08->1.234e-8   
				if ( fabsnum >= pw ) i++;	// adjust  1.23e+09->1.234e+9  
				if ( i > digit-1 ) i=digit-1;
				if ( i >= 18 ) i=18;
				if ( i <  1  ) i=0;
				if ( round_digit==2 ) if ( i>11 ) i=11;
				c='e';
			}
			break;
		case Fix:
				num = Round( num, round_mode, digit);
				if ( num==0 ) minus=0;
				if ( fabsnum < 1e17 ) {
					i=digit;
					if ( fabsnum >=0 ) j=log10(fabsnum); else j=0;
					if (j+i>15) i=15-j;
					if (i<0) i=0;
					c='f';
					break;
				}
				round_mode=Norm; round_digit=0;
				goto norm;
		case Sci:
				num = Round( num, round_mode, digit);
				i=digit-1; if ( i < 0 ) i=15;
				c='e';
			break;
	}
	
	if ( c=='e' ) {	eflag=1; 	// 1.23456e-123
		if ( num != 0 ) num=num/( pow(10, floor(log10fabsnum)));
		c='f';
	}
	p=0;
	fstr[p++]='%';
	if ( cplx ) fstr[p++]='+';
	fstr[p++]='.';
	if ( i >= 10 ) fstr[p++]='0'+i/10;
	fstr[p++]='0'+i%10;
	fstr[p++]=c;
	fstr[p++]='\0';
	sprintf((char*)buffer, fstr, num);
	
	i=0; if ( ( buffer[0]=='-' )||( buffer[0]=='+' ) ) i=1;		// 0087.6 -> 87.6    -05.76 -> -5.76
	if ( buffer[i+0]=='0' ) { 
		if ( '0'==buffer[i+1] ) {
			if ( ('1'<=buffer[i+2])&&(buffer[i+2]<='9') ) shift=2;
		} else 
		if ( ('1'<=buffer[i+1])&&(buffer[i+1]<='9') ) shift=1;
		
		while ( buffer[i] != '\0' ) { 
			buffer[i]=buffer[i+shift];
			i++;
		}
	}
	if ( eflag ) {	// "1.23456" + "e-123"
		i=(int)floor(log10fabsnum);
		if ( abs(i)<=9 ) {
			if ( i>=0 ) sprintf(tmp, "e+0%d", abs(i)); else sprintf(tmp, "e-0%d", abs(i));
		} else {
			if ( i>=0 ) sprintf(tmp, "e+%d", abs(i)); else sprintf(tmp, "e-%d", abs(i));
		}
		strcat( buffer, tmp );
	}

	if ( round_mode == Norm ) {
		dptr=(char *)strchr((char*)buffer,'.');
		if ( dptr ) {
			eptr=(char *)strchr((char*)buffer,'e');
			i=strlen((char*)buffer); 
			nptr=buffer+i;
			if (  eptr != '\0' ) {	// 1.234500000e123  zero cut
				eptr--; i=0;
				while ( eptr[i] == '0' ) i-- ;
				if ( i ) {
					j=0;
					while ( eptr[j] != '\0' ) eptr[++i]=eptr[++j];
				}
			} else {				// 1.234500000  zero cut
				i=-1;
				while ( nptr[i] == '0' ) nptr[i--]='\0';
				if ( nptr[i]=='.' ) nptr[i]='\0';
			}
		}
	} else
	if ( ( round_mode == Fix ) && ( digit == 0 ) ) {
		i=strlen((char*)buffer); 
		buffer[i++]='.';
		buffer[i]  ='\0';
	}
	
	if ( ENG==3 )  {	// 3 digit separate
		for(i=0; i<22; i++) buffer2[i]=buffer[i]; // 
		nptr=(char *)strchr(buffer,'.');
		if ( nptr==NULL ) w=strlen(buffer)+minus; else w=nptr-buffer+minus;
		if ( w < 4  ) goto align;
		i=0; j=0;
		if ( minus ) buffer[(i++)]=buffer2[j++];
		do {
			buffer[i++]=buffer2[j++];
			w--;
			if ( ( w==3 ) || ( w==6 )|| ( w==9 )|| ( w==12 )|| ( w==15 ) ) buffer[(i++)] = ',';
		} while ( buffer2[j] ) ;
		buffer[i]='\0';
	}
  align:
	i=-1;
	if ( cplx ) {
		if ( buffer[0]=='-' ) buffer[++i]=0x99;	// (-)minus
		if ( (buffer[1]=='1')&&(buffer[2]=='\0') ) buffer[1]='\0';
	}
	while (i<width) {
		c=buffer[++i];
		switch ( c ) {
			case 0:
				goto exit;
			case '-':
				buffer[i]=0x87;	// (-)sign
				break;
			case '+':
				buffer[i]=0x89;	// (+)
				break;
			case 'e':	// exp
				buffer[i]=0x0F;	// (exp)
				if ( buffer[i+2]=='0' ) {	// adjust  1.23e+09->1.234e9  1.23e-08->1.234e-8   
					if ( buffer[i+1]=='+' ) {
						buffer[i+1]=buffer[i+3];
						buffer[i+2]='\0';
					} else {
						buffer[i+2]=buffer[i+3];
						buffer[i+3]='\0';
					}
				}
				break;
		}
	}
	buffer[width]='\0';
  exit:
	if ( cplx ) { 
		buffer[  i]=0x7f;	// (i)
		buffer[++i]=0x50;	// (i)
		buffer[++i]='\0';	//
	}

	if ( align_mode == RIGHT_ALIGN ) RightAlign( buffer, width );
	return strlen( buffer );
}


void sprintExpType( char* buffer, int width, int cplx, int input  ) { // + round  ENG +(x10)
	int c,i,j,w,minus;
	char fstr[8];
	i=-1;
	while (i<width) {
		c=buffer[(++i)];
		if ( c==0 ) break ;
		else
		if ( ( input==0 ) && ( c==0x0F ) ) {	// exp
			if ( CB_Round.ExpTYPE >= 1 ) {	//	1.23e+23  -> 1.23(x10)23
				j=i+1;
				if ( buffer[j++]==0xFFFFFF89 ) minus=0; else minus=1;
				c=buffer[j++]; w=0;
				while ( ('0'<=c)&&(c<='9') ) { fstr[w++]=c; c=buffer[j++]; }
				buffer[i++]=0xE6;		// x
				buffer[i++]=0xFC;
				buffer[i++]=0xE6;		// 10+ or 10-
				buffer[i++]=0xFD+minus;
				for ( j=0; j<w; j++) {
					buffer[i++]=0xE5;
					buffer[i++]=0xC0+fstr[j]-'0';
				}
				if ( cplx ) { 
					buffer[i++]=0x7f;	// (i)
					buffer[i++]=0x50;	// (i)
				}
				buffer[i]='\0';
				return ;
			}
		}
	}
}

int sprintGRS( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) { // + round
	return sprintGRSi( buffer, num, width, align_mode, round_mode, round_digit, 0);
}
void sprintGi( char* buffer, double num, int width, int align_mode) {
	sprintGRSi( buffer, num, width, align_mode, Norm, 15, 1);	// imag
}
void sprintG( char* buffer, double num, int width, int align_mode) {
	sprintGRS(buffer, num, width, align_mode, Norm, 15); // + round
	sprintExpType( buffer, width, 0, 0 );	// real
}

int sprintGRSiE( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit, int cplx ) { // + round +ENG
	double fabsnum;
	unsigned char c=0,d=0;
	int r;
	if ( ENG==1 ) { // ENG mode
		fabsnum=fabs(num);
		num = Round( num, round_mode, round_digit );
		if ( ( 0== fabsnum ) || ( ( 1e-15 <= fabsnum ) && ( fabsnum < 1e21 ) ) ) {
			if      ( fabsnum >= 1e18 ) { num/=1e18;  c=0x1B; }	//  Exa
			else if ( fabsnum >= 1e15 ) { num/=1e15;  c=0x0A; }	//  Peta
			else if ( fabsnum >= 1e12 ) { num/=1e12;  c=0x09; }	//  Tera
			else if ( fabsnum >= 1e09 ) { num/=1e09;  c=0x08; }	//  Giga
			else if ( fabsnum >= 1e06 ) { num/=1e06;  c=0x07; }	//  Mega
			else if ( fabsnum >= 1e03 ) { num/=1e03;  c=0x06; }	//  Kiro
			else if ( fabsnum >= 1    ) { if ( cplx==0 ) c=' ';  }
			else if ( fabsnum == 0 )    {                c=' ';  }	//  0
			else if ( fabsnum >= 1e-3 ) { num/=1e-3;  c=0x05; }	//  milli
			else if ( fabsnum >= 1e-6 ) { num/=1e-6;  c=0x04; }	//  micro
			else if ( fabsnum >= 1e-9 ) { num/=1e-9;  c=0x03; }	//  nano
			else if ( fabsnum >= 1e-12) { num/=1e-12; c=0x02; }	//  pico
			else if ( fabsnum >= 1e-15) { num/=1e-15; c=0x01; }	//  femto
			width-- ; 
			round_mode=Norm; round_digit=0;
			r = sprintGRSi( buffer, num, width, align_mode, round_mode, round_digit, cplx );
			width=strlen((char*)buffer);
			if ( ( cplx ) && ( buffer[width-1] == 0x50 ) && c ) { 
				width-=2;
				buffer[width++]=c;
//				if ( d ) buffer[width++]=d;
				buffer[width++]=0x7f;	// (i)
				buffer[width++]=0x50;	// (i)
			} else {
				buffer[width++]=c;
//				buffer[width++]=d;
			}
			buffer[width]='\0';
			return r;
		} else {
			if ( round_mode==Fix ) { round_mode=Norm; round_digit=0; }
		}
	}
	return sprintGRSi( buffer, num, width, align_mode, round_mode, round_digit, cplx );
}

void sprintGR( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) { // + round  ENG
	sprintGRSiE( buffer, num, width, align_mode, round_mode, round_digit, 0); // + round +ENG
}

void sprintGR2( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit, int type ) { // + round  ENG +(x10)
	int c,i,j,w,minus,oplen;
	char fstr[8];
	sprintGR( buffer, num, width, LEFT_ALIGN, round_mode, round_digit);
	if ( CB_Round.ExpTYPE >= type ) {	//	1.23e+23  -> 1.23(x10)23
		sprintExpType( buffer, width, 0, 0 );	// real
	}
	w=StrLen(buffer, &oplen);
	if ( w<=width ) if ( align_mode == RIGHT_ALIGN ) RightAlign( buffer, width );
}


void Cplx_sprintGR1s( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit, int cut, int input ) { // + round  ENG  + i
	char buffer2[64];
	char bufferR[]="\x7F\x54";
	double a,b,r,t;
	int i,w,oplen;
	a = num.real;
	b = num.imag;
	if ( ( CB_INT==0 ) || ( b==0 ) ) {
			sprintGRSiE( buffer,  a, width, LEFT_ALIGN, round_mode, round_digit, 0);	// real
			sprintExpType( buffer , width, 0, input );	// real
			goto next;
	} else {
		if ( ComplexMode == 2 ) { // r_theta
			r = fpolr( a, b);
			t = fpolt( a, b);
			if ( ErrorNo ) ErrorNo=0;
			sprintGRSiE( buffer,  r, width, LEFT_ALIGN, round_mode, round_digit, 0);	// r
			sprintGRSiE( buffer2, t, width, LEFT_ALIGN, round_mode, round_digit, 0);	// theta
			sprintExpType( buffer , width, 0, input );	// real
			sprintExpType( buffer2, width, 0, input );	// real
			strcat(buffer, bufferR );	// /_
			strcat(buffer, buffer2 );
		} else {	// a+bi
			sprintGRSiE( buffer,  a, width, LEFT_ALIGN, round_mode, round_digit, 0);	// real
			sprintGRSiE( buffer2, b, width, LEFT_ALIGN, round_mode, round_digit, 1);	// imag
			sprintExpType( buffer , width, 0, input );	// real
			sprintExpType( buffer2, width, 1, input );	// imag
			if ( a==0 ) { buffer[0]='\0'; 
				if ( buffer2[0]==0xFFFFFF99 ) buffer2[0]=0x87;	// (-)sign
				if ( buffer2[0]==0xFFFFFF89 ) { strcat( buffer, buffer2+1 ); goto next; }
			}
			strcat( buffer, buffer2 );
		}
	  next:
		w=StrLen(buffer, &oplen);
		if ( w<=width ) if ( align_mode == RIGHT_ALIGN ) RightAlign( buffer, width );
		else {
			if ( cut ) {	// limited
				while ( w+1>width ) PrevOpcodeGB( buffer, &w );
				buffer[w  ]='>';
				buffer[w+1]='\0';
			}
		}
	}
}
void Cplx_sprintGR1( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit ) { // + round  ENG  + i
	Cplx_sprintGR1s(buffer, num, width, align_mode, round_mode, round_digit, 0, 0 ); // + round
}
void Cplx_sprintG( char* buffer, complex num, int width, int align_mode ) {
	Cplx_sprintGR1s(buffer, num, width, align_mode, CB_Round.MODE, CB_Round.DIGIT, 0, 0 ); // + round
}
//void Cplx_sprintGR1cut( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit ) { // + round  ENG  + i+ limited
//	Cplx_sprintGR1s(buffer, num, width, align_mode, round_mode, round_digit, 1, 0 ); // + round  + limited
//}
//void Cplx_sprintGcut( char* buffer, complex num, int width, int align_mode ) {
//	Cplx_sprintGR1cut(buffer, num, width, align_mode, CB_Round.MODE, CB_Round.DIGIT ); // + round  + limited
//}
void Cplx_sprintGR1cutlim( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit ) { // + round  ENG  + i
	char buffer2[64];
	char buffer3[64];
	int k,oplen,rlen,ilen,rwidth,iwidth;
	int over=0;
	if ( width<0 ) { width=-width; over=1; }
	if ( ( CB_INT==0 ) || ( num.real==0 ) || (num.imag==0) ) {
		Cplx_sprintGR1( buffer, num, width, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
		OpcodeStringToAsciiString( buffer2, buffer, 64-1 );
	} else {
		sprintGRSiE( buffer, num.real, width, LEFT_ALIGN, round_mode, round_digit, 0);	// real
		sprintExpType( buffer , width,   0, 0 );	// real
		OpcodeStringToAsciiString( buffer2, buffer, 64-1 );
		rlen = StrLen( buffer2, &oplen );
		if ( num.imag ) sprintGRSiE( buffer, num.imag, width, LEFT_ALIGN, round_mode, round_digit, 1);	// imag
		else buffer[0]='\0';
		sprintExpType( buffer , width, 1, 0 );	// imag
		OpcodeStringToAsciiString( buffer3, buffer, 64-1 );
		ilen = StrLen( buffer3, &oplen );
		k = width/2;
		if ( ( rlen>k )&&( ilen>k ) ) {
			rwidth = k;
			iwidth = k;
			if ( rwidth+iwidth < width ) iwidth++;
		} else
		if ( rlen>k ) {
			iwidth = ilen;
			rwidth = width - iwidth;
		} else {
			rwidth = rlen;
			iwidth = width - rwidth;
		}
	  next:
	  	if ( over==0 ) {
			sprintGRSiE( buffer, num.real, rwidth, LEFT_ALIGN, round_mode, round_digit, 0);	// real
			sprintExpType( buffer , rwidth,   0, 0 );	// real
			OpcodeStringToAsciiString( buffer2, buffer, 64-1 );
			if ( num.imag ) sprintGRSiE( buffer, num.imag, iwidth, LEFT_ALIGN, round_mode, round_digit, 1);	// imag
			else buffer[0]='\0';
			sprintExpType( buffer , iwidth, 1, 0 );	// imag
			OpcodeStringToAsciiString( buffer3, buffer, 64-1 );
			if ( ( num.real==0 ) && ( num.imag!=0 ) ) {
				buffer2[0]='\0';
				if ( buffer3[0]==0xFFFFFF89 ) strcat( buffer2, buffer3+1 );
			} else	strcat( buffer2, buffer3 );
		} else {	// 1234567>
			strcat( buffer2, buffer3 );
			ilen = StrLen( buffer2, &oplen );
			if ( ilen > width ) {
				if ( buffer2[width-2]==0x7F ) width--;
				buffer2[width-1]='>';
				buffer2[width]='\0';
			}
		}
	}
	strcpy( buffer, buffer2);
	if ( align_mode == RIGHT_ALIGN ) RightAlign( buffer, width );
}

void Cplx_sprintGR2( char* buffer, char* buffer2, complex num, int width, int align_mode, int round_mode, int round_digit ) { // + round  ENG  + i
	char buffer3[64];
	char buffer4[64];
	char bufferR[]="\x7F\x54";
	double a,b,r,t;
	int i,w,oplen;
	a = num.real;
	b = num.imag;
	if ( ComplexMode == 3 ) { // >DMS
		if ( b==0 ) {	// real only
			StrDMSsub( buffer3, a ) ;
			w=1;
		} else {		// real + imag
			StrDMSsub( buffer,  a ) ;
			StrDMSsub( buffer3, b ) ;
			if ( b>=0 ) sprintf3( buffer2, "+%s\x7F\x50", buffer3 );
			else		sprintf3( buffer2, "-%s\x7F\x50", buffer3 );
			w=100;
		}
		goto GR2j;
	} else
	if ( ( CB_INT==0 ) || ( b==0 ) ) {
		sprintGR2( buffer, a, width, align_mode, round_mode, round_digit, 1 );	// real
		buffer2[0]='\0';
	} else { buffer[0]='\0';
		if ( ComplexMode == 2 ) { // r_theta
			r = fpolr( a, b);
			t = fpolt( a, b);
			if ( ErrorNo ) ErrorNo=0;
			sprintGRSiE( buffer,  r, width, LEFT_ALIGN, round_mode, round_digit, 0);	// r
			sprintGRSiE( buffer2, t, width, LEFT_ALIGN, round_mode, round_digit, 0);	// theta
			sprintExpType( buffer , width, 0, 0 );	// real
			sprintExpType( buffer2, width, 0, 0 );	// real
			buffer3[0]='\0';
			strcat(buffer3, bufferR );	// /_
			strcat(buffer3, buffer2 );
			strcpy(buffer2, buffer3 );

			buffer3[0]='\0';
			strcat(buffer3, buffer  );
			strcat(buffer3, buffer2 );
			
		} else {		// a+bi
			if ( a!=0 )	sprintGRSiE( buffer,  a, width, LEFT_ALIGN, round_mode, round_digit, 0);	// real
			if ( b!=0 ) sprintGRSiE( buffer2, b, width, LEFT_ALIGN, round_mode, round_digit, 1);	// imag
			else buffer2[0]='\0';
			sprintExpType( buffer , width, 0, 0 );	// real
			sprintExpType( buffer2, width, 1, 0 );	// imag

			if ( buffer2[0]==0xFFFFFF89 ) buffer2[0]='+';
			if ( buffer2[0]==0xFFFFFF99 ) buffer2[0]='-';
			buffer3[0]='\0';
			if ( a==0 ) {
				if ( buffer2[0]=='-' ) buffer2[0]=0x87;	// (-)sign
				if ( buffer2[0]=='+' ) strcat( buffer3, buffer2+1 ); else strcat( buffer3, buffer2 ); 
			} else {
				strcat(buffer3, buffer  );
				strcat(buffer3, buffer2 );
			}
		}
		w=StrLen(buffer3, &oplen);
	  GR2j:
		if ( w > 21 ) {		// 2 line 
			if ( align_mode == RIGHT_ALIGN ) {
				RightAlign( buffer , 21 );
				RightAlign( buffer2, 21 );
			}
			return ;
		}
		if ( align_mode == RIGHT_ALIGN ) RightAlign( buffer3 , 21 );
		strcpy(buffer, buffer3);	// 1 line 
		buffer2[0]='\0';
	}
}

void Cplx_sprintGR2SRC( char* SRC, char* buffer, char* buffer2, complex num, int width ) { // + round  ENG  + i
	int c=SRC[ExecPtr];
	int Cplx_bk=ComplexMode;
	if ( c==0xFFFFFFF9 ) {
		c=SRC[++ExecPtr];
		if ( c==0x05 ) { ExecPtr++; ComplexMode = 3; }	// >DMS
		else
		if ( c==0x06 ) { ExecPtr++; ComplexMode = 1; }	// >a+bi
		else
		if ( c==0x07 ) { ExecPtr++; ComplexMode = 2; }	// >r_theta
	}
	Cplx_sprintGR2(buffer, buffer2, num, width, RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
	if ( ComplexMode == 3 ) {
		CB_CurrentStr=NewStrBuffer(); if ( ErrorNo==0 ) { strcpy( CB_CurrentStr, buffer ); strcat( CB_CurrentStr, buffer2 ); }
	}
	ComplexMode=Cplx_bk;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
const unsigned short CharMATH[]= {
	0x002B,0x002D,0x00A9,0x00B9,0x005E,0x002A,0x002F,0x003D,0x0011,0x003C,0x003E,0x0010,0x0012,0x0087,0xE5BE,0xE5BF,0xE6B0,0xE6B1,0xE5A3,
	0xE5A4,0x7F50,0xE5B0,0x000F,0xE64F,0x7F53,0xE542,0x0086,0xE551,0xE54F,0xE6BB,0xE6B7,0xE6B8,0xE6B9,0xE5C0,0xE5C1,0xE5C2,0xE5C3,0xE5C4,
	0xE5C5,0xE5C6,0xE5C7,0xE5C8,0xE5C9,0xE5CA,0xE5CB,0xE5CC,0xE5CD,0xE5CE,0xE5CF,0xE5D0,0xE5D1,0xE5D2,0xE5D3,0xE5D4,0xE5D5,0xE5D6,0xE5D7,
	0xE5D8,0xE5D9,0xE5DA,0xE5DB,0xE5DC,0xE5DD,0xE5DE,0xE5DF,0x00C2,0x00C3,0x00CB,0x00CC,0x7FC7,0x7F54,0x008C,0x009C,0x00AC,0x00BC,0xE6BD,
	0xE6BE,0xE6BF,0xE6C0,0xE6C1,0xE6C2,0xE6C3,0xE6C4,0xE6C5,0xE6C6,0xE6C7,0xE6C8,0xE6C9,0xE6CA,0xE6CB,0xE6D6,0xE6CC,0xE6CD,0xE6CE,0xE6CF,
	0xE6D0,0xE6D1,0xE6D2,0xE6D3,0xE6D4,0xE6D5,0xE6B2,0xE6B3,0xE6B4,0xE6B5,0xE6BC,0xE6B6,0xE6D7,0xE6D8,0xE6D9,0xE6DA,0xE6DB,0xE6DC,0xE6DD,
	0xE6DE,0x00D8,0x0090,0x00CD,0x00CE,0xE5B1,0xE5B2,0xE5B3,0xE5B4,0xE5E4,0xE6FB,0xE6FC,0xE6FD,0xE6FE,0x0000 };
	
const unsigned short CharSYBL[]= {
	0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002C,0x002E,0x003A,0x003B,0x003F,0x0040,0x005B,0x005C,0x005D,0x005F,
	0x0060,0x007B,0x007C,0x007D,0x007E,0x0013,0x00B5,0x00BB,0xE594,0xE595,0xE596,0xE597,0xE598,0xE590,0xE591,0xE592,0xE593,0xE599,0xE59A,
	0xE59B,0xE59C,0xE59D,0xE59E,0xE5A1,0xE59F,0xE5A2,0xE5A0,0xE5A5,0xE5A6,0xE5A7,0xE5B5,0xE5B6,0xE5B7,0xE5B8,0xE5B9,0xE5BA,0xE5BB,0xE5BC,
	0xE690,0xE691,0xE692,0xE693,0xE694,0xE695,0xE696,0xE697,0xE698,0xE699,0xE69A,0xE69B,0xE69C,0xE69D,0xE69E,0xE69F,0xE6A0,0xE6A1,0xE6A2,
	0xE6A3,0xE6A4,0xE6A5,0xE6A6,0xE6A7,0xE6A8,0xE6A9,0xE6AA,0xE5AE,0xE5AF,0xE5E8,0xE5E9,0xE5EA,0xE5EB,0xE5AD,0xE5EC,0xE5ED,0xE5EE,0xE5EF,
	0xE5E0,0xE5E1,0xE5E2,0xE5E3,0xE5F0,0xE5F1,0xE5F2,0xE5F3,0xE5F4,0xE5F5,0xE5FB,0xE5FC,0xE5FD,0xE6AC,0xE6AD,0xE6AF,0xE6AE,0xE5AB,0xE5AC,0x0000 };

const unsigned short CharABT[]= {
	0xE540,0xE541,0xE542,0xE543,0xE544,0xE545,0xE546,0xE547,0xE548,0xE549,0xE54A,0xE54B,0xE54C,0xE54D,0xE54E,0xE54F,0xE550,0xE551,0xE553,
	0xE554,0xE555,0xE556,0xE557,0xE558,0xE501,0xE502,0xE503,0xE504,0xE505,0xE506,0xE507,0xE508,0xE509,0xE50A,0xE50B,0xE50C,0xE50D,0xE50E,
	0xE50F,0xE510,0xE511,0xE512,0xE513,0xE514,0xE515,0xE516,0xE517,0xE518,0xE519,0xE51A,0xE51B,0xE51C,0xE51D,0xE51E,0xE520,0xE521,0xE522,
	0xE523,0xE524,0xE525,0xE526,0xE527,0xE528,0xE529,0xE52A,0xE52B,0xE52C,0xE52D,0xE52E,0xE52F,0xE530,0xE531,0xE532,0xE533,0xE534,0xE535,
	0xE560,0xE561,0xE562,0xE563,0xE564,0xE565,0xE566,0xE567,0xE568,0xE569,0xE56A,0xE56B,0xE56C,0xE56D,0xE56E,0xE56F,0xE570,0xE571,0xE572,
	0xE573,0xE574,0xE575,0xE576,0xE577,0xE578,0xE579,0xE57A,0xE57B,0xE57C,0xE57D,0xE57E,0xE580,0xE581,0xE582,0x0000 };

const unsigned short Charabr[]= {
	0xE640,0xE641,0xE642,0xE643,0xE644,0xE645,0xE646,0xE647,0xE648,0xE649,0xE64A,0xE64B,0xE64C,0xE64D,0xE64E,0xE64F,0xE650,0xE651,0xE652,
	0xE653,0xE654,0xE655,0xE656,0xE657,0xE658,0xE601,0xE602,0xE603,0xE604,0xE605,0xE606,0xE607,0xE608,0xE609,0xE60A,0xE60B,0xE60C,0xE60D,
	0xE60E,0xE60F,0xE610,0xE611,0xE612,0xE613,0xE614,0xE615,0xE616,0xE617,0xE618,0xE619,0xE61A,0xE61B,0xE61C,0xE61D,0xE61E,0xE61F,0xE620,
	0xE621,0xE622,0xE623,0xE624,0xE625,0xE626,0xE627,0xE628,0xE629,0xE62A,0xE62B,0xE62C,0xE62D,0xE62E,0xE62F,0xE630,0xE631,0xE632,0xE633,
	0xE634,0xE635,0xE660,0xE661,0xE662,0xE663,0xE664,0xE665,0xE666,0xE667,0xE668,0xE669,0xE66A,0xE66B,0xE66C,0xE66D,0xE66E,0xE66F,0xE670,
	0xE671,0xE672,0xE673,0xE674,0xE675,0xE676,0xE677,0xE678,0xE679,0xE67A,0xE67B,0xE67C,0xE67D,0xE67E,0xE680,0xE681,0xE682,0x0000 };

const unsigned short CharABC[]= {
	0xE741,0xE742,0xE743,0xE744,0xE745,0xE746,0xE747,0xE748,0xE749,0xE74A,0xE74B,0xE74C,0xE74D,0xE74E,0xE74F,0xE750,0xE751,0xE752,0xE753,
	0xE754,0xE755,0xE756,0xE757,0xE758,0xE759,0xE75A,0xE761,0xE762,0xE763,0xE764,0xE765,0xE766,0xE767,0xE768,0xE769,0xE76A,0xE76B,0xE76C,
	0xE76D,0xE76E,0xE76F,0xE770,0xE771,0xE772,0xE773,0xE774,0xE775,0xE776,0xE777,0xE778,0xE779,0xE77A,0x0020,0x0020,0x0020,0x0020,0x0020,
	0xE785,0xE788,0xE789,0xE790,0xE795,0xE796,0xE798,0xE79A,0xE79E,0xE7A2,0xE7A5,0xE7A6,0xE7AB,0xE7AC,0xE7AD,0xE7AE,0xE7AF,0x0000 };

const unsigned short CharKANA[]= {
	0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,0x0020,0x0020,0x0020,
	0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,0x0020,0x0020,0x0020,
	0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,0x0020,0x0020,0x0020,
	0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,0x0020,0x0020,0x0020,
	0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,0x0020,0x0020,0x0020,
	0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x0020,0x0020,0x0020,0x0020,
	0xFF80,0xFF81,0xFF82,0xFF83,0xFF84,0xFF85,0xFF86,0xFF87,0xFF88,0xFF89,0xFF8A,0xFF8B,0xFF8C,0xFF8D,0xFF8E,0xFF8F,0x0020,0x0020,0x0020,
	0xFF90,0xFF91,0xFF92,0xFF93,0xFF94,0xFF95,0xFF96,0xFF97,0xFF98,0xFF99,0xFF9A,0xFF9B,0xFF9C,0xFF9D,0xFF9E,0xFF9F,0x0020,0x0020,0x0020,
	0xFFA0,0xFFA1,0xFFA2,0xFFA3,0xFFA4,0xFFA5,0xFFA6,0xFFA7,0xFFA8,0xFFA9,0xFFAA,0xFFAB,0xFFAC,0xFFAD,0xFFAE,0xFFAF,0x0020,0x0020,0x0020,
	0xFFB0,0xFFB1,0xFFB2,0xFFB3,0xFFB4,0xFFB5,0xFFB6,0xFFB7,0xFFB8,0xFFB9,0xFFBA,0xFFBB,0xFFBC,0xFFBD,0xFFBE,0xFFBF,0x0020,0x0020,0x0020,
	0xFFC0,0xFFC1,0xFFC2,0xFFC3,0xFFC4,0xFFC5,0xFFC6,0xFFC7,0xFFC8,0xFFC9,0xFFCA,0xFFCB,0xFFCC,0xFFCD,0xFFCE,0xFFCF,0x0020,0x0020,0x0020,
	0xFFD0,0xFFD1,0xFFD2,0xFFD3,0xFFD4,0xFFD5,0xFFD6,0xFFD7,0xFFD8,0xFFD9,0xFFDA,0xFFDB,0xFFDC,0xFFDD,0xFFDE,0xFFDF,0x0020,0x0020,0x0020,
	0xFFE0,0xFFE1,0xFFE2,0x0000 };


unsigned short *oplist=(unsigned short *)CharMATH;
short CharPtr=0;
int GBbaseptr[]={0xA000,0xB000};
char selectGB=0;	// 1,2:GB   0x11,0x12;JIS

int EscapeGBcode( int opcode ) {
	int a,b;
	if ( GBcode ) {
		a = (opcode&0xFF00)>>8;
		b = (opcode&0x00FF);
		if ( ( (0xE5<=a)&&(a<=0xE7) ) || (a==0x7F) ) {
			return ( 0x5C005C00 + (a<<16) + b );	// escape GB code 4byte
		} else
		if ( (a==0)&&(CheckEscape1(b)) ) {
			return ( 0x5C00 + b );					// escape GB code 2byte
		}
	}
	return opcode ;
}

int SelectChar( int *ContinuousSelect ) {

	int cont=1, x,y,cx,cy,px,py,ptr;
	int scrl=((*ContinuousSelect)>>8)&0xFF;
	int key;
	char tmpbuf[18],buf[32];
	int opcode;
	int opNum=0, n ,i, H,L;
	int mini=((*ContinuousSelect)>>16)&0xFF;	// 0x10 GB font select mode 
	int selectcode=0;
	int bk_GBcode=GBcode;
	int bk_ExpTYPE=CB_Round.ExpTYPE;
	short CharBuf[96];
	char *extchar[]={"  ","ext A","ext G","ex AG","ext K","ex AK","ex GK","A G K"};
	char *Extchar[]={"  ","Ext A","Ext G","Ex AG","Ext K","Ex AK","Ex GK","A G K"};
	char *GBorJIS[]={" GB code ","JIS code "};
	char buf1[64],buf2[64];
	int GBtopptr,GBendptr,ofst;
	int selJIS;
	
	int StatusDisp = CB_StatusDisp;						// Save Status Area 
	int	StatusOS   = SysCalljmp(1,0,0,0,0x2B7);
	int StFlag;
	char StCol[2];
	char StInfo[12];
	char StStr[64]="";
	char *StStrPtr;
	StFlag = DefineStatusAreaFlags(2, 0, &StCol[0], &StCol[1]);
	StStrPtr = (char *)SysCalljmp( 1, 0, (int)&StInfo[0], 0, 0x2B9);
	strncpy( StStr, StStrPtr, 63 );

	*ContinuousSelect=(*ContinuousSelect)&0xFF;
	CB_Round.ExpTYPE=0;	//
	if ( ( selectGB ) && ( GBcode==0 ) ) { selectGB=0; CharPtr=0; }
	
	SaveDisp(SAVEDISP_PAGE1);		// --------

//	StatusArea_Run();
	if ( StatusDisp==0 ) {
		CB_SetStatusDisp( 1 );
		EnableDisplayStatusArea();
//		Bdisp_PutDisp_DD_stripe( 0, 23);
	}
	
	Cursor_SetFlashOff(); 		// cursor flashing off
	int backcolor=CB_BackColorIndex;
	int color=CB_ColorIndex;
	while (cont) {
		if ( selectGB ) {	// force C.Basic select mode
			switch ( mini ) {
				case 3:	// -3
					mini=0;
					break;
				case 1:	// +1
					mini=2;
			}
		}
		selJIS = (selectGB > 2);
		Bdisp_AllClr_VRAM3(0,191);
		CB_ColorIndex=COLOR_BLACK;		// black
		CB_BackColorIndex=COLOR_WHITE;	// white
		opNum=0;
		if ( CB_G1MorG3M==1 ) selectGB=0;
		if ( selectGB ) { 
			GBcode=2;
			opNum=95+15-1;
		} else {
			GBcode=0;
			while ( oplist[opNum++] ) ;
			opNum-=2;
		}
		if ( CharPtr > opNum ) CharPtr=opNum;
		if ( scrl == 0 ) if ( CharPtr>=19*6 ) scrl=1;
		if ( scrl == 1 ) if ( CharPtr>=19*7 ) scrl=2;
		if ( scrl == 2 ) if ( CharPtr>=19*8 ) scrl=3;
		if ( scrl == 3 ) if ( CharPtr>=19*9 ) scrl=4;
		if ( scrl == 4 ) if ( CharPtr>=19*10) scrl=5;
		if ( scrl == 5 ) if ( CharPtr>=19*11) scrl=6;
		if ( scrl == 6 ) if ( CharPtr>=19*12) scrl=7;
		if ( scrl == 7 ) if ( CharPtr< 19*7 ) scrl=6;
		if ( scrl == 6 ) if ( CharPtr< 19*6 ) scrl=5;
		if ( scrl == 5 ) if ( CharPtr< 19*5 ) scrl=4;
		if ( scrl == 4 ) if ( CharPtr< 19*4 ) scrl=3;
		if ( scrl == 3 ) if ( CharPtr< 19*3 ) scrl=2;
		if ( scrl == 2 ) if ( CharPtr< 19*2 ) scrl=1;
		if ( scrl == 1 ) if ( CharPtr< 19   ) scrl=0;
		Bdisp_AllClr_VRAM1();
		if ( selectGB ) {	// GB or JIS
			ofst=0;
			if ( selJIS ) ofst= -0x80;
			switch ( mini ) {
				case 2:
				case 1:
					if ( *ContinuousSelect ) {
						sprintf3( buf, "===Continuous Select=%s0x%2X** ==", GBorJIS[selJIS], (GBbaseptr[selJIS]>>8) +ofst );
						CB_PrintMinix3( 1,1, (unsigned char*)buf, MINI_REV  );
					} else {
						sprintf3( buf, "===Character Select==%s0x%2X** ==", GBorJIS[selJIS], (GBbaseptr[selJIS]>>8) +ofst );
						CB_PrintMinix3( 1,1, (unsigned char*)buf, MINI_OVER  );
					}
					break;
				default:
					sprintf3( buf, "==%s0x%2X** ==", GBorJIS[selJIS], (GBbaseptr[selJIS]>>8) +ofst );
					locate(1,1);
					if ( *ContinuousSelect ) {
					    PrintRevs((unsigned char*)buf);
					} else {
						Prints((unsigned char*)buf);
					}
			}
		} else {
			switch ( mini ) {
				case 2:
					if ( *ContinuousSelect ) {
						CB_PrintMinix3( 1,1, (unsigned char*)"===Continuous Select= C.Basic =", MINI_REV  );
					} else {
						CB_PrintMinix3( 1,1, (unsigned char*)"===Character Select== C.Basic =", MINI_OVER  );
					}
					break;
				case 1:
					if ( *ContinuousSelect ) {
						CB_PrintMinix3( 1,1, (unsigned char*)"===Continuous Select= Default =", MINI_REV );
					} else {
						CB_PrintMinix3( 1,1, (unsigned char*)"===Character Select== Default =", MINI_OVER );
					}
					break;
				case 3:		// normal default
					if ( *ContinuousSelect ) {
						locate(1,1); PrintRevs((unsigned char*)"==Continuous Default=");
					} else {
						locate(1,1);    Prints((unsigned char*)"==Character Default==");
					}
					break;
				default:
					locate(1,1); 
					if ( *ContinuousSelect ) {
						PrintRevs((unsigned char*)"==Continuous C.Basic=");
					} else {
						Prints((unsigned char*)"==Character C.Basic==");
					}
			}
		}
		x=2; y=2;
		opcode=1;
		while ( opcode ) {
			CB_ColorIndex=COLOR_BLACK;
			GBcode=bk_GBcode;
			ptr=(y-2+scrl)*19+x-2 ;
			if ( selectGB ) {
				if ( x-2 > 15 ) goto next ;
				i=(y-2+scrl)*16+x-2 ;
				if ( (i+0xA0)==0xFF ) break;
				if ( ( selJIS ) && ( GBbaseptr[selJIS] >= 0xB000 )  ) {
					opcode = JIStoGB[(GBbaseptr[selJIS]>>8)-0xB0][i];	// JIS
					GBcode=2;
					if ( IsGBCode( opcode ) == 0 ) opcode=0xFFFF;
					GBcode=bk_GBcode;
				} else {
					opcode = GBbaseptr[selJIS]+i+0xA0;		// GB
				}
				if ( opcode==0xFFFF ) opcode=0xA1A1;
				n=8;
				GBcode=1;
				if ( IsGBCode( opcode ) == 0 ) CB_BackColorIndex=COLOR_CYAN;	// cyan
//				if ( IsGBCode( opcode ) == 0 ) CB_BackColorIndex=0xE71C;	// gray
				GBcode=2;
			} else {
				opcode = oplist[ ptr ];
				if ( ( mini != 1 ) && ( mini != 3 ) ) opcode = EscapeGBcode( opcode );
				n=6;
			}
			CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
			px = ((x-1)*n+1)*3-3; if ( selectGB ) px = ((x-2)*n)*3;
			py = ((y-1)*8+1)*3;
			if ( CharPtr == ptr ) selectcode = opcode;
			if ( (opcode&0xFF00) == 0x5C00 ) {
				CB_BackColorIndex=COLOR_YELLOW;	// Yellow
			}
			switch ( mini ) {
				case 2:		// mini C.Basic
					if ( CB_G1MorG3M==1 ) {
						if ( CharPtr == ptr )	CB_PrintMiniC_fx( px, py, (unsigned char*)tmpbuf , MINI_REV  | 0x00000100 );
						else 					CB_PrintMiniC_fx( px, py, (unsigned char*)tmpbuf , MINI_OVER | 0x00000100 );
					} else {
						if ( CharPtr == ptr )	CB_PrintMiniC( px, py, (unsigned char*)tmpbuf , MINI_REV  | 0x00000100 );
						else 					CB_PrintMiniC( px, py, (unsigned char*)tmpbuf , MINI_OVER | 0x00000100 );
					}
					break;
				case 1:		// mini default
						if ( CharPtr == ptr )	PrintMinix1( px, py, (unsigned char*)tmpbuf , MINI_REV, CB_ColorIndex, CB_BackColorIndex );
						else 					PrintMinix1( px, py, (unsigned char*)tmpbuf , MINI_OVER, CB_ColorIndex, CB_BackColorIndex  );
					break;
				case 3:		// normal default
					locate(x,y); 
					if ( CharPtr == ptr )	{
						PrintRevs((unsigned char*)tmpbuf );
					} else {
						Prints((unsigned char*)tmpbuf );
					}
					break;
					
				default:	// normal C.Basic
						if ( CharPtr == ptr )	CB_PrintModeC( px, py-3, (unsigned char*)tmpbuf, MINI_REV  | 0x00000100 );	// extflag
						else 					CB_PrintModeC( px, py-3, (unsigned char*)tmpbuf, MINI_OVER | 0x00000100 );	// extflag
					break;
			}
		  next:
			GBcode=bk_GBcode;
			CB_BackColorIndex=0xFFFF;	// white
			x++;
			if (x>20) { x=2; y++; if ( y>7 ) break ; }
//			Bdisp_PutDisp_DD();
		}
		CB_ColorIndex=COLOR_MAGENTA;	//
		if ( scrl ) {
			CB_Prints(21,2, (unsigned char*)"\xE6\x92"); // up
		} 
		if ( ( opNum/19- 6 - scrl >= 0 ) ) {
			CB_Prints(21,7, (unsigned char*)"\xE6\x93"); // dw
		}
		CB_ColorIndex=COLOR_BLACK;
		
		if ( selectGB ) {
			Fkey_Icon( FKeyNo1, 1032 );	//	Fkey_dispN( FKeyNo1, "|<<");
			Fkey_Icon( FKeyNo2, 1033 );	//	Fkey_dispN( FKeyNo2, " <<");
			Fkey_dispN( FKeyNo3, "\xE6\x9A");	//	Fkey_DISPN( FKeyNo4, " <");
			Fkey_dispN( FKeyNo4, "\xE6\x9B");	//	Fkey_DISPN( FKeyNo4, " >");
			Fkey_Icon( FKeyNo5, 1035 );	//	Fkey_dispN( FKeyNo5, " >>");
			Fkey_Icon( FKeyNo6, 1036 );	//	Fkey_dispN( FKeyNo5, " >>|");
		} else {
			Fkey_Icon( FKeyNo1, 120 );	//	Fkey_dispN( FKeyNo1, "MATH");
			Fkey_Icon( FKeyNo2, 308 );	//	Fkey_dispN( FKeyNo2, "SYBL");
			Fkey_Icon( FKeyNo3, 674 );	//	Fkey_dispN( FKeyNo3, "AB\xCD");
			Fkey_Icon( FKeyNo4, 307 );	//	Fkey_DISPN( FKeyNo4, "\xE6\x40\xE6\x41\xE6\x42");
			Fkey_Icon( FKeyNo5,1088 );	//	Fkey_dispN( FKeyNo5, " ABC");
//			Fkey_dispN( FKeyNo6, "Ext C");
			if ( CB_G1MorG3M==1 ) {
				switch ( mini ) {
					case 2:
					case 1:
						i = (ExtCharKanaMiniFX!=0)*4 + (ExtCharGaijiMiniFX!=0)*2 + (ExtCharAnkMiniFX!=0);
						break;
					default:
						i = (ExtCharKanaFX!=0)*4 + (ExtCharGaijiFX!=0)*2 + (ExtCharAnkFX!=0);
						break;
				}
				Fkey_dispN_ext( FKeyNo6, extchar[i], 1, 0, -1, COLOR_WHITE );
			} else {
				switch ( mini ) {
					case 2:
					case 1:
						i = (ExtCharKanaMini!=0)*4 + (ExtCharGaijiMini!=0)*2 + (ExtCharAnkMini!=0);
						break;
					default:
						i = (ExtCharKana!=0)*4 + (ExtCharGaiji!=0)*2 + (ExtCharAnk!=0);
						break;
				}
				Fkey_dispN_ext( FKeyNo6, Extchar[i], 1, 0, -1, COLOR_WHITE );
			}
			if ( *ContinuousSelect ) {
				if ( oplist == CharMATH ) n=0;
				if ( oplist == CharSYBL ) n=1;
				if ( oplist == CharABT  ) n=2;
				if ( oplist == Charabr  ) n=3;
				if ( oplist == CharABC  ) n=4;
				if ( oplist == CharKANA ) n=5;
//				Bdisp_AreaReverseVRAM( n*64+3, 195, n*64+60, 212);
				Fkey_AreaReverseVRAM( n, 3, 60 );	// reverse Fkey
			}
		}
		
		cx=(CharPtr%19)+2;
		cy=(CharPtr/19)+2-scrl;
		locate(cx,cy);

//		StatusArea_Time();
		switch ( selectGB ) {
			case 1:
			case 2:
				StatusArea_Run_sub( "GB select  [0]:Help", CB_INT, CB_G1MorG3M );
				break;
			case 0x11:
			case 0x12:
				StatusArea_Run_sub( "JIS select [0]:Help", CB_INT, CB_G1MorG3M );
				break;
			default:
				StatusArea_Run_sub( "Normal     [0]:Help", CB_INT, CB_G1MorG3M );
				break;
		}
		EnableDisplayStatusArea();
		GetKey_DisableMenuCatalog( &key );
//		EnableStatusArea(3);	// disable StatusArea

		GBtopptr = 0xA000 + (selJIS)*0x1000;
		GBendptr = 0xFE00 - (selJIS)*0xA00;

		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_EXIT:
			case KEY_CTRL_QUIT:
				*ContinuousSelect=0;
				selectcode = 0;
				goto exit;
				return 0;	// input cancel

			case KEY_CTRL_EXE:
			case KEY_CHAR_CR:
				cont=0;
				break;
			case KEY_CTRL_F1:	// CharMATH
				if ( selectGB ) {
					GBbaseptr[selJIS] = GBtopptr;
				} else {
					if (oplist==CharMATH) *ContinuousSelect=1-*ContinuousSelect;
					oplist=(unsigned short *)CharMATH;
				}
				break;
			case KEY_CTRL_F2:	// CharSYBL
				if ( selectGB ) {
					GBbaseptr[selJIS] -= 0x1000;
					if ( GBbaseptr[selJIS] < GBtopptr ) GBbaseptr[selJIS] = GBtopptr;
					if ( ( selJIS ) && ( 0xAC00<=GBbaseptr[selJIS] ) && ( GBbaseptr[selJIS]<0xB000 ) ) GBbaseptr[selJIS]=0xAB00;
				} else {
					if (oplist==CharSYBL) *ContinuousSelect=1-*ContinuousSelect;
					oplist=(unsigned short *)CharSYBL;
				}
				break;
			case KEY_CTRL_F3:	// CharABT
				if ( selectGB ) {
					GBbaseptr[selJIS] -= 0x100;
					if ( GBbaseptr[selJIS] < GBtopptr ) GBbaseptr[selJIS] = GBtopptr;
					if ( ( selJIS ) && ( 0xAC00<=GBbaseptr[selJIS] ) && ( GBbaseptr[selJIS]<0xB000 ) ) GBbaseptr[selJIS]=0xAB00;
				} else {
					if (oplist==CharABT) *ContinuousSelect=1-*ContinuousSelect;
					oplist=(unsigned short *)CharABT;
				}
				break;
			case KEY_CTRL_F4:	// Charabr
				if ( selectGB ) {
					GBbaseptr[selJIS] += 0x100;
					if ( GBbaseptr[selJIS] > GBendptr ) GBbaseptr[selJIS] = GBendptr;
					if ( ( selJIS ) && ( 0xAC00<=GBbaseptr[selJIS] ) && ( GBbaseptr[selJIS]<0xB000 ) ) GBbaseptr[selJIS]=0xB000;
				} else {
					if (oplist==Charabr) *ContinuousSelect=1-*ContinuousSelect;
					oplist=(unsigned short *)Charabr;
				}
				break;
			case KEY_CTRL_F5:	// CharABC
				if ( selectGB ) {
					GBbaseptr[selJIS] += 0x1000;
					if ( GBbaseptr[selJIS] > GBendptr ) GBbaseptr[selJIS] = GBendptr;
					if ( ( selJIS ) && ( 0xAC00<=GBbaseptr[selJIS] ) && ( GBbaseptr[selJIS]<0xB000 ) ) GBbaseptr[selJIS]=0xB000;
				} else {
					if (oplist==CharABC) *ContinuousSelect=1-*ContinuousSelect;
					oplist=(unsigned short *)CharABC;
				}
				break;
			case KEY_CTRL_F6:	// CharKANA
				if ( selectGB ) {
					GBbaseptr[selJIS] = GBendptr;
				} else {
					if (oplist==CharKANA) { *ContinuousSelect=1-*ContinuousSelect; break; }
					oplist=(unsigned short *)CharKANA;
					if ( CharPtr%19 > 15 ) CharPtr-=(CharPtr%19-15);
				}
				break;
			case 0x756E:	// SHIFT + <-
			case KEY_CTRL_LEFT:
				if ( ( ( selectGB ) || (oplist==CharKANA) ) && ( CharPtr%19 == 0 ) ) CharPtr-=3;
				CharPtr--; if ( CharPtr < 0 ) CharPtr=opNum;
				break;
			case 0x756F:	// SHIFT + ->
			case KEY_CTRL_RIGHT:
				if ( ( ( selectGB ) || (oplist==CharKANA) ) && ( CharPtr%19 > 14 ) ) CharPtr+=3;
				CharPtr++; if ( CharPtr > opNum ) CharPtr=0;
				break;
			case KEY_CTRL_UP:
			case KEY_CTRL_PAGEUP:
				CharPtr-=19; if ( CharPtr < 0 ) CharPtr=opNum;
				break;
			case KEY_CTRL_DOWN:
			case KEY_CTRL_PAGEDOWN:
				if ( CharPtr == opNum ) { CharPtr= 0; break; }
				CharPtr+=19; if ( CharPtr > opNum ) CharPtr=opNum;
				break;
			case KEY_SHIFT_OPTN:
			case KEY_CTRL_OPTN:
				switch ( mini ) {
					case 3:	// -2
						mini=1;
						break;
					case 0:	// +2
						mini=2;
						break;
					case 1:	// +2
						mini=3;
						break;
					case 2:	// -2
						mini=0;
				}
				break;
			case KEY_CTRL_PRGM:
			case KEY_CTRL_VARS:
				if ( selectGB ) break;
				switch ( mini ) {
					case 3:	// -3
						mini=0;
						break;
					case 0:	// +3
						mini=3;
						break;
					case 2:	// -1
						mini=1;
						break;
					case 1:	// +1
						mini=2;
				}
				break;
			case KEY_CHAR_SQUARE:	// Normal <-> GB
			case KEY_CHAR_ROOT:
			case KEY_CHAR_VALR:
				if ( bk_GBcode==0 ) break;
				if ( ( selectGB==1 ) || ( selectGB==2 ) ) {	// GB
					selectGB = 0; 
				} else {
					GBcode = bk_GBcode;
					selectGB = GBcode ;
					if ( CharPtr%19 > 15 ) CharPtr-=(CharPtr%19-15);
				}
				break;
			case KEY_CHAR_POW:		// Normal <-> JIS
			case KEY_CHAR_POWROOT:
			case KEY_CHAR_THETA:
				if ( bk_GBcode==0 ) break;
				if ( selectGB>2 ) {	// JIS
					selectGB = 0; 
				} else {
					GBcode = bk_GBcode;
					selectGB = GBcode | 0x10;
					if ( CharPtr%19 > 15 ) CharPtr-=(CharPtr%19-15);
				}
				break;
				
			case KEY_CTRL_SHIFT:
				*ContinuousSelect=1-*ContinuousSelect;
				break;
				
			case KEY_CHAR_0:
			case KEY_CHAR_IMGNRY:
			case KEY_CHAR_Z:
				Bdisp_AllClr_VRAM3(0,191);
				CB_ColorIndex=0x001F;		// blue
				CB_Prints( 1, 1, (unsigned char*)"==Char Select Help ==");
				CB_ColorIndex=0x0000;		// black
				locate(1,2);    Prints((unsigned char*)"[OPTN] \xE5\xE9");
				locate(1,4);    Prints((unsigned char*)"[ \x90\xE5\xC2 ] \xE5\xE9");
				locate(1,5);    Prints((unsigned char*)"[ ^  ] \xE5\xE9");
				locate(9,2);
				switch ( mini ) {
					case 1:	// mini default
					case 2:	// mini C.Basic
						Prints((unsigned char*)"Standard");
						break;
					case 0:	// normal C.Basic
					case 3:	// normal default
						locate(9,2); Prints((unsigned char*)"Mini Font");
						break;
				}
				
				if ( selectGB==0 ) {
					locate(1,3);    Prints((unsigned char*)"[VARS] \xE5\xE9");
					locate(9,3); 
					switch ( mini ) {
						case 0:	// normal C.Basic
						case 2:	// mini C.Basic
							Prints((unsigned char*)"Default Font");
							break;
						case 1:	// mini default
						case 3:	// normal default
							Prints((unsigned char*)"C.Basic Font");
							break;
					}
				}
				if ( selectGB ) {
					if ( selJIS ) {
						locate(9,4); Prints((unsigned char*)"GB select");
						locate(9,5); Prints((unsigned char*)"Normal");
					} else {
						locate(9,4); Prints((unsigned char*)"Normal");
						locate(9,5); Prints((unsigned char*)"JIS select");
					}
				} else {
						locate(9,4); Prints((unsigned char*)"GB select");
						locate(9,5); Prints((unsigned char*)"JIS select");
				}
				locate(5,7);    Prints((unsigned char*)"Press:[EXIT]");
				ExitKey();
				break;
		
			default:
				break;
		}
	}

	*ContinuousSelect += (scrl<<8) + (mini<<16);
  exit:
  
//	CB_SetStatusDisp( StatusDisp );
//	if ( *ContinuousSelect == 0 ) {
//		EnableDisplayStatusArea();
//		StatusArea_Run();
//		Bdisp_PutDisp_DD_stripe( 0, 23);
//	}
//	if ( StatusOS ==0 ) {
//		EnableDisplayStatusArea();	// enable StatusArea
//		Bdisp_PutDisp_DD_stripe( 0, 23);
//	}
//	if ( StatusDisp==0 ) {
//		EnableStatusArea(3);	// disable StatusArea
//	}

	DefineStatusAreaFlags( 3, StFlag, &StCol[0], &StCol[1]);			// Restore Status Area 
	SysCalljmp(2, (int)&StStr[0], (int)&StInfo[0], 0, 0x2B9);

	if ( StatusOS ) {
		CB_SetStatusDisp( 0 );
		EnableStatusArea(0);	// enable StatusArea
	} else
	if ( StatusDisp ) {
		CB_SetStatusDisp( 1 );
	} else {
		CB_SetStatusDisp( 0 );
		EnableStatusArea(3);	// disable StatusArea
	}

//	if ( *ContinuousSelect == 0 ) Bdisp_PutDisp_DD_stripe( 0, 23);
	Bdisp_PutDisp_DD_stripe( 0, 23);
	RestoreDisp(SAVEDISP_PAGE1);	// --------
//	Bdisp_PutDisp_DD();
	CB_ColorIndex=color;			// current color index restore
	CB_BackColorIndex=backcolor;	// current color index restore
	GBcode=bk_GBcode;
	CB_Round.ExpTYPE=bk_ExpTYPE;
	return ( selectcode );
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void DMS_Opcode( char * buffer, short code ) {
	if ( code == 0x9C ) { strcat( buffer,"(deg)"); }
	if ( code == 0xAC ) { strcat( buffer,"(rad)"); }
	if ( code == 0xBC ) { strcat( buffer,"(gra)"); }
}
//----------------------------------------------------------------------------------------------
toplistrecentfreq *OplistRecentFreq;
int *OplistRecent;
toplistrecentfreq OplistRecentFreqMem[OpRecentFreqMax];
int OplistRecentMem[OpRecentMax];

int qsort_OpRecentFreq( const void *p1, const void *p2 ){
	toplistrecentfreq *OpRecent1 = (toplistrecentfreq *)p1;
	toplistrecentfreq *OpRecent2 = (toplistrecentfreq *)p2;
	if ( OpRecent2->count == OpRecent1->count )	return OpRecent1->code - OpRecent2->code;
	return OpRecent2->count - OpRecent1->count;
}

void InitOpcodeRecent() {
	int i;
	for (i=0; i<OpRecentMax; i++) {
		OplistRecent[i]=0;
	}
}
void InitOpcodeRecentAll() {
	int i;
	for (i=0; i<OpRecentFreqMax-1; i++) {
		OplistRecentFreq[i].code =0;
		OplistRecentFreq[i].count=0;
	}
	InitOpcodeRecent();
}

void AddOpcodeRecent( int opcode ) {
	int i,j;

	if ( ( ('a'<=opcode)&&(opcode<='z') ) || ( 0x0D == opcode ) || ( 0x0E == opcode ) || ( ',' == opcode ) ) return;
	if ( MathKey( (unsigned int) opcode ) ) return ;
	
	for (i=0; i<OpRecentMax; i++) {
		if ( OplistRecent[i] == opcode ) {	// matching
			i++;
			break;
		}
	}
	i--;
	for (j=i; j>0; j--) OplistRecent[j] = OplistRecent[j-1];
	OplistRecent[0] = opcode;	// top of recent code
	
	for (i=0; i<OpRecentFreqMax; i++) {
		if ( OplistRecentFreq[i].code == opcode ) {	// matching
			OplistRecentFreq[i].count++;
			break;
		}
	}
	if ( i >= OpRecentFreqMax ) {
//		i=OpRecentFreqMax-1;
//		j=OplistRecentFreq[i].count;
//		while ( OplistRecentFreq[i].count == j ) i--;
//		for ( j=i+1; j<OpRecentFreqMax-1; j++ ) {
//			OplistRecentFreq[j].code  = OplistRecentFreq[j+1].code;
//			OplistRecentFreq[j].count = OplistRecentFreq[j+1].count;
//		}
		i=OpRecentFreqMax-1;
		OplistRecentFreq[i].code =opcode;
		OplistRecentFreq[i].count=1;
	}
	qsort( OplistRecentFreq,  OpRecentFreqMax, sizeof(toplistrecentfreq),  qsort_OpRecentFreq );
}

int SelectOpcodeRecent( int listselect ) {
	int select;
	int opNum;
	char buffer[22];
	char tmpbuf[18];
	int key;
	int	cont,cont2=1;
	int i,j,k,y;
	int seltop;
	int bk_CB_ColorIndex=CB_ColorIndex;						// current color index 
	unsigned short bk_CB_BackColorIndex=CB_BackColorIndex;	// current backcolor index 
	unsigned short bk_CB_ColorIndexPlot=CB_ColorIndexPlot;	// Plot color index (default blue)
	int g1mg3m=CB_G1MorG3M;
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// White
	CB_ColorIndexPlot=0x001F;	// Blue
	CB_G1MorG3M=3;

	select=0;
	seltop=0;
	
	while ( cont2 ) {
		opNum=0 ;
		if ( listselect == CMDLIST_RECENT ) {
			while ( OplistRecent[opNum++] );
		} else {
			while ( OplistRecentFreq[opNum++].code );
		}
		opNum-=1;
		seltop=select;
		cont=1;
		Cursor_SetFlashOff(); 		// cursor flashing off

		SaveDisp(SAVEDISP_PAGE1);
		PopUpWin(6);

		while (cont) {
			if (  select<seltop ) seltop=select;
			if ( (select-seltop) > 5 ) seltop=select-5;
			if ( (opNum-seltop) < 5 ) seltop = opNum-5; 
			if ( seltop < 0 ) seltop=0;
			for ( i=0; i<6; i++ ) {
				CB_Prints(3,2+i,(unsigned char *)"                 ");
				sprintf3(buffer, "F%d:", i+1 ) ;
				CB_ColorIndex=CB_ColorIndexEditLine;	//
//				CB_ColorIndex=COLOR_BLACK;
				CB_PrintMinix3( 13, 8*i+10, (unsigned char *)buffer, MINI_OVER ) ;
				if ( listselect == CMDLIST_RECENT ) {
					j=OplistRecent[seltop+i];
				} else {
					j=OplistRecentFreq[seltop+i].code;
				}
				CB_OpcodeToStr( j, tmpbuf ) ; // SYSCALL
				tmpbuf[12]='\0'; 
				DMS_Opcode( tmpbuf, j);
				j=0; if ( tmpbuf[0]==' ' ) j++;
				if ( listselect == CMDLIST_RECENT ) {
					sprintf3(buffer, "%-15s", tmpbuf+j ) ;
				} else {
					k=OplistRecentFreq[seltop+i].count;
					sprintf3(buffer, "%s (%d)", tmpbuf+j, (unsigned short)k ) ;
					if ( k==0 ) buffer[0]='\0';
				}
//				CB_ColorIndex=CB_ColorIndexEditCMD;	//
				CB_ColorIndex=COLOR_BLACK;
				CB_PrintMode( 4*18, (1+i)*24, (unsigned char*)buffer, MINI_OVER | (0x100*EditExtFont) | 0x10000000, 332 );
			}
//			Bdisp_PutDisp_DD();	
			CB_ColorIndex=COLOR_MAGENTA;	//
			if ( seltop <= opNum-7 ) { CB_Prints(19,7, (unsigned char*)"\xE6\x93"); } // dw
			if ( seltop >= 1 )       { CB_Prints(19,2, (unsigned char*)"\xE6\x92"); } // up
			CB_ColorIndex=COLOR_BLACK;

			y = (select-seltop) + 1 ;
			Bdisp_AreaReverseVRAMx3(12, y*8, 113, y*8+7);	// reverse *select line 
//			Bdisp_PutDisp_DD();

			GetKey_DisableMenuCatalog(&key);
			switch (key) {
				case KEY_CTRL_EXIT:
				case KEY_CTRL_QUIT:
				case KEY_CTRL_MENU:
					RestoreDisp(SAVEDISP_PAGE1);
					j=0;
					goto exit;
					
				case KEY_CTRL_EXE:
					cont=0;
					cont2=0;
					break;
				case KEY_CTRL_F1:
				case KEY_CHAR_1:
				case KEY_CHAR_U:
					select=seltop;
					cont=0;
					cont2=0;
					break;
				case KEY_CTRL_F2:
				case KEY_CHAR_2:
				case KEY_CHAR_V:
					select=seltop+1;
					cont=0;
					cont2=0;
					break;
				case KEY_CTRL_F3:
				case KEY_CHAR_3:
				case KEY_CHAR_W:
					select=seltop+2;
					cont=0;
					cont2=0;
					break;
				case KEY_CTRL_F4:
				case KEY_CHAR_4:
				case KEY_CHAR_P:
					select=seltop+3;
					cont=0;
					cont2=0;
					break;
				case KEY_CTRL_F5:
				case KEY_CHAR_5:
				case KEY_CHAR_Q:
					select=seltop+4;
					cont=0;
					cont2=0;
					break;
				case KEY_CTRL_F6:
				case KEY_CHAR_6:
				case KEY_CHAR_R:
					select=seltop+5;
					cont=0;
					cont2=0;
					break;
			
				case KEY_CTRL_LEFT:
				case KEY_CTRL_RIGHT:
					if ( listselect == CMDLIST_RECENT ) {
						listselect = CMDLIST_RECENTFREQ;
					} else {
						listselect = CMDLIST_RECENT;
					}
					select=0;
					cont=0;
					break;
				case KEY_CTRL_UP:
					select--;
					if ( select < 0 ) select = opNum-1;
					if ( select < 0 ) select = 0;
					break;
				case KEY_CTRL_DOWN:
					select++;
					if ( select >= opNum ) select =0;
					break;
					
				case KEY_CTRL_AC:
					if ( YesNo_NoSave( "Delete History ?" ) ) InitOpcodeRecentAll();
					select=0;
					cont=0;
					break;
				default:
					break;
			}
		}
		RestoreDisp(SAVEDISP_PAGE1);
		j=1;
	}
	exit:
//	Bdisp_PutDisp_DD();
	CB_G1MorG3M=g1mg3m;
	CB_ColorIndex    =bk_CB_ColorIndex;		// current color index 
	CB_BackColorIndex=bk_CB_BackColorIndex;	// current backcolor index 
	CB_ColorIndexPlot=bk_CB_ColorIndexPlot;	// Plot color index (default blue)

	if ( j ) {
		if ( listselect == CMDLIST_RECENT ) {
			j=OplistRecent[select];
		} else {
			j=OplistRecentFreq[select].code;
		}
	}
	return j;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

int SelectOpcode( int listselect, int flag ) {
	short *select;
	short *oplist;
	int opNum;
	char buffer[22];
	char tmpbuf[18];
	int key;
	int	cont,cont2=1;
	int i,j,y,r;
	int seltop;
	int bk_CB_ColorIndex=CB_ColorIndex;						// current color index 
	unsigned short bk_CB_BackColorIndex=CB_BackColorIndex;	// current backcolor index 
	unsigned short bk_CB_ColorIndexPlot=CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// White
	CB_ColorIndexPlot=0x001F;	// Blue

	while ( cont2 ) {

		switch ( listselect ){
			case CMDLIST_OPTN:
				oplist = (short *)oplistOPTN;
				select =&selectOPTN;
				break;
			case CMDLIST_VARS:
				oplist = (short *)oplistVARS;
				select =&selectVARS;
				break;
			case CMDLIST_PRGM:
				oplist = (short *)oplistPRGM;
				select =&selectPRGM;
				break;
		}
		
		opNum=0 ;
		while ( oplist[opNum++] ) ;
		opNum-=2;
		seltop=*select;
		cont=1;

		Cursor_SetFlashOff(); 		// cursor flashing off

		SaveDisp(SAVEDISP_PAGE1);
		PopUpWin(6);

		while (cont) {
			if (  (*select)<seltop ) seltop=(*select);
			if ( ((*select)-seltop) > 5 ) seltop=(*select)-5;
			if ( (opNum-seltop) < 5 ) seltop = opNum-5; 
			for ( i=0; i<6; i++ ) {
				CB_Prints(3,2+i,(unsigned char *)"                 ");
				j=oplist[seltop+i];
				if ( j == 0xFFFFFFFF ) {
					CB_Prints(3,2+i,(unsigned char *)"-----------------");
				} else {
					CB_OpcodeToStr( j, tmpbuf ) ; // SYSCALL
					tmpbuf[12]='\0'; 
					DMS_Opcode( tmpbuf, j);
					j=0; if ( tmpbuf[0]==' ' ) j++;
//	//				sprintf3(buffer,"%04X:%-12s",(unsigned short)oplist[seltop+i],tmpbuf+j ) ;
					sprintf3(buffer,"%-17s",tmpbuf+j ) ;
					CB_Prints(3,2+i,(unsigned char *)buffer);
				}
			}
//			Bdisp_PutDisp_DD();	
			
			CB_ColorIndex=COLOR_MAGENTA;	//
			if ( seltop <= opNum-6 ) { CB_Prints(19,7,(unsigned char*)"\xE6\x93"); } // dw
			if ( seltop >= 1 )       { CB_Prints(19,2,(unsigned char*)"\xE6\x92"); } // up
			CB_ColorIndex=COLOR_BLACK;
			
			y = ((*select)-seltop) + 1 ;
			Bdisp_AreaReverseVRAMx3(12, y*8, 113, y*8+7);	// reverse *select line 
//			Bdisp_PutDisp_DD();

			GetKey_DisableMenuCatalog(&key);
			switch (key) {
				case KEY_CTRL_EXIT:
				case KEY_CTRL_QUIT:
					RestoreDisp(SAVEDISP_PAGE1);
					if ( flag ) { r = 0x10000; goto exit; }
					{ r=0; goto exit; }
					
				case KEY_CTRL_EXE:
					cont=0;
					cont2=0;
					break;
			
				case KEY_CTRL_LEFT:
					for ( i=(*select)-2; i>0; i-- ) {
						if ( oplist[i] == 0xFFFFFFFF ) break;
					}
					if ( i<0 ) i = 0 ;
					if ( i>0 ) i++ ;
					*select = i ;
					seltop = *select;
					break;
				case KEY_CTRL_RIGHT:
					for ( i=(*select)+1; i<(*select)+opNum; i++ ) {
						if ( oplist[i] == 0xFFFFFFFF ) break;
					}
					*select = i+1 ;
					if ( *select > opNum ) *select = opNum;
					seltop = *select;
					break;
				case KEY_CTRL_UP:
					(*select)--;
					if ( oplist[(*select)] == 0xFFFFFFFF ) (*select)--;
					if ( *select < 0 ) *select = opNum;
					break;
				case KEY_CTRL_DOWN:
					(*select)++;
					if ( oplist[(*select)] == 0xFFFFFFFF ) (*select)++;
					if ( *select > opNum ) *select =0;
					break;
				case KEY_CTRL_OPTN:
					listselect=CMDLIST_OPTN;
					cont=0;
					break;
				case KEY_CTRL_VARS:
					listselect=CMDLIST_VARS;
					cont=0;
					break;
				case KEY_CTRL_SHIFT:
					GetKey_DisableMenu_pushpop(&key);
					switch (key) {
						case KEY_CTRL_PRGM:
							listselect=CMDLIST_PRGM;
							cont=0;
							break;
						default:
							break;
					}
					break;
				case KEY_CTRL_F3:	// :
					RestoreDisp(SAVEDISP_PAGE1);
					if ( flag ) { r=0; goto exit; }
					key=SelectOpcode5800P( 1 );
					if ( key > 0x10000 ) { 
						listselect=key-0x10000; key=0;
						cont=0;
					}
					if ( key ) { r = key & 0xFFFF; goto exit; }
				break;
				default:
					break;
			}
		}
		RestoreDisp(SAVEDISP_PAGE1);
	}
//	Bdisp_PutDisp_DD();
	r = oplist[(*select)]  & 0xFFFF;
  exit:
	CB_ColorIndex    =bk_CB_ColorIndex;		// current color index 
	CB_BackColorIndex=bk_CB_BackColorIndex;	// current backcolor index 
	CB_ColorIndexPlot=bk_CB_ColorIndexPlot;	// Plot color index (default blue)
	return r;
}


//----------------------------------------------------------------------------------------------
//	CATALOG for C.Basic version
//----------------------------------------------------------------------------------------------

const short catalog_opcode[]={
	
//						A
		0xF906, // >a+bi
		0x97,	// Abs
		0xF717,	// ACBreak
		0xF90F,	// AliasVar
		0x7FB0,	// And
		0xBA,	// and
		0xF96D, // Angle(	ver.2.04~
		0x7F22,	// Arg 
		0x7F49,	// Augment(
		0xF7D2,	// AxesOff
		0xF7C2,	// AxesOn
		
//						B
		0xF9BE,	// Back-Color
		0xF7FE,	// BackLight
		0xF7DE,	// BatteryStatus
		0xF7DD,	// Beep
		0xF778,	// BG-None
		0xF779,	// BG-Pict
		0xF947,	// Bin(
		0xF9DC,	// BmpLoad(
		0xF9DB,	// BmpSave 
		0xF70D,	// Break

//						C
		0xF7F5,	// Call
		0xF7EB,	// Case
		0x7FE9,	// CellSum(
		0xF714,	// CloseComport38k
		0xF7A6,	// Circle
		0xF719,	// ClrGraph
		0xF71A,	// ClrList
		0xF91E,	// ClrMat
		0xF93E,	// ClrVct	
		0xF718,	// ClrText
		0xD1,	// Cls
//		0xF973,	// ColorAuto 
//		0xF97D,	// ColorClr 
//		0xF974,	// ColorLighter 
//		0xF97A,	// ColorNormal 
//		0xF978,	// ColorLinkOn
//		0xF979,	// ColorLinkOff
//		0xF975,	// ColorLinkX&Y
//		0xF976,	// ColorLinkOnlyX
//		0xF977,	// ColorLinkOnlyY
//		0xF97E,	// ColorLinkX&Freq
		0x7F5A,	// ColSize(
		0x7F23,	// Conjg 
		0xF90E,	// Const 
		0xF7D3,	// CoordOff
		0xF7C3,	// CoordOn
		0xA2,	// cosh
		0xB2,	// arccosh
		0xF74E,	// Cross
		0xF94A, // CrossP(	ver.2.04~

//						D
		0xF941,	// DATE
		0xF7EC,	// Default
		0xF7DF,	// Delete 
//		0xF90D,	// Define 
		0xDA,	// Deg
		0xF7D5,	// DerivOff
		0xF7C5,	// DerivOn
		0x7F21, // Det
		0x7F46,	// Dim
		0xF7E4,	// Disp
		0xF905,	// >DMS
		0xF70A,	// Do
		0xF74F,	// Dot
		0xF73E,	// DotGet(
		0xF7E0,	// DotLife(
		0xF94B, // DotP("
		0xF73B,	// DotPut(
		0xF7F0,	// DotShape(
		0xF73D,	// DotTrim(
		0xF720,	// DrawGraph
		0xF9DD,	// DrawMat
		0xF723,	// DrawStat
		0xF7DC,	// DrawOff
		0xF7CC,	// DrawOn
		0xE8,	// Dsz

//						E
		0x7F58,	// ElemSize(
		0xF702,	// Else
		0xF70F,	// ElseIf
		0xDD,	// Eng
		0xF90C,	// EngOff
		0xF90B,	// EngOn
		0xF738,	// Except
		0xF937,	// Exp>Str(
		0xF938,	// Exp(

//						F
		0xF7A7,	// F-Line
		0x7F47,	// Fill(
		0xF7E2,	// FillRect(
		0xE3,	// Fix
		0xF7FD, // FKeyMenu(
		0xF91B,	// fn
		0xF704,	// For
		0xB6,	// frac

//						G
		0xF770,	// G-Connect
		0xF771,	// G-Plot
		0x7F3C,	// GCD(
		0xF960,	// GetFont(
		0xF962,	// GetFOntMini 
		0x7F72,	// GetHSL(
		0x7F70,	// GetHSV(
		0x7F8F,	// Getkey
		0x7F5D,	// GetRGB(
		0xFA,	// Gosub
		0xEC,	// Goto
		0xDC,	// Grad
		0xEE,	// Graph Y=
		0xF5,	// Graph(X,Y)=(
		0x7FF0,	// GraphY
		0xF77A,	// GridOff
		0xF77D,	// GridOn

//						H
		0xF946,	// Hex(
		0xF7A4,	// Horizontal
		0x7F73,	// HSL( 
		0x7F71,	// HSV(

//						I
		0x7F48,	// Identity
		0xF700,	// If
		0xF703,	// IfEnd
		0x7F25,	// ImP 
		0xA6,	// Int
		0xDE,	// Intg
		0x7FBC,	// Int/
		0x7FF5,	// IsExist(
		0xE9,	// Isz

//						J

//						K
		0x7F9F,	// KeyRow(

//						L
		0xF7C4,	// LabelOn
		0xF7D4,	// LabelOff
		0xE2,	// Lbl
		0x7F3D,	// LCM(
		0xE1,	// Line
		0x7F4A,	// List->Mat(
		0x7F5C,	// ListCmp(
		0xF7EF,	// Load(
		0xF7F1,	// Local
		0xF710,	// Locate
		0xF7E3,	// LocateYX
		0x7F85,	// logab(
		0xF70B,	// LpWhile

//						M
		0x7F5B,	// MatBase(
		0x7F4B,	// Mat->List(
		0x7F20,	// Max(
		0x7F2E,	// Mean(
		0xF79E,	// Menu
		0x7F2D,	// Min(
		0x7F3A,	// MOD(
		
//						N
		0XF707,	// Next
		0x7FB3,	// Not
		0xA7,	// not
		0xD9,	// Norm
		0xF95B, // Norm("	ver.2.04~

//						O
		0xF713,	// OpenComport38k
		0x7FB1,	// Or
		0xAA,	// or

//						P
		0x7FF6,	// Peek(
		0xE0,	// Plot
		0xF7AA,	// PlotChg
		0xF7A9,	// PlotOff
		0xF7A8,	// PlotOn
		0xF7F6,	// Poke 
		0x80,	// Pol(
		0xF7F2,	// PopUpWin(
		0x7F4D,	// Prod
		0xED,	// Prog
		0x7FFA,	// ProgPtr(
		0xF7FC,	// PutDispDD
		0xF7AD,	// PxlChg
		0xF7AC,	// PxlOff
		0xF7AB,	// PxlOn
		0xF7AF,	// PxlTest(

//						Q

//						R
		0xDB,	// Rad
		0xC1,	// Ran#
		0x7F89,	// RanBin#(	
		0x7F87,	// RanInt#(	
		0x7F88,	// RanList#(
		0x7F8A,	// RanNorm#(	
		0xF79F,	// RclCapt
		0xF794,	// RclPict
		0xF798,	// RclV-Win
		0xF7E8,	// ReadGraph(
		0xA0,	// Rec(
		0xF712,	// Receive(
		0xF716,	// Receive38k 
		0xF7E1,	// Rect(
		0x7F55,	// Ref 
		0xF7F8,	// RefreshCtrl
		0xF7FA,	// RefreshTime
		0x7F24,	// ReP 
		0xF70C,	// Return
		0x7F5E,	// RGB(
		0xF907, // >re^Theta
		0x7FBD,	// Rmdr
		0xD3,	// Rnd
		0x7F86,	// RndFix(
		0x7F44,	// Row+
		0x7F59,	// RowSize(
		0x7F42,	// *Row
		0x7F43,	// *Row+
		0x7F56,	// Rref 


//						S
		0xF74A,	// S-Gph1
		0xF74B,	// S-Gph2
		0xF74C,	// S-Gph3
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		0xF7EE,	// Save
		0xF750,	// Scatter
		0xE4,	// Sci
		0xF711,	// Send(
		0xF715,	// Send38k 
		0x7F2C,	// Seq(
		0xF961,	// SetFont
		0xF963,	// SetFontMini
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF7FB,	// Screen
		0xF961,	// SetFont
		0xF963,	// SetFOntMini
		0x7F29,	// Sigma(
		0xA1,	// sinh
		0xB1,	// arcsinh
		0xF7B0,	// SortA(
		0xF7B1,	// SortB(
		0xF943,	// Sprintf(
		0xF74D,	// Square
		0xF706,	// Step
		0xF79D,	// StoCapt
		0xF70E,	// Stop
		0xF793,	// StoPict
		0xF797,	// StoV-Win
		0xF93F,	// Str
		0xF950,	// StrAsc(
		0xF948,	// StrBase(
		0xF945,	// StrCenter(
		0xF944,	// StrChar(
		0xF932,	// StrCmp(
		0xF93B,	// StrInv(
		0xF930,	// StrJoin(
		0xF934,	// StrLeft(
		0xF931,	// StrLen
		0xF93A,	// StrLwr(
		0xF936,	// StrMid(
		0xF935,	// StrRight(
		0xF93D,	// StrRotate(
		0xF93C,	// StrShift(
		0xF933,	// StrSrc(
		0xF939,	// StrUpr(
		0xF949,	// StrRepl(	
		0xF94D,	// StrSplit(	
		0x7F4C,	// Sum
		0x7F45,	// Swap
		0xF7EA,	// Switch
		0xF7ED,	// SwitchEnd
		0xF7F4,	// SysCall
		0x7FCF,	// System(
		
//						T
		0xA3,	// tanh
		0xB3,	// arctanh
		0xF7A5,	// Text
		0x7F5F,	// Ticks
		0xF942,	// TIME
		0xF701,	// Then
		0xF705,	// To
		0xF940,	// ToStr(
		0xF9BF,	// Transp-Color
		0x7F41,	// Trn 
		0xF737, // Try
		0xF739,	// TryEnd
		0x7F09,	// TThetamax
		0x7F08,	// TThetamin
		0x7F0A,	// TThetaptch

//						U
		0xF95E, // UnitV(	ver.2.04~

//						V
		0x7FF8,	// VarPtr(
		0x7FDF,	// Version
		0xF7A3,	// Vertical
		0xEB,	// ViewWindow

//						W
		0xF94F,	// Wait 
		0xF708,	// While
		0xF709,	// WhileEnd
		0xF7E9,	// WriteGraph(

//						X
		0xF921,	// Xdot
		0x7F0B,	// Xfct
		0x7F01,	// Xmax
		0x7F00,	// Xmin
		0x7FB4,	// Xor
		0x9A,	// xor
		0x7F02,	// Xscl
		0xF751,	// xyLine

//						Y
		0x7F0C,	// Yfct
		0x7F05,	// Ymax
		0x7F04,	// Ymin
		0x7F06,	// Yscl

//						Z

		0x9C,	// (deg)
		0xAC,	// (rad)
		0xBC,	// (grad)
		0x8C,	// dms
		0x3A,	// :
		0x3F,	// ?
		0x0C,	// dsps
		0x3D,	// =
		0x11,	// !=
		0x3C,	// <
		0x3E,	// >
		0x10,	// <=
		0x12,	// >=
		0x13,	// =>
		0x27,	// '
//		0x22,	// "
		0x7E,	// ~
		0x23,	// #
		0x24,	// $
		0x25,	// %
		0x26,	// &
		0x2A,	// *
		0x2F,	// /
		0x7C,	// |

		0x0B,	// Exa
		0x0A,	// Peta
		0x09,	// Tera
		0x08,	// Giga
		0x07,	// Mega
		0x06,	// Kiro
		0x05,	// milli
		0x04,	// micro
		0x03,	// nano
		0x02,	// pico
		0x01,	// femto

		0x14,	// f1
		0x15,	// f2
		0x16,	// f3
		0x17,	// f4
		0x18,	// f5
		0x19,	// f6

		0x7F6A,	// List1
		0x7F6B,	// List2
		0x7F6C,	// List3
		0x7F6D,	// List4
		0x7F6E,	// List5
		0x7F6F,	// List6

//		0xF7B2,	// VarList1
//		0xF7B3,	// VarList2
//		0xF7B4,	// VarList3
//		0xF7B5,	// VarList4
//		0xF7B6,	// VarList5
//		0xF7B7,	// VarList6

		0xF7B8,	// File1
		0xF7B9,	// File2
		0xF7BA,	// File3
		0xF7BB,	// File4
		0xF7BC,	// File5
		0xF7BD,	// File6

		0xF9D5,	// _Bmp 
		0xF9D6,	// _Bmp8
		0xF9D7,	// _Bmp16
		0xF9DA,	// _BmpRotate
		0xF9D9,	// _BmpZoom
		0xF9DE,	// _BmpZoomRotate
		0xF9CD,	// _Circle
		0xF9C0, // _ClrVRAM
		0xF9C1,	// _ClrScreen
		0xF9C3,	// _Contrast
		0xF9C2,	// _DispVRAM
		0xF9CF,	// _Elips 
		0xF9D1,	// _ElipsInRct 
		0xF9CE,	// _Fcircle 
		0xF9D0,	// _Felips 
		0xF9D2,	// _FelipsInRct .
		0xF9CC, // _Fporgon 
		0xF9C8,	// _Horizontal
		0xF9D3,	// _Hscroll
		0xF9C7,	// _Line 
		0xF9DF,	// _Paint
		0xF9C5,	// _Point 
		0xF9C4,	// _Pixel 
		0xF9C6,	// _PixelTest(
		0xF9CB,	// _Polygon
		0xF9CA,	// _Rect 
		0xF9D8,	// _Test
		0xF9C9,	// _Vertical
		0xF9D4,	// _Vscroll 
		
		0};


const short catalog_opcode_ext[]={
	
//						A
		0xF717,	// ACBreak
		0xF90F,	// AliasVar
		
//						B
		0xF9BE,	// Back-Color
		0xF7FE,	// BackLight
		0xF7DE,	// BatteryStatus
		0xF7DD,	// Beep
		0xF947,	// Bin(
		0xF9DB,	// BmpSave 
		0xF9DC,	// BmpLoad(

//						C
		0xF7F5,	// Call
		0xF7EB,	// Case
		0x7FE9,	// CellSum(
		0x7F5A,	// ColSize(
		0xF90E,	// Const 

//						D
		0xF941,	// DATE
		0xF7EC,	// Default
//		0xF90D,	// Define 
		0xF7DF,	// Delete 
		0xF7E4,	// Disp
		0xF73E,	// DotGet(
		0xF7E0,	// DotLife(
		0xF73B,	// DotPut(
		0xF7F0,	// DotShape(
		0xF73D,	// DotTrim(
		0xF9DD,	// DrawMat

//						E
		0x7F58,	// ElemSize(
		0xF70F,	// ElseIf
		0xF738,	// Except

//						F
		0xF7E2,	// FillRect(
		0xF7FD, // FKeyMenu(

//						G
		0xF960,	// GetFont(
		0xF962,	// GetFOntMini 
		0x7F72,	// GetHSL(
		0x7F70,	// GetHSV(
		0x7F5D,	// GetRGB(
		0xFA,	// Gosub

//						H
		0xF946,	// Hex(
		0xF7A4,	// Horizontal
		0x7F73,	// HSL( 
		0x7F71,	// HSV(

//						I
		0x7FF5,	// IsExist(

//						J

//						K
		0x7F9F,	// KeyRow(

//						L
		0x7F5C,	// ListCmp(
		0xF7EF,	// Load(
		0xF7F1,	// Local
		0xF7E3,	// LocateYX

//						M
		0x7F5B,	// MatBase(
		
//						N

//						O

//						P
		0x7FF6,	// Peek(
		0xF7F6,	// Poke 
		0xF7F2,	// PopUpWin(
		0x7FFA,	// ProgPtr(
		0xF7FC,	// PutDispDD

//						Q

//						R
		0xF7E8,	// ReadGraph(
		0xF7E1,	// Rect(
		0xF7F8,	// RefreshCtrl
		0xF7FA,	// RefreshTime
		0x7F5E,	// RGB(
		0x7F59,	// RowSize(

//						S
		0xF7EE,	// Save
		0xF7FB,	// Screen
		0xF961,	// SetFont
		0xF963,	// SetFontMini
		0xF950,	// StrAsc(
		0xF948,	// StrBase(
		0xF943,	// Sprintf(
		0xF944,	// StrChar(
		0xF945,	// StrCenter(
		0xF949,	// StrRepl(	
		0xF94D,	// StrSplit(	
		0xF7EA,	// Switch
		0xF7ED,	// SwitchEnd
		0xF7F4,	// SysCall
		0x7FCF,	// System(

//						T
		0x7F5F,	// Ticks
		0xF942,	// TIME
		0xF940,	// ToStr(
		0xF9BF,	// Transp-Color
		0xF737, // Try
		0xF739,	// TryEnd

//						U

//						V
		0x7FF8,	// VarPtr(
		0x7FDF,	// Version
		0xF7A3,	// Vertical

//						W
		0xF94F,	// Wait 
		0xF7E9,	// WriteGraph(

		0xF9D5,	// _Bmp 
		0xF9D6,	// _Bmp8
		0xF9D7,	// _Bmp16
		0xF9DA,	// _BmpRotate
		0xF9D9,	// _BmpZoom
		0xF9DE,	// _BmpZoomRotate
		0xF9CD,	// _Circle
		0xF9C0, // _ClrVRAM
		0xF9C1,	// _ClrScreen
		0xF9C3,	// _Contrast
		0xF9C2,	// _DispVRAM
		0xF9CF,	// _Elips 
		0xF9D1,	// _ElipsInRct 
		0xF9CE,	// _Fcircle 
		0xF9D0,	// _Felips 
		0xF9D2,	// _FelipsInRct .
		0xF9CC, // _Fporgon 
		0xF9C8,	// _Horizontal
		0xF9D3,	// _Hscroll
		0xF9C7,	// _Line 
		0xF9DF,	// _Paint
		0xF9C4,	// _Pixel 
		0xF9C6,	// _PixelTest(
		0xF9C5,	// _Point 
		0xF9CB,	// _Polygon
		0xF9CA,	// _Rect 
		0xF9D8,	// _Test
		0xF9C9,	// _Vertical
		0xF9D4,	// _Vscroll 

		0};


int check_ext_opcode(int code) {	// 0:genuine	1:ext
	int i,j,k;	
	int	opNum=0 ;
		while ( catalog_opcode_ext[opNum++] ) ;
	i=0;
	while ( i < opNum ) {
		if ( code == catalog_opcode_ext[i++] ) return 1;
	}
	return 0;
}

void SetAlphalock();

int CB_Catalog(void) {
	short *select=&selectCATALOG;
	short *oplist=(short *)catalog_opcode;
	int opNum;
	char buffer[22];
	char tmpbuf[18];
	int key;
	int	cont,cont2=1;
	int i,j,k,m,y;
	int seltop;
	char search[10]="", *search2;
	int CursorStyle;
	int alphalock ;
	char alphalock_bk ;
	int searchmode=1;
	int csrX=0;

	CB_BackPict=0;				// back image
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;
	
	Cursor_SetFlashOff(); 			// cursor flashing off

 alpha_start:
	SetAlphalock();

	while ( cont2 ) {
		
		opNum=0 ;
		while ( oplist[opNum++] ) ;
		opNum-=2;
		seltop=*select;
		cont=1;

		SaveDisp(SAVEDISP_PAGE1);

		while (cont) {
			loop:
			Bdisp_AllClr_VRAM();
			CB_ColorIndex=0x001F;	// Blue
			CB_Prints( 1,1, (unsigned char*)"Catalog.CB");
			CB_ColorIndex=0x0000;	// Black
			Fkey_dispN( FKeyNo1, "INPUT");
			Fkey_dispR( FKeyNo5, "Hist");
			if (  (*select)<seltop ) seltop=(*select);
			if ( ((*select)-seltop) > 5 ) seltop=(*select)-5;
			if ( (opNum-seltop) < 5 ) seltop = opNum-5; 
			for ( i=0; i<6; i++ ) {
//				CB_Print(1,2+i,(unsigned char *)"                     ");
				j=oplist[seltop+i];
//				k=0;
//				while ( j == 0xFFFFFFFF ) { 
//					k++;
//					j=oplist[seltop+i+k]; 
//				}
				if ( j != 0xFFFFFFFF ) {
					CB_OpcodeToStr( j, tmpbuf ) ; // SYSCALL
					tmpbuf[12]='\0'; 
					DMS_Opcode( tmpbuf, j);
					k=0; if ( tmpbuf[0]==' ' ) k++;
					sprintf(buffer,"%-17s",tmpbuf+k ) ;
					if ( check_ext_opcode( j ) ) {
						CB_ColorIndex=0x8010;	// Magenta + black
						CB_Prints(1,2+i,(unsigned char *)buffer);
						CB_Prints(17,2+i,(unsigned char *)"(ext)");
//						CB_ColorIndex=0xFFE0;	// Yellow
//						ML_rectangle( 0, (y+1)*24, 125*3+2, (y+1)*24+23, 0, 0, ML_XOR );
					} else {
						CB_ColorIndex=0x0000;	// Black
						CB_Prints(1,2+i,(unsigned char *)buffer);
					}
				}
			}
			CB_ColorIndex=0x0000;	// Black
			if ( searchmode ) {
				StatusArea_Run_sub( "== SEARCH MODE ==", CB_INTDefault, CB_G1MorG3MDefault );
				locate(12, 1);Prints((unsigned char*)"[        ]");
				CB_Prints(13, 1, (unsigned char*)search );	// search string
//				if ( lowercase  ) Fkey_dispN_aA( FKeyNo4, "A <> a"); else Fkey_dispN_Aa( FKeyNo4, "A <> a");
//				Fkey_Icon( FKeyNo5, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
//				Fkey_Icon( FKeyNo6, 402 );	//	Fkey_DISPN( FKeyNo6, " / ");
			} else {
				StatusArea_Time();
			}
			
//			if ( seltop <= opNum-6 ) { locate(19,7); Print((unsigned char*)"\xE6\x93"); } // dw
//			if ( seltop >= 1 )       { locate(19,2); Print((unsigned char*)"\xE6\x92"); } // up
			
			y = ((*select)-seltop) + 1 ;
			Bdisp_AreaReverseVRAM( 0, (y+1)*24, 125*3+2, (y+1)*24+23 );	// reverse *select line 
			Bdisp_PutDisp_DD();

			if ( searchmode ) {
				locate(13+csrX,1);
				Cursor_SetFlashMode(1);			// cursor flashing on
			}
			GetKey_DisableMenu( &key );
			switch (key) {
				case KEY_CTRL_SHIFT:
					goto loop;
					
				case KEY_CTRL_MENU:
				case KEY_CTRL_F5:
					key=SelectOpcodeRecent( CMDLIST_RECENT );
					if ( key ) return key;
					goto loop;
					break;
					
				case KEY_CTRL_QUIT:
				case KEY_CTRL_EXIT:
					RestoreDisp(SAVEDISP_PAGE1);
					return 0;
					
				case KEY_CTRL_F1:
				case KEY_CTRL_EXE:
					cont=0;
					cont2=0;
					goto exit;
					break;
			
				case KEY_CTRL_ALPHA:
					alphalock = 0 ;
					key = 0;
					goto loop;
					break;
						
				case KEY_CTRL_AC:
					search[0]='\0';
					csrX=0;
					if  ( alphalock ) goto alpha_start;
					key = 0;
					goto loop;
					break;

				case KEY_CTRL_DEL:
					if (searchmode ) {
						if ( CursorStyle < 0x6 ) {		// insert mode
							PrevOpcodeGB( search, &csrX );
						}
						DeleteOpcode1( search, 8, &csrX );
					}
					goto loop;
					break;
				
				case KEY_CTRL_LEFT:
					if ( searchmode ) { 
						PrevOpcodeGB( search, &csrX ); 
						goto loop;
						break; 
					}
//					for ( i=(*select)-2; i>0; i-- ) {
//						if ( oplist[i] == 0xFFFFFFFF ) break;
//					}
//					if ( i<0 ) i = 0 ;
//					if ( i>0 ) i++ ;
//					*select = i ;
//					seltop = *select;
					searchmode=1;
					goto loop;
					break;
					
				case KEY_CTRL_RIGHT:
					if ( searchmode ) {
						if ( search[csrX] != 0x00 )	NextOpcodeGB( search, &csrX );
						goto loop;
						break;
					}
//					for ( i=(*select)+1; i<(*select)+opNum; i++ ) {
//						if ( oplist[i] == 0xFFFFFFFF ) break;
//					}
//					*select = i+1 ;
//					if ( *select > opNum ) *select = opNum;
//					seltop = *select;
					searchmode=1;
					goto loop;
					break;
					
				case KEY_CTRL_UP:
					(*select)--;
					if ( oplist[(*select)] == 0xFFFFFFFF ) (*select)--;
					if ( *select < 0 ) *select = opNum;
					goto loop;
					break;
					
				case KEY_CTRL_DOWN:
					(*select)++;
					if ( oplist[(*select)] == 0xFFFFFFFF ) (*select)++;
					if ( *select > opNum ) *select =0;
					goto loop;
					break;
					
				default:
					break;
			}
			if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
			if ( key == ' ' ) key=0x9C;
			if ( key == '"' ) key='_';
			if ( ( ('A' <= key) && (key <= 'Z') ) || (key == 0x9C) || (key == '_') ) {
				if ( CursorStyle < 0x6 ) {		// insert mode
					i=InsertOpcode1( search, 8, csrX, key );
				} else {					// overwrite mode
					if ( search[csrX] != 0x00 ) DeleteOpcode1( search, 8, &csrX);
					i=InsertOpcode1( search, 8, csrX, key );
				}
				if ( i==0 ) NextOpcodeGB( search, &csrX );
			} else {
				RestoreDisp(SAVEDISP_PAGE1);
				return key;
			}
			if ( ( ( ('A' <= key) && (key <= 'Z') ) || (key == 0x9C) || (key == '_') ) && ( strlen(search) ) ) {
				searchmode=1;
				i=0;
				j=strlen(search);
				while ( i<opNum ) {
						if ( oplist[i] == 0xFFFFFFFF ) i++;
						else {
							CB_OpcodeToStr( oplist[i] , tmpbuf ) ; // SYSCALL
							tmpbuf[12]='\0'; 
							m=0; if ( tmpbuf[0]==' ' ) m++;
							k=0;
							while ( k<=j ) {
								if ( ToUpperC(tmpbuf[k+m]) != search[k] ) { k=(k>=j); break; }
								k++;
							}
							if ( k>0 ) {
								(*select) = i;
								seltop = (*select);
								break;
							} else i++;
						}
						
				}
			}
		}
		RestoreDisp(SAVEDISP_PAGE1);
	}
	exit:
	Bdisp_PutDisp_DD();
	return oplist[(*select)] & 0xFFFF;
}

//--------------------------------------------------------------------------

const short oplistOPTN[]={
		0x97,	// Abs
		0xA6,	// Int
		0xDE,	// Intg
		0xB6,	// frac
		0xAB,	// !
		0x7F3A,	// MOD(
		0x7F3C,	// GCD(
		0x7F3D,	// LCM(
		0x7F85,	// logab(
		0x7F29,	// Sigma(
		
		0xFFFF,	// 				-
		0x7FB0,	// And
		0x7FB1,	// Or
		0x7FB3,	// Not
		0x7FB4,	// Xor
		0xC1,	// Ran#
		0x7F87,	// RanInt#(	
		0x7F8A,	// RanNorm#(	
		0x7F89,	// RanBin#(	
		0x7F88,	// RanList#(
		
		0xFFFF,	// 				-
		0xBA,	// and
		0xAA,	// or
		0xA7,	// not
		0x9A,	// xor
//		0x26,	// &
//		0x7C,	// |
		0x7FBC,	// Int/
		0x7FBD,	// Rmdr
		
		0xFFFF,	// 				-
		0xD3,	// Rnd
		0x7F86,	// RndFix(
		0xD9,	// Norm
		0xE3,	// Fix
		0xE4,	// Sci
		0xDD,	// Eng
		0xF90B,	// EngOn
		0xF90C,	// EngOff

		0xFFFF,	// 				-
		0x7F46,	// Dim
		0x7F4A,	// List->Mat(
		0x7F47,	// Fill(
		0x7F2C,	// Seq(
		0x7F2D,	// Min(
		0x7F20,	// Max(
		0x7F2E,	// Mean(
		0x7F49,	// Augment(
		0x7F4C,	// Sum
		0x7F4D,	// Prod
		0xF7B0,	// SortA(
		0xF7B1,	// SortB(
		0x7F5C,	// ListCmp(

		0xFFFF,	// 				-
		0x7F46,	// Dim
		0x7F4B,	// Mat->List(
		0x7F21, // Det
		0x7F41,	// Trn 
		0x7F49,	// Augment(
		0x7F48,	// Identity
		0x7F47,	// Fill(
		0x7F55,	// Ref 
		0x7F56,	// Rref 
		0xF94A, // CrossP(
		0xF94B, // DotP("
		0xF96D, // Angle(
		0xF95E, // UnitV(
		0xF95B, // Norm("
		
		0xFFFF,	// 				-
		0x7F45,	// Swap
		0x7F42,	// *Row
		0x7F43,	// *Row+
		0x7F44,	// Row+
		0x7F5B,	// MatBase(
		0x7F58,	// ElemSize(
		0x7F59,	// RowSize(
		0x7F5A,	// ColSize(
		0x7FE9,	// CellSum(

		0xFFFF,	// 				-
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh

		0xFFFF,	// 				-
		0x7F22,	// Arg 
		0x7F23,	// Conjg 
		0x7F24,	// ReP 
		0x7F25,	// ImP 
		0x7F54,	// Angle
//		0x7F55,	// Ref 
//		0x7F56,	// Rref 
//		0x7F57,	// Conv

		0xFFFF,	// 				-
		0x01,	// femto
		0x02,	// pico
		0x03,	// nano
		0x04,	// micro
		0x05,	// milli
		0x06,	// Kiro
		0x07,	// Mega
		0x08,	// Giga
		0x09,	// Tera
		0x0A,	// Peta
		0x0B,	// Exa
		
		0xFFFF,	// 				-
		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0x9C,	// (deg)
		0xAC,	// (rad)
		0xBC,	// (grad)
		
		0xFFFF,	// 				-
		0x8C,	// dms
		0xF905,	// >DMS
		0x80,	// Pol(
		0xA0,	// Rec(
		0xF941,	// DATE
		0xF942,	// TIME
		0x7F5F,	// Ticks
		0xF94F,	// Wait 
		0xF7DE,	// BatteryStatus
		0xF7DD,	// Beep
		0x7FDF,	// Version
		0x7FCF,	// System(
//		0xF95F, // IsError( 

		0xFFFF,	// 				-
		0xF999,	// Plot/Line-Color
		0xF9BE,	// Back-Color
		0xF9BF,	// Transp-Color
		0x7F5E,	// RGB(
		0x7F5D,	// GetRGB(
		0x7F71,	// HSV(
		0x7F70,	// GetHSV(
		0x7F73,	// HSL( 
		0x7F72,	// GetHSL(

		0xFFFF,	// 				-
		0x23,	// #
		0x24,	// $
		0x25,	// %
		0x7C,	// |
		0x26,	// &
		0x2A,	// *
		0};
							
const short oplistPRGM[]={	
		0x3F,	// ?
		0x0C,	// dsps
		0x3A,	// :
		0x27,	// '
		0x7E,	// ~
		
		0xFFFF,	// 				-
		0x3D,	// =
		0x11,	// !=
		0x3E,	// >
		0x3C,	// <
		0x12,	// >=
		0x10,	// <=
		
		0xFFFF,	// 				-
		0xE2,	// Lbl
		0xEC,	// Goto
		0x13,	// =>
		0xE9,	// Isz
		0xE8,	// Dsz
		0xF79E,	// Menu
		
		0xFFFF,	// 				-
		0xF700,	// If
		0xF701,	// Then
		0xF702,	// Else
		0xF70F,	// ElseIf
		0xF703,	// IfEnd

		0xFFFF,	// 				-
		0xF704,	// For
		0xF705,	// To
		0xF706,	// Step
		0XF707,	// Next

		0xFFFF,	// 				-
		0xF708,	// While
		0xF709,	// WhileEnd
		0xF70A,	// Do
		0xF70B,	// LpWhile

		0xFFFF,	// 				-
		0xF7EA,	// Switch
		0xF7EB,	// Case
		0xF7EC,	// Default
		0xF7ED,	// SwitchEnd
		
		0xFFFF,	// 				-
		0xED,	// Prog
		0xF70C,	// Return
		0xF70D,	// Break
		0xF70E,	// Stop
		
		0xF7F1,	// Local
		0xFA,	// Gosub
		0xF717,	// ACBreak
		0xF737, // Try
		0xF738,	// Except
		0xF739,	// TryEnd

		0xFFFF,	// 				-
		0xF718,	// ClrText
		0xF719,	// ClrGraph
		0xF71A,	// ClrList
		0xF91E,	// ClrMat
		0xF93E,	// ClrVct
		0xF710,	// Locate
		0x7F8F,	// Getkey
		0xF7E4,	// Disp
		
		0xFFFF,	// 				-
		0xF711,	// Send(
		0xF712,	// Receive(
		0xF713,	// OpenComport38k
		0xF714,	// CloseComport38k
		0xF715,	// Send38k 
		0xF716,	// Receive38k 
		
		0xFFFF,	// 				-
		0x7F9F,	// KeyRow(
		0xF90F,	// Alias
		0xF90E,	// Const 
		0x7FF5,	// IsExist(
		0xF7DF,	// Delete 
		0xF7EE,	// Save
		0xF7EF,	// Load(
		0xF7F4,	// SysCall
		0xF7F5,	// Call
		0xF7F6,	// Poke 
		0x7FF6,	// Peek(
		0x7FF8,	// VarPtr(
		0x7FFA,	// ProgPtr(
		
		0xFFFF,	// 				-
		0xF93F,	// Str
		0xF930,	// StrJoin(
		0xF931,	// StrLen
		0xF932,	// StrCmp(
		0xF933,	// StrSrc(
		0xF934,	// StrLeft(
		0xF935,	// StrRight(
		0xF936,	// StrMid(
		0xF937,	// Exp>Str(
		0xF938,	// Exp(
		0xF939,	// StrUpr(
		0xF93A,	// StrLwr(
		0xF93B,	// StrInv(
		0xF93C,	// StrShift(
		0xF93D,	// StrRotate(

		0xFFFF,	// 				-
		0xF943,	// Sprintf(
		0xF944,	// StrChar(
		0xF945,	// StrCenter(
		0xF949,	// StrRepl(	
		0xF94D,	// StrSplit(	
		0xF946,	// Hex(
		0xF947,	// Bin(
		0xF948,	// StrBase(
		0xF940,	// ToStr(
		0xF950,	// StrAsc(
		0x5C,	// 
		0x24,	// $
		0};
		
const short oplistVARS[]={
		0xD1,	// Cls
		0xF719,	// ClrGraph
		0xEB,	// ViewWindow
		0xE0,	// Plot
		0xE1,	// Line
		0xF7A7,	// F-Line

		0xFFFF,	// 				-
		0xF7A3,	// Vertical
		0xF7A4,	// Horizontal
		0xF7A6,	// Circle
		0xF7A5,	// Text
		0xF7E3,	// LocateYX

		0xFFFF,	// 				-
		0xF7A8,	// PlotOn
		0xF7A9,	// PlotOff
		0xF7AA,	// PlotChg
		0xF7AB,	// PxlOn
		0xF7AC,	// PxlOff
		0xF7AD,	// PxlChg
		0xF7AF,	// PxlTest(

		0xFFFF,	// 				-
		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0xF770,	// G-Connect
		0xF771,	// G-Plot
		0xF7C3,	// CoordOn
		0xF7D3,	// CoordOff
		0xF77D,	// GridOn
		0xF77A,	// GridOff
		0xF7C2,	// AxesOn
		0xF7D2,	// AxesOff
		0xF7C4,	// LabelOn
		0xF7D4,	// LabelOff
		0xF7C5,	// DerivOn
		0xF7D5,	// DerivOff

		0xFFFF,	// 				-
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		0xF9F4,	// S-L-Thin
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		0xF9F3,	// SketchThin
		
		0xFFFF,	// 				-
		0x7F00,	// Xmin
		0x7F01,	// Xmax
		0x7F02,	// Xscl
		0xF921,	// Xdot
		0x7F04,	// Ymin
		0x7F05,	// Ymax
		0x7F06,	// Yscl
		0x7F0B,	// Xfct
		0x7F0C,	// Yfct
		0x7F08,	// TThetamin
		0x7F09,	// TThetamax
		0x7F0A,	// TThetaptch

		0xFFFF,	// 				-
		0xF797,	// StoV-Win
		0xF798,	// RclV-Win
		0xF793,	// StoPict
		0xF794,	// RclPict
		0xF79D,	// StoCapt
		0xF79F,	// RclCapt
		0xF778,	// BG-None
		0xF779,	// BG-Pict
		0xF91B,	// fn
		0x7FF0,	// GraphY
		0xF720,	// DrawGraph
		0xEE,	// Graph Y=
		
		0xFFFF,	// 				-
		0xF5,	// Graph(X,Y)=(
		0xF723,	// DrawStat
		0xF7CC,	// DrawOn
		0xF7DC,	// DrawOff
		0xF74A,	// S-Gph1
		0xF74B,	// S-Gph2
		0xF74C,	// S-Gph3
		0xF74D,	// Square
		0xF74E,	// Cross
		0xF74F,	// Dot
		0xF750,	// Scatter
		0xF751,	// xyLine
		0xF77C,	// GridLine
		0xF99A,	// AxesScale
		
		0xFFFF,	// 				-
		0xF973,	// ColorAuto 
		0xF97D,	// ColorClr 
		0xF974,	// ColorLighter 
		0xF97A,	// ColorNormal 
		0xF978,	// ColorLinkOn
		0xF979,	// ColorLinkOff
		0xF975,	// ColorLinkX&Y
		0xF976,	// ColorLinkOnlyX
		0xF977,	// ColorLinkOnlyY
		0xF97E,	// ColorLinkX&Freq

		0xFFFF,	// 				-
		0xF7E1,	// Rect(
		0xF7E2,	// FillRect(
		0xF7E8,	// ReadGraph(
		0xF7E9,	// WriteGraph(
		0xF73E,	// DotGet(
		0xF73B,	// DotPut(
		0xF73D,	// DotTrim(
		0xF7E0,	// DotLife(
		0xF7F0,	// DotShape(

		0xFFFF,	// 				-
		0xF7FB,	// Screen
		0xF7FC,	// PutDispDD
		0xF7FD, // FKey(
		0xF7F2,	// PopUpWin(
		0xF7FE,	// BackLight
		0xF7F8,	// RefreshCtrl
		0xF7FA,	// RefreshTime
		
		0xFFFF,	// 				-
		0xF9C0, // _ClrVRAM
		0xF9C1,	// _ClrScreen
		0xF9C2,	// _DispVRAM
		0xF9C3,	// _Contrast
		0xF9C5,	// _Point 
		0xF9C4,	// _Pixel 
		0xF9C6,	// _PixelTest(
		0xF9C7,	// _Line 
		0xF9CA,	// _Rect 
		0xF9C8,	// _Horizontal
		0xF9C9,	// _Vertical
		0xF9CB,	// _Polygon
		0xF9CC, // _Fporgon 
		
		0xF9CD,	// _Circle
		0xF9CE,	// _Fcircle 
		0xF9CF,	// _Elips 
		0xF9D0,	// _Felips 
		0xF9D1,	// _ElipsInRct 
		0xF9D2,	// _FelipsInRct 
		0xF9D3,	// _Hscroll
		0xF9D4,	// _Vscroll 

		0xFFFF,	// 				-
		0xF9D5,	// _Bmp 
		0xF9D6,	// _Bmp8
		0xF9D7,	// _Bmp16
		0xF9D9,	// _BmpZoom
		0xF9DA,	// _BmpRotate
		0xF9DE,	// _BmpZoomRotate
		0xF9DF,	// _Paint
		0xF9DD,	// _DrawMat
		0xF9D8,	// _Test
		0xF9DB,	// BmpSave 
		0xF9DC,	// BmpLoad(
		0xF960,	// GetFont(
		0xF961,	// SetFont
		0xF962,	// GetFOntMini
		0xF963,	// SetFOntMini
		0};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

const short oplistCMD[]={		// 5800P like
//											0	STD
		0x3F,	// ?			1
		0x0E,	// ->			2
		0xF700,	// If			3
		0xF701,	// Then			4
		0xF702,	// Else			5
		0xF703,	// IfEnd		6
		0xE2,	// Lbl			7
		0xEC,	// Goto			8
		0xF70F,	// ElseIf
		0x7E,	// ~
		0x23,	// #
		0x25,	// %

//											1
		0x3D,	// =			1
		0x11,	// !=			2
		0x3E,	// >			3
		0x3C,	// <			4
		0x12,	// >=			5
		0x10,	// <=			6
		0x7FB0,	// And			6
		0x7FB1,	// Or			7
		0x7FB3,	// Not			8
		0x7FB4,	// Xor			9
		0x23,	// #
		0x25,	// %

//		0xBA,	// and
//		0xAA,	// or
//		0xA7,	// not
//		0x9A,	// xor
//		0x26,	// &
//		0x7C,	// |
		
//											2
		0xE8,	// Dsz			1
		0xE9,	// Isz			2
		0x13,	// =>			3
		0xF710,	// Locate		4
		0xD1,	// Cls			5
		0xF7E4,	// Disp
		0xF718,	// ClrText	
		0xF719,	// ClrGraph	
		0xF91E,	// ClrMat	
		0xF71A,	// ClrList	
		0xF93E,	// ClrVct	
		0x25,	// %
		
//											3
		0xF704,	// For			1
		0xF705,	// To			2
		0xF706,	// Step			3
		0XF707,	// Next			4
		0xF708,	// While		5
		0xF709,	// WhileEnd		6
		0xF70A,	// Do			7
		0xF70B,	// LpWhile		8
		0xF79E,	// Menu
		0x7E,	// ~
		0x23,	// #
		0x25,	// %

		
//											4
		0xF70D,	// Break		1
		0xF70C,	// Return		2
		0xF70E,	// Stop			3
		0x7F8F,	// Getkey		4
		0xED,	// Prog			5
		0xFA,	// Gosub
		0xF7F1,	// Local
		0xF717,	// ACBreak
		0xF7EA,	// Switch
		0xF7EB,	// Case
		0xF7EC,	// Default
		0xF7ED,	// SwitchEnd

//											5
		0x7F46,	// Dim	
		0x7F4A,	// List->Mat(
		0x7F47,	// Fill(
		0x7F2C,	// Seq(
		0x7F2D,	// Min(
		0x7F20,	// Max(
		0x7F2E,	// Mean(
		0x7F49,	// Augment(
		0x7F4C,	// Sum
		0x7F4D,	// Prod
		0x7F5C,	// ListCmp(
		0x25,	// %

//											6
		0x7F46,	// Dim	
		0x7F4B,	// Mat->List(
		0x7F21, // Det
		0x7F41,	// Trn 
		0x7F48,	// Identity
		0x7F47,	// Fill(
		0xFFFF,
		0x7F49,	// Augment(
		0x7F55,	// Ref 
		0x7F56,	// Rref 
//		0x5C,	// 
//		0x24,	// $
		0x23,	// #
		0x25,	// %
		
//											7
		0x7F84, // Vct
		0xF94A, // CrossP(
		0xF94B, // DotP("
		0xF96D, // Angle(
		0xF95E, // UnitV(
		0xF95B, // Norm("
		0xF7B0,	// SortA(
		0xF7B1,	// SortB(
		0x7F45,	// Swap
		0x7F42,	// *Row
		0x7F43,	// *Row+
		0x7F44,	// Row+
		
//											8
		0x7F5B,	// MatBase(
		0x7F58,	// ElemSize(
		0x7F59,	// RowSize(
		0x7F5A,	// ColSize(
		0x7FE9,	// CellSum(
		0xFFFF,
		0xFFFF,
		0xFFFF,
		0x5C,	// 
		0x24,	// $
		0x23,	// #
		0x25,	// %

//											9
		0xF711,	// Send(
		0xF712,	// Receive(
		0xF713,	// OpenComport38k
		0xF714,	// CloseComport38k
		0xF715,	// Send38k 
		0xF716,	// Receive38k 
		0xF7DD,	// Beep
		0xFFFF,
		0xFFFF,
		0xFFFF,
		0x23,	// #
		0x25,	// %
		
//											10
		0xF999,	// Plot/Line-Color
		0xF9BE,	// Back-Color
		0x7F5E,	// RGB(
		0x7F5D,	// GetRGB(
		0x7F71,	// HSV(
		0x7F70,	// GetHSV(
		0x7F73,	// HSL( 
		0x7F72,	// GetHSL(
		0xF9BF,	// Transp-Color
		0xFFFF,
		0x23,	// #
		0x25,	// %

//											11	GR
		0xD1,	// Cls		
		0xF719,	// ClrGraph
		0xEB,	// ViewWindow
		0xE0,	// Plot
		0xF7A8,	// PlotOn
		0xF7A9,	// PlotOff
		0xF7AA,	// PlotChg
		0xF7A6,	// Circle
		0xE1,	// Line
		0xF7A7,	// F-Line
		0xF7A3,	// Vertical
		0xF7A4,	// Horizontal
//											12
		0xF7AB,	// PxlOn
		0xF7AC,	// PxlOff
		0xF7AD,	// PxlChg
		0xF7AF,	// PxlTest(
		0xF7A5,	// Text
		0xF7E3,	// LocateYX
		0xF5,	// Graph(X,Y)=(
		0xEE,	// Graph Y=
		0xF720,	// DrawGraph
		0x7FF0,	// GraphY
		0xF79D,	// StoCapt
		0xF79F,	// RclCapt
//		0x23,	// #
//		0x25,	// %

//											13
		0xF5,	// Graph(X,Y)=(
		0xF723,	// DrawStat
		0xF7CC,	// DrawOn
		0xF7DC,	// DrawOff
		0xF74A,	// S-Gph1
		0xF74B,	// S-Gph2
		0xF74C,	// S-Gph3
		0xF74D,	// Square
		0xF74E,	// Cross
		0xF74F,	// Dot
		0xF750,	// Scatter
		0xF751,	// xyLine

//											14
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		0xF778,	// BG-None
		0xF779,	// BG-Pict
		0xF793,	// StoPict
		0xF794,	// RclPict

//											15
		0xF770,	// G-Connect
		0xF771,	// G-Plot
		0xF7C3,	// CoordOn
		0xF7D3,	// CoordOff
		0xF77D,	// GridOn
		0xF77A,	// GridOff
		0xF7C2,	// AxesOn
		0xF7D2,	// AxesOff
		0xF7C4,	// LabelOn
		0xF7D4,	// LabelOff
		0xF797,	// StoV-Win
		0xF798,	// RclV-Win
		
//											16
		0x7F00,	// Xmin
		0x7F04,	// Ymin
		0x7F01,	// Xmax
		0x7F05,	// Ymax
		0x7F02,	// Xscl
		0x7F06,	// Yscl
		0x7F0B,	// Xfct
		0x7F0C,	// Yfct
		0xF921,	// Xdot
		0x7F08,	// TThetamin
		0x7F09,	// TThetamax
		0x7F0A,	// TThetaptch

//											17
		0xF77C,	// GridLine
		0xF99A,	// AxesScale
		0xF973,	// ColorAuto 
		0xF97D,	// ColorClr 
		0xF974,	// ColorLighter 
		0xF97A,	// ColorNormal 
		0xF978,	// ColorLinkOn
		0xF979,	// ColorLinkOff
		0xF975,	// ColorLinkX&Y
		0xF976,	// ColorLinkOnlyX
		0xF977,	// ColorLinkOnlyY
		0xF97E,	// ColorLinkX&Freq
		
//											18	FN
		0x97,	// Abs
		0xA6,	// Int
		0xB6,	// frac
		0xDE,	// Intg
		0x7F3A,	// MOD(
		0xAB,	// !
		0x7FBC,	// Int/
		0x7FBD,	// Rmdr
		0x7F29,	// Sigma( 
		0x7F85,	// logab(
		0x7F3C,	// GCD(
		0x7F3D,	// LCM(

//											19
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh
		0xBA,	// and
		0xAA,	// or
		0xA7,	// not
		0x9A,	// xor
		0x26,	// &
		0x7C,	// |
		
//											20
		0xC1,	// Ran#
		0x7F87,	// RanInt#(
		0x7F8A,	// RanNorm#(
		0x7F89,	// RanBin#(	
		0xD3,	// Rnd
		0x7F86,	// RndFix(
		0xD9,	// Norm
		0xE3,	// Fix
		0xE4,	// Sci
		0xDD,	// Eng
		0xF90B,	// EngOn
		0xF90C,	// EngOff
//		0x23,	// #
//		0x25,	// %

//											21
		0x9C,	// deg
		0xAC,	// rad
		0xBC,	// grad
		0xF905,	// >DMS
		0x8C,	// dms
		0xF91B,	// fn
		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0x7E,	// ~
		0x80,	// Pol(
		0xA0,	// Rec(

//											22
		0x01,	// femto
		0x02,	// pico
		0x03,	// nano
		0x04,	// micro
		0x05,	// milli
		0x06,	// Kiro
		0x07,	// Mega
		0x08,	// Giga
		0x09,	// Tera
		0x0A,	// Peta
		0x0B,	// Exa
		0x25,	// %	

//											23
		0x7F22,	// Arg 
		0x7F23,	// Conjg 
		0x7F24,	// ReP 
		0x7F25,	// ImP 
//		0x7F54,	// Angle
//		0x7F57,	// Conv
		0xF906, // >a+bi
		0xF907, // >re^Theta		
		0xD9,	// Norm
		0xE3,	// Fix
		0xE4,	// Sci
		0xDD,	// Eng
		0xF90B,	// EngOn
		0xF90C,	// EngOff
//		0x23,	// #
//		0x25,	// %	

//											24	STR
		0xF93F,	// Str
		0xF930,	// StrJoin(
		0xF931,	// StrLen
		0xF932,	// StrCmp(
		0xF933,	// StrSrc(
		0xF934,	// StrLeft(
		0xF935,	// StrRight(
		0xF936,	// StrMid(
		0x5C,	// 
		0x24,	// $
		0x23,	// #
		0x25,	// %
		
//											25
		0xF937,	// Exp>Str(
		0xF938,	// Exp(
		0xF939,	// StrUpr(
		0xF93A,	// StrLwr(
		0xF93B,	// StrInv(
		0xF93C,	// StrShift(
		0xF93D,	// StrRotate(
		0xFFFF,
		0x5C,	// 
		0x24,	// $
		0x23,	// #
		0x25,	// %

//											26
		0xF940,	// ToStr(
		0xF943,	// Sprintf(
		0xF946,	// Hex(
		0xF947,	// Bin(
		0xF944,	// StrChar(
		0xF945,	// StrCenter(
		0xF949,	// StrRepl(	
		0xF94D,	// StrSplit(	
		0xF950,	// StrAsc(
		0x24,	// $
		0x23,	// #
		0x25,	// %

//											27	EX
		0xF90F,	// Alias
		0x7F5F,	// Ticks
		0xF941,	// DATE
		0xF942,	// TIME
		0x7FF5,	// IsExist(
		0xF7DF,	// Delete 
		0xF7EE,	// Save
		0xF7EF,	// Load(
		0xF7FC,	// PutDispDD
		0xF94F,	// Wait 
		0x23,	// #
		0xF90E,	// Const 
		
		0xF7FB,	// Screen
		0xF7FC,	// PutDispDD
		0xF7E1,	// Rect(
		0xF7E2,	// FillRect(
		0xF7E8,	// ReadGraph(
		0xF7E9,	// WriteGraph(
		0xF73E,	// DotGet(
		0xF73B,	// DotPut(
		0xF73D,	// DotTrim(
		0xF7E0,	// DotLife(
		0x7FE9,	// CellSum(
		0x25,	// %

		0xF7F4,	// SysCall
		0xF7F5,	// Call
		0xF7F6,	// Poke 
		0x7FF6,	// Peek(
		0x2A,	// *
		0x26,	// &
		0x7FF8,	// VarPtr(
		0x7FFA,	// ProgPtr(
		0x5C,	// 
		0x24,	// $
		0x23,	// #
		0x25,	// %
		
		0xF7FD, // FKeyMenu(
		0xF7F2,	// PopUpWin(
		0xF7F8,	// RefreshCtrl
		0xF7FA,	// RefreshTime
		0x7F9F,	// KeyRow(
		0xF7FE,	// BackLight
		0x7F5B,	// MatBase(
		0x7F58,	// ElemSize(
		0x7F59,	// ColSize(
		0x7F5A,	// RowSize(
		0x23,	// #
		0x25,	// %
		
		0xF9C0, // _ClrVRAM
		0xF9C1,	// _ClrScreen
		0xF9C2,	// _DispVRAM
		0xF9C5,	// _Point 
		0xF9C4,	// _Pixel 
		0xF9C6,	// _PixelTest(
		0xF9C7,	// _Line 
		0xF9CA,	// _Rect 
		0xF9C8,	// _Horizontal
		0xF9C9,	// _Vertical
		0x23,	// #
		0x25,	// %
		
		0xF9CB,	// _Polygon
		0xF9CC, // _Fporgon 
		0xF9CD,	// _Circle
		0xF9CE,	// _Fcircle 
		0xF9CF,	// _Elips 
		0xF9D0,	// _Felips 
		0xF9D1,	// _ElipsInRct 
		0xF9D2,	// _FelipsInRct 
		0xF9D3,	// _Hscroll
		0xF9D4,	// _Vscroll
		0x23,	// #
		0x25,	// %

		0xF9D5,	// _Bmp 
		0xF9D6,	// _Bmp8
		0xF9D7,	// _Bmp16
		0xF9D9,	// _BmpZoom
		0xF9DA,	// _BmpRotate
		0xF9DB,	// BmpSave 
		0xF9DC,	// BmpLoad(
		0xF9DD,	// DrawMat
		0xF9D8,	// _Test
		0xF9DE,	// _BmpZoomRotate
		0xF9DF,	// _Paint
		0x25,	// %
		
		0xF960,	// GetFont(
		0xF961,	// SetFont
		0xF962,	// GetFontMini
		0xF963,	// SetFOntMini
		0xF7DE,	// BatteryStatus
		0xF7DD,	// Beep
		0x7FDF,	// Version
		0x7FCF,	// System(
		0x5C,	// 
		0x24,	// $
		0x23,	// #
		0x25,	// %
		
		
//		0xF797,	// StoV-Win
//		0xF798,	// RclV-Win

		0};

#define CMD_STD  0
#define CMD_GR  11
#define CMD_FN  18
#define CMD_STR 24
#define CMD_EX  27

int SelectOpcode5800P( int flag ) {
	short *select=&selectCMD;
	short *oplist=(short *)oplistCMD;
	int opNum=0 ;
	char buffer[32];
	char tmpbuf[32];
	int key;
	int	cont=1;
	int i,j,y,n,len;
	int seltop;
	int shift=0;

	while ( oplist[opNum++] ) ;
	opNum-=2;
	seltop=*select;

	Cursor_SetFlashOff(); 		// cursor flashing off

	SaveDisp(SAVEDISP_PAGE1);
//	Bdisp_AllClr_DDVRAM();
	
//	PopUpWin(6);

	while (cont) {
		Bdisp_AllClr_VRAM1();
		(*select)=(*select)/12*12;
//		locate(1,1); Prints((unsigned char *)"== Command Select ==");
		locate(1,1); Prints((unsigned char *)"==STD GR FN STR EX==");
		if ( (*select) < CMD_GR*12 )       { locate( 3,1); PrintRevs((unsigned char *)"STD"); }
		else if ( (*select) < CMD_FN *12 ) { locate( 7,1); PrintRevs((unsigned char *)"GR"); }
		else if ( (*select) < CMD_STR*12 ) { locate(10,1); PrintRevs((unsigned char *)"FN"); }
		else if ( (*select) < CMD_EX *12 ) { locate(13,1); PrintRevs((unsigned char *)"STR"); }
		else                               { locate(17,1); PrintRevs((unsigned char *)"EX"); }
		
		for ( i=0; i<12; i++ ) {
			n=oplist[(*select)+i];
			tmpbuf[0]='\0'; 
			if ( n == 0xFFFF ) n=' ';
			else CB_OpcodeToStr( n, tmpbuf ) ; // SYSCALL
//			tmpbuf[8]='\0'; 
			DMS_Opcode( tmpbuf, n);
			len=strlenOp( tmpbuf );
			n=i+1; if (n>9) n=0;
			j=0; if ( tmpbuf[0]==' ' ) j++;
			if ( i< 10 ) sprintf3(buffer,"%d:%-9s",n,tmpbuf+j ) ;
			if ( i==10 ) sprintf3(buffer,".:%-9s",tmpbuf+j ) ;
			if ( i==11 ) sprintf3(buffer,"\x0F:%-9s",tmpbuf+j ) ;
			locate(1+(i%2)*11,2+i/2); Prints((unsigned char *)buffer);
			if ( len>7+j ) { 
				locate(3+(i%2)*11,2+i/2); Prints((unsigned char *)"         ");
				PrintMiniXY((2+(i%2)*11)*18, (1+i/2)*24+4, tmpbuf+j, COLOR_BLACK, COLOR_WHITE, 0, 0);
			} else {
				locate(11+(i%2)*11,2+i/2); Prints((unsigned char *)" ");
			}
			if ( i==11 ) { locate(0+(i%2)*12,2+i/2); Prints((unsigned char *)"\x0F"); }
		}
		if ( shift ) {
			Menu_CMD_PRGM_REL();
		} else {
			Menu_CMD_MENU_EXT();
		}
//		Bdisp_PutDisp_DD();	
		
		y = ((*select)-seltop) + 1 ;
//		Bdisp_AreaReverseVRAMx3(12, y*8, 113, y*8+7);	// reverse *select line 
//		Bdisp_PutDisp_DD();

		GetKey_DisableMenu(&key);
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_QUIT:
				RestoreDisp(SAVEDISP_PAGE1);
				if ( flag ) return 0x10000;
				return 0;
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_F1:	// ?
				RestoreDisp(SAVEDISP_PAGE1);
				if ( shift ) return '='; else return '?';
				break;
			case KEY_CTRL_F2:	// dsps
				RestoreDisp(SAVEDISP_PAGE1);
				if ( shift ) return 0x11; else return 0x0C;
				break;
			case KEY_CTRL_F3:	// :
				RestoreDisp(SAVEDISP_PAGE1);
				if ( shift ) return '>'; else return ':';
				break;
			case KEY_CTRL_F4:	// '
				RestoreDisp(SAVEDISP_PAGE1);
				if ( shift ) return '<'; else return 0x13;
				break;
			case KEY_CTRL_F5:	// =>
				RestoreDisp(SAVEDISP_PAGE1);
				if ( shift ) return 0x12; else return 0x27;
				break;
			case KEY_CTRL_F6:	// !=
				RestoreDisp(SAVEDISP_PAGE1);
				if ( shift ) return 0x10; else return '/';
				break;

			case KEY_CTRL_LEFT:
				if ( (*select) < CMD_GR*12 )       { (*select)=CMD_EX *12; break; }
				else if ( (*select) < CMD_FN *12 ) { (*select)=CMD_STD*12; break; }
				else if ( (*select) < CMD_STR*12 ) { (*select)=CMD_GR *12; break; }
				else if ( (*select) < CMD_EX *12 ) { (*select)=CMD_FN *12; break; }
				else                               { (*select)=CMD_STR*12; break; }
				break;
			case KEY_CTRL_RIGHT:
				if ( (*select) < CMD_GR*12 )       { (*select)=CMD_GR *12; break; }
				else if ( (*select) < CMD_FN*12 )  { (*select)=CMD_FN *12; break; }
				else if ( (*select) < CMD_STR*12 ) { (*select)=CMD_STR*12; break; }
				else if ( (*select) < CMD_EX*12 )  { (*select)=CMD_EX *12; break; }
				else                               { (*select)=CMD_STD*12; break; }
				break;
			case KEY_CTRL_UP:
			case KEY_CTRL_PAGEUP:
				(*select)-=12;
				if ( *select < 0 ) *select = opNum;
				break;
			case KEY_CTRL_DOWN:
			case KEY_CTRL_PAGEDOWN:
				(*select)+=12;
				if ( *select >= opNum ) *select =0;
				break;
			case KEY_CHAR_1:
			case KEY_CHAR_U:
				n=0;
				cont=0;
				break;
			case KEY_CHAR_2:
			case KEY_CHAR_V:
				n=1;
				cont=0;
				break;
			case KEY_CHAR_3:
			case KEY_CHAR_W:
				n=2;
				cont=0;
				break;
			case KEY_CHAR_4:
			case KEY_CHAR_P:
				n=3;
				cont=0;
				break;
			case KEY_CHAR_5:
			case KEY_CHAR_Q:
				n=4;
				cont=0;
				break;
			case KEY_CHAR_6:
			case KEY_CHAR_R:
				n=5;
				cont=0;
				break;
			case KEY_CHAR_7:
			case KEY_CHAR_M:
				n=6;
				cont=0;
				break;
			case KEY_CHAR_8:
			case KEY_CHAR_N:
				n=7;
				cont=0;
				break;
			case KEY_CHAR_9:
			case KEY_CHAR_O:
				n=8;
				cont=0;
				break;
			case KEY_CHAR_0:
			case KEY_CHAR_Z:
				n=9;
				cont=0;
				break;
			case KEY_CHAR_DP:
			case KEY_CHAR_SPACE:
				n=10;
				cont=0;
				break;
			case KEY_CHAR_EXP:
			case KEY_CHAR_DQUATE:
				n=11;
				cont=0;
				break;
			case KEY_CTRL_OPTN:
				RestoreDisp(SAVEDISP_PAGE1);
				if ( flag ) return 0x10000+CMDLIST_OPTN;
				key=SelectOpcode( CMDLIST_OPTN, 1);
				if ( key ) return key & 0xFFFF;
				break;
			case KEY_CTRL_VARS:
			case KEY_CTRL_PRGM:
				RestoreDisp(SAVEDISP_PAGE1);
				if ( flag ) {
					if ( shift )	return 0x10000+CMDLIST_PRGM;
					else			return 0x10000+CMDLIST_VARS;
				}
				if ( shift )	key=SelectOpcode( CMDLIST_PRGM, 1 );
				else			key=SelectOpcode( CMDLIST_VARS, 1 );
				if ( key ) return key & 0xFFFF;
				break;
			case KEY_CTRL_SHIFT:
				shift=1-shift;
				break;
			default:
				break;
		}
	}

	RestoreDisp(SAVEDISP_PAGE1);
//	Bdisp_PutDisp_DD();

	i=oplist[(*select)+n]; if (i==0xFFFFFFFF ) i=0;
	return i & 0xFFFF;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int IsGBCode1( int Hi, int Lw ){
	Hi&=0xFF; Lw&=0xFF;
	if ( (Hi==0x5C)&&(((0xE5<=Lw)&&(Lw<=0xE7))||(Lw==0x7F)) ) return 0;		// escape GB code
	if ( (0xA0<=Hi)&&(Hi<=0xFE)&&(0xA0<=Lw)&&(Lw<=0xFE) ) return 1;	// GB code
	return 0;		// not GB code
}
int IsGBCode2( int Hi, int Lw ){
	Hi&=0xFF; Lw&=0xFF;
	if ( (Hi==0x5C)&&(((0xE5<=Lw)&&(Lw<=0xE7))||(Lw==0x7F)) ) return 0;		// escape GB code 4byte
	if ( ( GBcode==0 ) ) return 0;		// not GB code
	if ( (Hi==0x03)&&(Lw==0xA8) ) { GBcode=2; return 1; }	// GB code pre byte
	if ( (0xA0<=Hi)&&(Hi<=0xFE)&&(0xA0<=Lw)&&(Lw<=0xFE) ) {
		if ( GBcode==2 ) return 1;	// GB code
		switch ( Hi ) {
			case 0xF7:		// 
			case 0xF9:		// 
			case 0xE5:		// 
			case 0xE6:		// 
			case 0xE7:		// 
				return 0 ;
		}
		return 1;	// GB code
	}
	return 0;		// not GB code
}
int IsGBCode( int opcode ){
	return IsGBCode2( (opcode&0xFF00)>>8, (opcode&0x00FF) );
}
int CheckOpcodeGB( int opcode ){
	if ( GBcode==0 ) return 0;
	return IsGBCode(opcode) ;
}

int OpcodeLenGB( int opcode ){
	if ( opcode >> 24 ) return 4;
	if ( opcode >> 16 ) return 3;
	if ( opcode >>  8 ) return 2;
	return 1;
}

int OpCodeLenGB2( char *SRC ) {
	return OpcodeLenGB( GetOpcodeGB( SRC, 0 ) ) ;
}

int CB_MB_ElementCount( char *str ) {
	int ptr=0,len=0;
	int c,d,opcode;
	while ( 1 ) {
		opcode = GetOpcodeGB( str, ptr );
		if ( opcode ==0x00) break;
		ptr += OpcodeLenGB( opcode );
		len++;
	}
	return len;
}
int CB_MB_strlen( char *str ) {
	int ptr=0;
	int c,d,opcode;
	while ( 1 ) {
		opcode = GetOpcodeGB( str, ptr );
		if ( opcode ==0x00) break;
		ptr += OpcodeLenGB( opcode );
	}
	return ptr;
}
int CheckEscape1( int a ){
	a &= 0xFF;
	return ( ((0x01<=a)&&(a<=0x0D))||(a==0x0F)||((0x10<=a)&&(a<=0x13))||(0x86==a)||(a==0x87)||(a==0x8C)||(a==0x90)||(a==0x9C)||(a==0xA9)||(a==0xAC)||(a==0xB5)||(a==0xB9)||(a==0xBB)||(a==0xBC)||(a==0xC2)||(a==0xC3)||(a==0xCB)||(a==0xCC)||(a==0xCD)||(a==0xCE)||(a==0xD8) );
}
int GetOpcodeGB( char *SRC, int ptr ){
	int c,d,e,f;
	c=SRC[ptr]  ;
	d=SRC[ptr+1];
	switch ( c ) {
		case 0x00:		// <EOF>
			return 0 ;
		case 0x5C:		// backslash
			if ( (((0xFFFFFFE5<=d)&&(d<=0xFFFFFFE7))||(d==0x7F)) && ( SRC[ptr+2]==0x5C ) ) {	// backslash
					return ( 0x5C005C00 + ((unsigned char)d<<16) + (unsigned char)SRC[ptr+3] );	// escape GB code 4byte
			} else
			if ( (GBcode!=0) && CheckEscape1(d) ) {
				return ( 0x5C00 + ((unsigned char)d) );	// escape GB code 2byte
			}
			break;
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			return ((unsigned char)c<<8)+(unsigned char)d;
		default:
			if ( GBcode ) {
				if ( (c==0x03)&&(d==0xFFFFFFA8) ) return 0x03A8;	// GB code pre byte
				if ( IsGBCode2(c,d) ) { return ((unsigned char)c<<8)+(unsigned char)d; }	// GB code
			}
	}
	return (unsigned char)c ;
}

int PrevOpcode7F( char *SRC, int offset ){
	int c,prev;
	int ofst=offset-2;
	if ( ofst <= 0 ) {ofst=0; goto next; }
	while ( ofst > 0 ) {
		c=SRC[--ofst];
		if ( (0x03<c)&&(c<0x5C) ) { ofst++; break; }
	}
 next:
	do {
		prev = ofst;
		NextOpcodeGB( SRC, &ofst );
	} while ( ofst < offset );
	return prev;
}
int PrevOpcodeGB( char *SRC, int *offset ){
	int c,d,e,f;
	(*offset)-=2; 
	if ( *offset < 0 ) { (*offset)=0; return 0;}
		c=SRC[*offset];
		switch ( c ) {
			case 0x5C:		// backslash
				if ( (*offset)-2 >= 0 ) {
					(*offset)-=2;
					e=SRC[*offset];
					f=SRC[*offset+1];
					if ( (e==0x5C)&&(((0xFFFFFFE5<=f)&&(f<=0xFFFFFFE7))||(f==0x7F)) ) return 4;	// escape GB code 4byte
					(*offset)+=2;
				}
				if ( GBcode ) goto prevj;
//				// escape + 1byte			... c:5C(-2) d:A9(-1) ...
//				if ( (GBcode!=0) && ( CheckEscape1( SRC[*offset+1] ) ) ) return 2;	// escape GB code 2byte
				break;
			case 0x7F:		// 
			case 0xFFFFFFFF:	// 
				return 2 ;
			case 0xFFFFFFF7:		// 
			case 0xFFFFFFF9:		// 
			case 0xFFFFFFE5:		// 
			case 0xFFFFFFE6:		// 
			case 0xFFFFFFE7:		// 
				if ( GBcode==0 ) return 2;
				if ( (*offset)-1 < 0 ) { return 2;}
			 prevj:
				d = (*offset)+2;
				(*offset) = PrevOpcode7F( SRC, d );
				return d-(*offset);
			default:
				if ( GBcode ) if ( IsGBCode2(c,SRC[*offset+1]) ) goto prevj;
		}
	(*offset)++;
	return 1 ;
}
int NextOpcodeGB( char *SRC, int *offset ){
	int c,d,e,f;
	c = SRC[(*offset)++];
	switch ( c ) {
		case 0x00:		// <EOF>
			(*offset)--;
			return 0 ;
		case 0x5C:		// backslash
			d = SRC[(*offset)];
			if ( SRC[(*offset)+1]==0x5C ) if ( ((0xFFFFFFE5<=d)&&(d<=0xFFFFFFE7))||(d==0x7F) ) { (*offset)+=3; return 4; }	// escape GB code 
			if ( (GBcode!=0) && CheckEscape1(d) ) { (*offset)++; return 2; }	// escape GB code 2byte
			break;
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			(*offset)++;
			return 2 ;
		default:
			if ( GBcode ) {
				d = SRC[(*offset)];
				if ( (c==0x03)&&(d==0xFFFFFFA8) )  { (*offset)++; return 2; }	// GB code pre byte
				if ( IsGBCode2(c,d) ) 			   { (*offset)++; return 2; }	// GB code
			}
	}
	return 1 ;
}

int NextLineGB( char *SRC, int *offset ){
	int c;
	int ofst=*offset,ofst2;
	int n;
	while (1) {
		 if ( NextOpcodeGB( SRC, &(*offset) ) ==0 ) return (*offset)-ofst ;
		ofst2=*offset;
		PrevOpcodeGB( SRC, &ofst2 );
		c=SRC[ofst2];
		switch ( c ) {
			case 0x0C:		// dsps
			case 0x0D:		// <CR>
				return (*offset)-ofst ;
				break;
		}
	}
	return (*offset)-ofst ;
}


int PrevLineGB( char *SRC, int *offset ){
	int c,d;
	int ofst=*offset,ofst2;
	int IsGB;
	while (1) {
		PrevOpcodeGB( SRC, &(*offset) );
		if ( *offset <= 0 ) { *offset=0; return ofst-(*offset) ; }
		c=SRC[*offset];
		ofst2=*offset;
		PrevOpcodeGB( SRC, &ofst2 );
		d=SRC[ofst2];
		switch ( c ) {
			case 0x0C:		// dsps
			case 0x0D:		// <CR>
				switch ( d ) {
					case 0x0C:		// dsps
					case 0x0D:		// <CR>
						return ofst-(*offset) ;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		switch ( d ) {
			case 0x0C:		// dsps
			case 0x0D:		// <CR>
				return ofst-(*offset) ;
				break;
			default:
				break;
		}
	}
	return ofst-(*offset) ;
}

int strlenOpGB( char *buffer ) {
	int	i=0;
	while ( 1 ) {
		if ( NextOpcodeGB( buffer, &i ) == 0x00 ) break;
	}
	return i;
}


int GetOpcodeLen( char *SRC, int ptr, int *opcode ){
	*opcode = GetOpcodeGB( SRC, ptr ) ; 
	return OpcodeLenGB( *opcode ) ;
}

int OpcodeStrlen(int c) {
	int opcode=c & 0xFFFF;
	char tmpbuf[18];
	CB_OpcodeToStr( opcode, tmpbuf ) ;		// SYSCALL+
	return CB_MB_ElementCount( tmpbuf ) ;		// SYSCALL
}

//---------------------------------------------------------------------------------------------

typedef struct {
	short code;
	char str[14];
} topcodes;

const topcodes OpCodeStrList[] = {
	{ 0x7F3A, "MOD(" }, 		// SDK emu not support
	{ 0x7F3C, "GCD(" }, 		// SDK emu not support
	{ 0x7F3D, "LCM(" }, 		// SDK emu not support
	{ 0x7F87, "RanInt#(" }, 	// SDK emu not support
	{ 0x7F88, "RanList#(" }, 	// SDK emu not support
	{ 0x7F89, "RanBin#(" }, 	// SDK emu not support
	{ 0x7F8A, "RanNorm#(" }, 	// SDK emu not support
	{ 0x7F58, "ElemSize(" }, 
	{ 0x7F59, "RowSize(" }, 
	{ 0x7F5A, "ColSize(" }, 
	{ 0x7F5B, "MatBase(" }, 
	{ 0x7F5C, "ListCmp(" }, 
	{ 0x7F5D, "GetRGB(" }, 
	{ 0x7F5E, "RGB(" }, 
	{ 0x7F5F, "Ticks" }, 
	{ 0x7F9F, "KeyRow(" }, 
	{ 0x7FCF, "System(" }, 
	{ 0x7FDF, "Version" }, 
	{ 0x7FB4, " Xor " }, 		// SDK emu not support
	{ 0x7FBC, " Int\xB9 " }, 	// SDK emu not support
	{ 0x7FBD, " Rmdr " }, 		// SDK emu not support
	{ 0x7FF5, "IsExist(" }, 
	{ 0x7FF6, "Peek(" }, 
	{ 0x7FF8, "VarPtr(" }, 
	{ 0x7FFA, "ProgPtr(" }, 
	{ 0xF70C, "Return " }, 		// add space
	{ 0xF70F, "ElseIf " }, 
	{ 0xF717, "ACBreak" }, 
	{ 0xF737, "Try " }, 
	{ 0xF738, "Except " }, 
	{ 0xF739, "TryEnd" }, 
	{ 0xF73B, "DotPut(" }, 
	{ 0xF73D, "DotTrim(" }, 
	{ 0xF73E, "DotGet(" }, 
	{ 0x7F70, "GetHSV(" }, 
	{ 0x7F71, "HSV(" }, 
	{ 0x7F72, "GetHSL(" }, 
	{ 0x7F73, "HSL(" }, 
	{ 0xF79D, "StoCapt " },
	{ 0xF79E, "Menu " },		// SDK emu not support
	{ 0xF7CF, "Rename " }, 
	{ 0xF7DD, "Beep " }, 
	{ 0xF7DE, "BatteryStatus" }, 
	{ 0xF7DF, "Delete " }, 
	{ 0xF7E0, "DotLife(" }, 
	{ 0xF7E1, "Rect " }, 
	{ 0xF7E2, "FillRect " }, 
	{ 0xF7E3, "LocateYX " }, 
	{ 0xF7E4, "Disp " }, 
	{ 0xF7E8, "ReadGraph(" }, 
	{ 0xF7E9, "WriteGraph " }, 
	{ 0xF7EA, "Switch " }, 
	{ 0xF7EB, "Case " }, 
	{ 0xF7EC, "Default " }, 
	{ 0xF7ED, "SwitchEnd" }, 
	{ 0xF7EE, "Save " }, 
	{ 0xF7EF, "Load(" }, 
	{ 0xF7F0, "DotShape(" }, 
	{ 0xF7F1, "Local " }, 
	{ 0xF7F2, "PopUpWin(" }, 
	{ 0xF7F4, "SysCall(" }, 
	{ 0xF7F5, "Call(" }, 
	{ 0xF7F6, "Poke(" }, 
	{ 0xF7F8, "RefrshCtrl " }, 
	{ 0xF7FA, "RefrshTime " }, 
	{ 0xF7FB, "Screen" }, 
	{ 0xF7FC, "PutDispDD" }, 
	{ 0xF7FD, "FKeyMenu(" }, 
	{ 0xF7FE, "BackLight " }, 
	{ 0xF90E, "Const ",}, 
	{ 0xF90F, "Alias ",}, 
	{ 0xF930, "StrJoin(" }, 		// SDK emu not support
	{ 0xF931, "StrLen(" }, 			// SDK emu not support
	{ 0xF932, "StrCmp(" }, 			// SDK emu not support
	{ 0xF933, "StrSrc(" }, 			// SDK emu not support
	{ 0xF934, "StrLeft(" }, 		// SDK emu not support
	{ 0xF935, "StrRight(" }, 		// SDK emu not support
	{ 0xF936, "StrMid(" }, 			// SDK emu not support
	{ 0xF937, "Exp\xE6\x9EStr(" }, 	// SDK emu not support
	{ 0xF938, "Exp(" }, 			// SDK emu not support
	{ 0xF939, "StrUpr(" }, 			// SDK emu not support
	{ 0xF93A, "StrLwr(" }, 			// SDK emu not support
	{ 0xF93B, "StrInv(" }, 			// SDK emu not support
	{ 0xF93C, "StrShift(" }, 		// SDK emu not support
	{ 0xF93D, "StrRotate(" }, 		// SDK emu not support
	{ 0xF93F, "Str " }, 			// SDK emu not support
	{ 0xF940, "ToStr(" }, 
	{ 0xF941, "DATE" }, 
	{ 0xF942, "TIME" }, 
	{ 0xF943, "Sprintf(" }, 
	{ 0xF944, "StrChar(" }, 
	{ 0xF945, "StrCenter(" }, 
	{ 0xF946, "Hex(" }, 
	{ 0xF947, "Bin(" }, 
	{ 0xF948, "StrBase(" }, 
	{ 0xF949, "StrRepl(" }, 
	{ 0xF94D, "StrSplit(" }, 
	{ 0xF94F, "Wait " }, 
	{ 0xF950, "StrAsc(" }, 
//	{ 0xF95F, "IsError(" }, 
	{ 0xF960, "GetFont(" }, 
	{ 0xF961, "SetFont " }, 
	{ 0xF962, "GetFontMini(" }, 
	{ 0xF963, "SetFontMini " }, 
	{ 0xF99C, "White " },
	{ 0xF9BE, "Back-Color " },
	{ 0xF9BF, "Transp-Color " },
	{ 0xF9C0, "_ClrVram" },
	{ 0xF9C1, "_ClrScreen" },
	{ 0xF9C2, "_DispVram" },
	{ 0xF9C3, "_Contrast " },
	{ 0xF9C4, "_Pixel " },
	{ 0xF9C5, "_Point " },
	{ 0xF9C6, "_PixelTest(" },
	{ 0xF9C7, "_Line " },
	{ 0xF9C8, "_Horizontal " },
	{ 0xF9C9, "_Vertical " },
	{ 0xF9CA, "_Rectangle " },
	{ 0xF9CB, "_Polygon " },
	{ 0xF9CC, "_FillPolygon " },
	{ 0xF9CD, "_Circle " },
	{ 0xF9CE, "_FillCircle " },
	{ 0xF9CF, "_Elips " },
	{ 0xF9D0, "_FillElips " },
	{ 0xF9D1, "_ElipsInRct " },
	{ 0xF9D2, "_FElipsInRct " },
	{ 0xF9D3, "_Hscroll " },
	{ 0xF9D4, "_Vscroll " },
	{ 0xF9D5, "_Bmp " },
	{ 0xF9D6, "_Bmp8 " },
	{ 0xF9D7, "_Bmp16 " },
	{ 0xF9D8, "_Test" },
	{ 0xF9D9, "_BmpZoom " },
	{ 0xF9DA, "_BmpRotate " },
	{ 0xF9DB, "BmpSave " },
	{ 0xF9DC, "BmpLoad(" },
	{ 0xF9DD, "DrawMat " },
	{ 0xF9DE, "_BmpZmRotate " },
	{ 0xF9DF, "_Paint " },
	{ 0x00FA, "Gosub "},
	{ 0x00A7, "not "}, 			// small
	{ 0x009A, " xor "}, 		// add space
	{ 0x00AA, " or "}, 			// add space
	{ 0x00BA, " and "}, 		// add space
	{ 0, "" }
};

int CB_OpcodeToStr( int opcode, char *string ) {
	int i;
	int code,a,b;
	if ( ( ( opcode>>24)&0xFF) == 0x5C ) {
		a =((opcode>>16)&0xFF);
		b =((opcode>> 8)&0xFF);
		if ( (((0xE5<=a)&&(a<=0xE7))||(a==0x7F))&&(b==0x5C) ) {	// escape GB code
			string[0]=b ;
			string[1]=a;
			string[2]=b ;
			string[3]=opcode&0xFF;
			string[4]='\0';
			return 0;
		}
	}
	opcode &= 0xFFFF;
	a =((opcode>> 8)&0xFF);
	b =((opcode    )&0xFF);
	if ( ( GBcode )&&( opcode == 0x03A8 ) ) { goto code; }	// GBcode in
	if ( ( GBcode )&&( IsGBCode(opcode) ) ) { goto code; }	// GBcode
	if ( ( GBcode )&&( (opcode>>8)==0x5C ) ) { goto code; }	// escape GB code 1byte
	if ( ( opcode==0x00CD )||( opcode==0x00CE ) ) {	// 	<r>  <theta>
		string[0]=opcode &0xFF;	// Reverse L
		string[1]='\0';
		return 0;
	}
	if ( EditListChar ) {
		if ( opcode==0x7F51 ) {	// List
			if ( EditListChar==1 ) {
				string[0]=0xE5;	// Reverse L
				string[1]=0xB7;	// 
			} else {
				string[0]=0xFF;	// Bold L
				string[1]=0xE0;	// 
			}
			string[2]='\0';
			return 0;
		} else
		if ( ( 0x7F6A<=opcode )&&( opcode<=0x7F6F ) ){	// List1-6
			code = opcode -0x7F6A +'1' ;
			if ( EditListChar==1 ) {
				string[0]=0xE5;	// Reverse L
				string[1]=0xB7;	// 
			} else {
				string[0]=0xFF;	// Bold L
				string[1]=0xE0;	// 
			}
			string[2]=code;
			string[3]='\0';
			return 0;
		} else
		if ( opcode==0x7F40 ) {	// Mat
			if ( EditListChar==1 ) {
				string[0]=0xFF;	// Reverse M
				string[1]=0xE2;	// 
			} else {
				string[0]=0xFF;	// Bold M
				string[1]=0xE1;	// 
			}
			string[2]='\0';
			return 0;
		}
	}
	i=0;
	do {
		code = OpCodeStrList[i].code & 0xFFFF ;
		if ( code == opcode ) { strcpy( string, OpCodeStrList[i].str ); return 0; }
		i++;
	} while ( code ) ;
	
	if ( opcode == 0x0F ) {	// e or x10
		switch ( CB_Round.ExpTYPE ) {
			case 2:		// x10
			case 1:		// x10
				string[0]=0xE6;
				string[1]=0xFC;
				string[2]=0xE6;
				string[3]=0xFD;
				string[4]='\0';
				break;
			default:	// e
				string[0]=0x0F;
				string[1]='\0';
				break;
		}
	} else
	if ( ( opcode >= 0xFF00 ) || ( ( 0xE500 <= opcode ) && ( opcode <= 0xE7FF ) ) ) {	// kana or E5xx,E6xx,E7xx
	  code:
		string[0]=opcode >> 8 ;
		string[1]=opcode&0xFF;
		string[2]='\0';
	} else
		return OpCodeToStr( opcode, (const char*)string ) ; // SYSCALL
	return 0;
}

//----------------------------------------------------------------------------------------------

int strlenOp( char *buffer ) {
	int	i=0;
	while ( 1 ) {
		if ( NextOpcodeGB( buffer, &i ) == 0x00 ) break;
	}
	return i;
}

int InsertOpcode1( char *buffer, int Maxstrlen, int ptr, int opcode ){
	int len,i,j;
	len=OpcodeLenGB( opcode );
	if ( ( len + strlenOpGB( buffer ) ) > Maxstrlen ) return -1 ;		// buffer over
	j=ptr+len;
	for ( i=Maxstrlen; i>=j; i-- )
		 buffer[i]=buffer[i-len];
		 
	switch (len) {
		case 1:
			buffer[ptr  ]= (opcode    )&0xFF ;
			break;
		case 2:
			buffer[ptr ] = (opcode>> 8)&0xFF ;
			buffer[ptr+1]= (opcode    )&0xFF ;
			break;
		case 4:
			buffer[ptr ] = (opcode>>24)&0xFF ;
			buffer[ptr+1]= (opcode>>16)&0xFF ;
			buffer[ptr+2]= (opcode>> 8)&0xFF ;
			buffer[ptr+3]= (opcode    )&0xFF ;
			break;
	}
	AddOpcodeRecent( opcode ) ;
	return 0;
}
void DeleteOpcode1( char *buffer, int Maxstrlen, int *ptr){
	int len,i;
	int opcode;
	opcode=GetOpcodeGB( buffer, *ptr );
	if ( opcode == 0 ) {
		if ( *ptr == 0 ) return ;
		PrevOpcodeGB( buffer, &(*ptr) );
		opcode=GetOpcodeGB( buffer, *ptr );
	}
	len=OpcodeLenGB( opcode );
	if ( len == 0 ) return ;
	for ( i=(*ptr); i<=Maxstrlen; i++ ) buffer[i]=buffer[i+len];
}

//---------------------------------------------------------------------------------------------
void EditPaste1( char *srcbase, char *Buffer, int *ptr, int maxlength ){
	int len,i,j,clipPtr;
	int opcode;

	if ( Buffer[0]=='\0' ) return ;	// no clip data
	len=strlenOpGB(Buffer);
	if ( (*ptr)+len > maxlength ) {
		i= (*ptr)+len - maxlength ;
		len -= i; if ( i<0 ) return ;	// no area 
	}
	j=(*ptr)+len;
	for ( i=maxlength; i>=j; i-- ) srcbase[i]=srcbase[i-len];
		 
	for ( i=0; i<len; i++ ) srcbase[i+(*ptr)]=Buffer[i];	// copy from Buffer

	(*ptr)=(*ptr)+len;
}

void EditCopy1( char *srcbase, char *Buffer, int ptr, int startp, int endp ){
	int len,i,j;
	
	PrevOpcodeGB( srcbase, &endp ); if ( startp>endp ) return;
	i=OpcodeLenGB( GetOpcodeGB(srcbase, endp ) );
	len=(endp)-(startp)+i;
	if ( len <=0 ) return;
//	if ( len > ClipMax ) {
//		ErrorPtr=ptr; ErrorNo=NotEnoughMemoryERR;		// Memory error
//		CB_ErrMsg(ErrorNo);
//		return ;
//	}
	for ( i=0; i<len; i++ ) Buffer[i]=srcbase[i+startp];	// copy to Buffer
	Buffer[i]='\0';
}

void EditCutDel1( char *srcbase, char *Buffer, int *ptr, int startp, int endp, int del, int maxlength ){	// del:1 delete
	int len,i,flag=0;

	if ( startp>endp ) { i=startp; startp=endp; endp=i; flag=1; }
	PrevOpcodeGB( srcbase, &endp ); if ( startp>endp ) return;
	i=OpcodeLenGB( GetOpcodeGB(srcbase, endp )  );
	len=(endp)-(startp)+i;
	if ( len <=0 ) return;
	if ( del == 0 ) {
//		if ( len > ClipMax ) {
//			ErrorPtr=(*ptr); ErrorNo=NotEnoughMemoryERR;		// Memory error
//			CB_ErrMsg(ErrorNo);
//			return ;
//		}
		for ( i=0; i<len; i++ ) Buffer[i]=srcbase[i+startp];	// copy to Buffer
		Buffer[i]='\0';
	}
	for ( i=(startp); i<=maxlength; i++ ) srcbase[i]=srcbase[i+len];

	(*ptr)=(startp);
}

//----------------------------------------------------------------------------------------------
int PrintOpcode(int px, int py, char *buffer, int width, int ofst, int ptrX, int *csrX, int rev_mode, char *SPC, int miniflag, int ClipStartPtr, int ClipEndPtr ) {
	char tmpbuf[18],*tmpb;
	int i,len,pxmax,rev;
	int opcode=1;
	int  c=1;
	int wk=18,hk=24;
	int colortmp=CB_ColorIndex;
	int backcolortmp=CB_BackColorIndex;
	if ( miniflag ) { wk=12; hk=18; }
	pxmax=px+width*wk;
	if ( pxmax>383 ) pxmax=383;
	if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
	while ( 1 ) {
		rev=0; if ( ( ClipStartPtr >= 0 ) && ( ClipStartPtr <= ofst ) && ( ofst < ClipEndPtr ) ) rev=1;
		if (ofst==ptrX) *csrX=px;
		opcode = GetOpcodeGB( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLenGB( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		if ( px+len*wk > pxmax ) break;
		tmpb=tmpbuf;
		CB_ColorIndex=colortmp;
		CB_BackColorIndex=backcolortmp;
		if ( rev ) { CB_ColorIndex=backcolortmp; CB_BackColorIndex=colortmp; }	// reverse set
		while ( i < len ) {
			if ( px <= pxmax-wk ) {
				if ( miniflag ) {
					CB_PrintMiniC_Fix( px, py, (unsigned char*)(tmpb+i), MINI_OVER | 0x20000000 ) ;	// fixed 12 dot
					px+=12;
				} else {
					CB_PrintModeC( px, py, (unsigned char*)(tmpb+i), MINI_OVER | 0x30000000 ) ;	// fixed 18 dot
					px+=18;
				}
			}
			c=tmpb[i]&0xFF;
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) tmpb++;
			i++ ;
		}
	}
	CB_ColorIndex=colortmp;
	CB_BackColorIndex=backcolortmp;
	c = 0;
	while ( px <= pxmax-wk ) {
				if ( miniflag ) {
					CB_PrintMiniC_Fix( px, py, (unsigned char*)SPC, MINI_OVER ) ;
					px+=12;
				} else {
					CB_PrintModeC( px, py, (unsigned char*)SPC, MINI_OVER );
					px+=18;
				}
				c++;	// SPC count
	}
	return c;
}

//----------------------------------------------------------------------------------------------

void SetAlphaStatus( int alphalock, int lowercase ){
	int key;
//	int	alphastatus = Setup_GetEntry(0x14);	// 00:SHIFT and ALPHA off, 01:SHIFT on, 02:Clip  04:ALPHA on, 0x84:SHIFT and ALPHA on  // 0x08 0x88 lowercase
	if ( ( alphalock != 0 ) && ( lowercase != 0 ) ) Setup_SetEntry(0x14, 0x88);		// lowercase  alpha lock
	else
	if ( ( alphalock != 0 ) && ( lowercase == 0 ) ) Setup_SetEntry(0x14, 0x84);		// upperrcase alpha lock
	else
	if ( ( alphalock == 0 ) && ( lowercase != 0 ) ) Setup_SetEntry(0x14, 0x08);		// lowercase 
	else
	if ( ( alphalock == 0 ) && ( lowercase == 0 ) ) Setup_SetEntry(0x14, 0x04);		// upperrcase
//	PutKey( KEY_CTRL_NOP, 1 );
}
void PutAlphamode1( int lowercase ){
	int key;
//	PutKey( KEY_CTRL_ALPHA, 1 );
	SetAlphaStatus( 0, lowercase );
}

short selectCMD=0;
short selectOPTN=0;
short selectVARS=0;
short selectPRGM=0;
short selectCATALOG=0;
int  lowercase = 0;
int  alphalock = 0;
int  alphastatus = 0;

int InputStrSubC(int px, int py, int width, int ptrX, char* buffer, int MaxStrlen, char* SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel, int ac_cancel, int fn_cancel, int color, int backcolor, int miniflag, int displaystatus ) {
	char buffer2[1024];
	char buf[32];
	char stbuf[384*2*24];
	char Inbuf[384*2*24];
	char fnbuf[384*2*24];
	int key=0;
	int keyH,keyL;
	int cont=1;
	int i,j,k;
	int csrX;
	int offsetX=0;	// offsetX:  buffer offset for display
	int length;		// buffer length (<MaxStrlen)
	int csrwidth;	// 
	int InsertMode;
	int key2,multibyte=0;	// 0:mono   non 0:multi
	int dspX;
	int oplen,oplenL,oplenR;
	char alphalock_bk ;
	int ContinuousSelect=0;
	int EditFontSize_bk=EditFontSize;
	int wk=18,hk=24;
	int slash=displaystatus & 0xF00;
	int ClipStartPtr = -1 ;
	int ClipEndPtr   = -1 ;
	int H,S,V,L;
	int StatusDisp = CB_StatusDisp;
	int	StatusOS   = SysCalljmp(1,0,0,0,0x2B7);
	int selectStr=0;
	char *string;
	
 	unsigned short col=0x0000;
	if ( color < 0 ) color=0x0000;
	if ( miniflag ) { wk=12; hk=18; }

	memcpy( stbuf, PictAry[0],                384*2*24);		// status line image save
	memcpy( Inbuf, PictAry[0]+384*2*(py+24),  384*2*24);		// Inp line image save
	memcpy( fnbuf, PictAry[0]+384*2*24*(7+1), 384*2*24);		// fn key image save
	
	csrX=ptrX*wk;	// ptrX:  current buffer ptr      csrX: ccursor px (0~383)
	
//	if ( x + width > 22 ) width=22-x;
//	csrwidth=width; if ( x + csrwidth > 20 ) csrwidth=21-x;

	if ( (px/wk+1)+width > 383/wk+1 ) width=383/wk+1-(px/wk+1);
	csrwidth=width; if ( (px/wk+1)+csrwidth > 383/wk ) csrwidth=383/wk+1-(px/wk+1);

	if ( MaxStrlen > 1023 ) MaxStrlen = 1023;
	for(i=0; i<=MaxStrlen; i++) buffer2[i]=buffer[i]; // backup

	CommandType=0; CommandPage=0;
	if ( alpha_mode == 0 ) { Setup_SetEntry(0x14, 0x00 ); alphastatus = 0; alphalock = 0; }
	if ( Setup_GetEntry(0x14) == 0x00 ) { alphastatus = 0; }	// dummy
	if ( Setup_GetEntry(0x14) == 0x00 ) { alphalock = 0; }	// dummy
//	if ( Setup_GetEntry(0x14) >= 0x04 ) alphastatus = 1; else alphastatus = 0;
//	if ( Setup_GetEntry(0x14) >= 0x84 ) alphalock = 1; 
//	if ( ( float_mode == 0 ) && ( exp_mode == 0 ) && ( alpha_mode ) && ( fn_cancel==0 ) ) {
//		EnableDisplayStatusArea();
//		Setup_SetEntry(0x14, 0x84 );	// alpha lock 
//		alphastatus = 1;
//		alphalock = 1;
//		SetAlphaStatus( alphalock, lowercase );
//	}
	
//	SaveDisp(SAVEDISP_PAGE1);

//	if ( ( StatusDisp==0 ) && ( StatusOS==0 ) && ( py >= -2 ) ) {
//		CB_SetStatusDisp( 1 );
//		StatusArea_Run();
//		EnableDisplayStatusArea();
//		Bdisp_PutDisp_DD_stripe( 0, 23);
//	}
	if ( ( StatusOS==0 ) && ( py >= -2 ) ) {
		CB_SetStatusDisp( 1 );
		EnableDisplayStatusArea();
		Bdisp_PutDisp_DD_stripe( 0, 23);
		PutKey( KEY_CTRL_NOP, 1 );
		GetKey_DisableMenuCatalog( &key );
	}

	while (cont) {
//		RestoreDisp(SAVEDISP_PAGE1);
		if ( ContinuousSelect & 0xFF ) {
			key=KEY_CTRL_F5;
			goto jmpF5;
		}
		
		MiniCursorClipMode = 0;
		CB_ColorIndex=color;	// current color index reset
		CB_BackColorIndex=backcolor;	// current color index reset
		if ( CB_ColorIndex < 0 ) CB_ColorIndex = 0x0000;
		if ( rev_mode ){ i=CB_ColorIndex; CB_ColorIndex=CB_BackColorIndex; CB_BackColorIndex=i; }	// reverse set
	
		InsertMode = Setup_GetEntry(0x15) == 02 ;	// 01:OverWrite   02:Insert
		if ( ( ClipStartPtr >=0 ) ) { CommandType=0; CommandPage=0; }
	
		memcpy( PictAry[0]+384*2*(py+24),  Inbuf, 384*2*24);		// Inp line image restore
		memcpy( PictAry[0]+384*2*24*(7+1), fnbuf, 384*2*24);		// fn key image restore
		length=strlenOp( buffer );
		if ( ptrX > length-1 ) ptrX=length;
		csrX=-1;
		for (i=0; i<1024; i++) {
			PrintOpcode( px, py,  buffer, width, offsetX, ptrX, &csrX, rev_mode , SPC, miniflag, ClipStartPtr, ClipEndPtr );
			if ( ( 0 <= csrX ) && ( csrX < 383 ) )   break;
			if ( offsetX < ptrX )	NextOpcodeGB( buffer, &offsetX);
				else				PrevOpcodeGB( buffer, &offsetX);
			if ( i==512 ) { ptrX=0; offsetX=0; csrX=-1; }
		}
		if ( csrX<  0 ) csrX=  0;
		if ( csrX>383 ) csrX=383;
		
		MiniCursorX=csrX-1-(miniflag!=0); if ( MiniCursorX<0 ) MiniCursorX=0;
		MiniCursorY=py;
		EditFontSize=0; if ( miniflag ) EditFontSize=2;
		MiniCursorSetFlashMode( 1 );		// mini cursor flashing on
	
		if ( ClipStartPtr>=0 ) {
			MiniCursorClipMode = 1;
			Fkey_Icon( FKeyNo1,  52 );	//	Fkey_dispN( FKeyNo1, "COPY ");
			Fkey_Icon( FKeyNo2, 105 );	//	Fkey_dispN( FKeyNo2, "CUT ");
			Fkey_Icon( FKeyNo3,   9 );	//	Fkey_dispN( FKeyNo2, "DEL ");
			FkeyClear( FKeyNo4 );
			FkeyClear( FKeyNo5 );
			FkeyClear( FKeyNo6 );
		} else {
			ClipEndPtr   = -1 ;		// ClipMode cancel
			if ( ( pallet_mode ) && ( alpha_mode ) ) if ( lowercase ) Fkey_dispN_aA( FKeyNo4, "A <> a"); else Fkey_dispN_Aa( FKeyNo4, "A <> a");
			if ( ( pallet_mode ) && ( alpha_mode ) ) Fkey_Icon( FKeyNo5, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
			if ( CommandInputMethod ) DispGenuineCmdMenu();
		}
	
//		sprintf3(buf,"len=%2d ptr=%2d off=%2d   csr=%2d  ",length,ptrX,offsetX,csrX); PrintMinix3( 0,7*8+2,(unsigned char *)buf, MINI_OVER);

		if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );	

		if ( ( CB_StatusDisp ) && ( py >= -2 ) ) {
				EnableDisplayStatusArea();
				Bdisp_PutDisp_DD_stripe( 0, 23);
		}

		if ( displaystatus ) GetKey_DisableMenuCatalog(&key);
		else GetKey_DisableMenu(&key);
		if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
		MiniCursorflag=0;
		MiniCursorFlashing();

		MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
		EditFontSize=EditFontSize_bk;
		CB_ColorIndex=color;	// current color index reset
		CB_BackColorIndex=backcolor;	// current color index reset
		
		if ( alphastatus == 1 ) {
			if ( KeyCheckPMINUS() ) {
//			if ( CheckKeyRow7305(1) & 0x08 ) {
				key = '%';
			}
		}
		switch (key) {
			case KEY_CTRL_NOP:
					ClipStartPtr = -1 ;		// ClipMode cancel+
					alphalock = 0 ;
					break;
					
			case KEY_CTRL_ALPHA:
			  alphaj:
				if ( ( alpha_mode ) ) {
					if ( alphastatus ) { 
						alphastatus = 0;
						alphalock = 0 ; 
						Setup_SetEntry(0x14, 0x00); 	// clear
					} else { 
						SetAlphaStatus( alphalock, lowercase ); 
						alphastatus = 1; 
					}
				}
				key=0;
				ClipStartPtr = -1 ;		// ClipMode cancel+
				break;
				
			case KEY_CTRL_AC:
				if ( length==0 ) cont = ac_cancel;
				ptrX=0; offsetX=0;
				buffer[0]='\0';
				alphalock = 0 ;
				break;
				
			case KEY_CTRL_EXIT:
				if ( CommandType ) {
					CommandType>>=4;
					CommandPage=CommandType & 0xF;
					CommandType>>=4;
				} else { 
		inpexit:	if ( exit_cancel == 0 ) {
						cont=0;
						for(i=0; i<=MaxStrlen; i++) buffer[i]=buffer2[i]; // restore
					}
					if ( ClipEndPtr < 0 ) cont=0;
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
				
			case KEY_CTRL_EXE:
				cont=0;
				break;
				
			case KEY_CTRL_DEL:
				if ( length ) {
					if ( ClipStartPtr >= 0 ) {
						goto F3del;
					} else {
						if ( InsertMode ) {		// insert mode
							PrevOpcodeGB( buffer, &ptrX );
							if ( offsetX ) if ( ptrX == offsetX ) PrevOpcodeGB( buffer, &offsetX );
						}
						DeleteOpcode1( buffer, MaxStrlen, &ptrX );
						key=0;
					}
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
				
			case 0x755A:		// ClipMode LEFT
			case KEY_CTRL_LEFT:
				PrevOpcodeGB( buffer, &ptrX );
				if ( ClipStartPtr>=0 ) ClipEndPtr=ptrX;
				key=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case 0x755B:		// ClipMode RIGHT
			case KEY_CTRL_RIGHT:
				if ( buffer[ptrX] != 0x00 )	NextOpcodeGB( buffer, &ptrX );
				if ( ClipStartPtr>=0 ) ClipEndPtr=ptrX;
				key=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case 0x7559:		// ClipMode UP
			case KEY_CTRL_UP:
				ptrX=0;
				offsetX=0;
				if ( ClipStartPtr>=0 ) ClipEndPtr=ptrX;
				key=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case 0x755C:		// ClipMode DOWN
			case KEY_CTRL_DOWN:
				ptrX=length;
				if ( ClipStartPtr>=0 ) ClipEndPtr=ptrX;
				key=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case KEY_CTRL_F1:
				if ( ClipStartPtr >= 0 ) {
					if ( ClipEndPtr < 0 ) goto F1j;
					if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
					EditCopy1( buffer, ClipBuffer, ptrX, ClipStartPtr, ClipEndPtr );
				} else 
				if ( CommandType ) GetGenuineCmdF1( &key );
				else if ( length ) cont= fn_cancel;
			F1j:alphalock = 0 ;
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
				
			case KEY_CTRL_F2:
			case 0x7563:		// ClipMode F2
				if ( ClipStartPtr >= 0 ) {
					if ( ClipEndPtr < 0 ) goto F2j;
					if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
					EditCutDel1( buffer, ClipBuffer, &ptrX, ClipStartPtr, ClipEndPtr, 0, MaxStrlen );	// cut
				} else
				if ( CommandType ) GetGenuineCmdF2( &key );
				else if ( length ) cont= fn_cancel;
			F2j:alphalock = 0 ;
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
				
			case KEY_CTRL_F3:
				if ( ClipStartPtr >= 0 ) {
				  F3del:					// clip delete
					if ( ClipEndPtr < 0 ) goto F3j;
					if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
					EditCutDel1( buffer, ClipBuffer, &ptrX, ClipStartPtr, ClipEndPtr, 1, MaxStrlen );	// delete
				} else
				if ( CommandType ) GetGenuineCmdF3( &key );
				else {
					if ( displaystatus ) if ( length ) { cont= fn_cancel; break; }
					if ( CommandInputMethod ) { 
						CommandType=CMD_MENU; CommandPage=0;
					} else {
						key=SelectOpcode5800P( 0 );
						if ( ( pallet_mode ) && ( alpha_mode ) ) if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
					}
				}
			F3j:ClipStartPtr = -1 ;		// ClipMode cancel
				break;
				
			case KEY_CTRL_F4:
				if ( ClipStartPtr >= 0 ) ClipStartPtr = -1 ;		// ClipMode cancel
				else
				if ( CommandType ) GetGenuineCmdF4( &key );
				else {
					if ( ( alpha_mode ) ) {
						lowercase=1-lowercase;
						if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
					}
					key=0;
				}
				break;
				
			case KEY_CTRL_F5:
				if ( ClipStartPtr >= 0 ) ClipStartPtr = -1 ;		// ClipMode cancel
				else
				if ( CommandType ) { GetGenuineCmdF5( &key );
					if ( key == KEY_CTRL_F5 ) { selectSetup=SetupG(selectSetup, 1); CommandType=0; }
				} else {
					Cursor_SetFlashOff(); 		// cursor flashing off
					if ( ( pallet_mode ) && ( alpha_mode ) ) {
					  jmpF5:
						key=SelectChar( &ContinuousSelect);
						if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
					}
				}
				break;
				
			case KEY_CTRL_F6:	// !=
				if ( ClipStartPtr >= 0 ) ClipStartPtr = -1 ;		// ClipMode cancel
				else
				if ( CommandType ) GetGenuineCmdF6( &key );
				else if ( slash ) key='/';
				break;
				
			case KEY_CTRL_SHIFT:
				if ( ClipStartPtr > 0 ) {  ClipStartPtr = -1 ;	// ClipMode cancel
					PrintOpcode( px, py,  buffer, width, offsetX, ptrX, &csrX, rev_mode , SPC, miniflag, ClipStartPtr, ClipEndPtr );
				}
				alphalock_bk = alphalock ;
				alphalock = 0 ;
				alphastatus = 0; 
				FkeyClearAll();
				if ( CommandInputMethod ) if ( displaystatus == 0 ) Menu_SHIFT_MENU();
				if ( ( pallet_mode ) && ( alpha_mode ) ) if ( lowercase  ) Fkey_dispN_aA( FKeyNo5, "A <> a"); else Fkey_dispN_Aa( FKeyNo5, "A <> a");
				if ( ( pallet_mode ) && ( alpha_mode ) ) Fkey_Icon( FKeyNo6, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
//				MiniCursorSetFlashMode( 1 );		// mini cursor flashing on
				if ( displaystatus ) GetKey_DisableMenuCatalog(&key);
				else GetKey_DisableMenu_pushpop(&key);
				if ( key==0 ) if ( KeyCheckCHAR6() ) key=KEY_CHAR_6;
				if ( key==0 ) if ( KeyCheckCHAR3() ) key=KEY_CHAR_3;
//				MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
				KeyRecover();
				switch (key) {
					case KEY_CTRL_QUIT:
							goto inpexit;
							
					case KEY_CTRL_CLIP:
							ClipStartPtr=ptrX;
							Setup_SetEntry(0x14, 02 );	// Clip mode
							key=0;
							break;
					case KEY_CTRL_PASTE:
							if ( ClipBuffer != NULL ) {
								EditPaste1( buffer, ClipBuffer, &ptrX, MaxStrlen );
							}
							key=0;
							break;
							
					case KEY_CTRL_ALPHA:
						  alphalockjp:
							if ( ( alpha_mode ) ) {
								alphalock = 1 ;
								alphastatus = 1; 
								SetAlphaStatus( alphalock, lowercase );
							}
							key=0;
							break;
					case KEY_CTRL_SHIFT:
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;

					case KEY_CTRL_F3:
							if ( displaystatus ) break;
							if ( CommandInputMethod ) {
								CommandType=CMD_SHIFT_VWIN; CommandPage=0;
							}
							key=0;
							break;
							
					case KEY_CTRL_F4:
							if ( displaystatus ) break;
							if ( CommandInputMethod ) {
								CommandType=CMD_SHIFT_SKTCH; CommandPage=0;
							}
							key=0;
							break;
							
					case KEY_CTRL_F5:	// A<>a
							if ( pallet_mode == 0 ) break;
							if ( ( pallet_mode ) && ( alpha_mode ) ) {
								lowercase=1-lowercase;
								if ( alphalock_bk ) goto alphalockjp;
								else
								if ( alphastatus ) SetAlphaStatus( alphalock, lowercase );
							}
							key=0;
							break;
					case KEY_CTRL_F6:	// CHAR
							if ( pallet_mode == 0 ) break;
							if ( ( pallet_mode ) && ( alpha_mode ) ) {
								key=SelectChar( &ContinuousSelect);
								if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
							}
							break;

					case KEY_CTRL_SETUP:
							if ( displaystatus ) break;
							if ( CommandInputMethod ) {
								CommandType=CMD_SETUP; CommandPage=0;
							} else {
								selectSetup=SetupG(selectSetup, 1);
							}
							key=0;
							break;
							
					case KEY_CTRL_PRGM:
							if ( displaystatus ) break;
							if ( CommandInputMethod ) {
								CommandType=CMD_PRGM; CommandPage=0;
							} else {
								key=SelectOpcode( CMDLIST_PRGM, 0 );
								if ( ( pallet_mode ) && ( alpha_mode ) ) if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
							}
							break;
							
					case KEY_CHAR_3:
							if ( displaystatus ) goto key0;
							MsgBoxPush( 1 );
							locate(3,4); Prints((unsigned char*)"Hit GetKey Code");
							KeyRecover();
							GetKey_DisableMenu(&key);
							MsgBoxPop();
							sprintf3(buf,"%d",CB_KeyCodeCnvt( key ) );
							EditPaste1( buffer, buf, &ptrX, MaxStrlen );
						  key0:
							key=0;
							break;
					case KEY_CHAR_6:
						RGBselect:
							if ( pallet_mode == 0 ) goto key0;
							if ( displaystatus ) goto key0;
							key=SetRGBColor( &col );
							if ( key != KEY_CTRL_EXE ) goto key0;
							switch ( SetRGBColorMode ) {
								case 0:	// RGB
									sprintf3(buf,"\x7F\x5E%d,%d,%d)",((col&0xF800) >> 8),((col&0x07E0) >> 3),((col&0x001F) << 3) );
									break;
								case 1:	// HSV -> HSL
									rgb2hsv( ((col&0xF800) >> 8), ((col&0x07E0) >> 3), ((col&0x001F) << 3), &H,&S,&V);
									sprintf3(buf,"\x7F\x71%d,%d,%d)",H,S,V );
									break;
								case 2:	// HSL -> RGB
									rgb2hsl( ((col&0xF800) >> 8), ((col&0x07E0) >> 3), ((col&0x001F) << 3), &H,&S,&L);
									sprintf3(buf,"\x7F\x73%d,%d,%d)",H,S,L );
									break;
							}
							EditPaste1( buffer, buf, &ptrX, MaxStrlen );
							key=0;
							break;
					case KEY_CTRL_FORMAT:
							if ( pallet_mode == 0 ) break;
							if ( displaystatus ) break;
							i=ColorIndexDialog0( 0 );
							switch ( i ) {
								case 0:	// black
									key=0xF99B;	
									break;
								case 1:	// blue
									key=0x7F35;	
									break;
								case 2:	// green
									key=0x7F36;	
									break;
								case 3:	// cyan
									key=0xF99E;	
									break;
								case 4:	// red
									key=0x7F34;	
									break;
								case 5:	// magenta
									key=0xF99D;	
									break;
								case 6:	// yellow
									key=0xF99F;	
									break;
								case 7:	// white
									key=0xF99C;	
									break;
								default:
									key=0;
									break;
							}
							break;
//					case KEY_CTRL_INS:
//							if (InsertMode )	Cursor_SetFlashOn(0x0);		// insert mode cursor 
//							else 					Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
//							Cursor_SetFlashOff();		// cursor flashing off
//							break;
					case KEY_CTRL_CAPTURE:
							SysCalljmp( 4,5,6,7,0x17E6);	// CAPTURE
							break;
					default:
							break;
				}
				break;

			case KEY_CTRL_OPTN:
				if ( displaystatus ) break;
				if ( CommandInputMethod ) { 
					CommandType=CMD_OPTN; CommandPage=0;
				} else {
					key=SelectOpcode( CMDLIST_OPTN, 0 );
					if ( ( pallet_mode ) && ( alpha_mode ) ) if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_VARS:
				if ( displaystatus ) break;
				if ( CommandInputMethod ) { 
					CommandType=CMD_VARS; CommandPage=0;
				} else {
					key=SelectOpcode( CMDLIST_VARS, 0 );
					if ( ( pallet_mode ) && ( alpha_mode ) ) if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_MENU:
				if ( displaystatus ) break;
				key=SelectOpcodeRecent( CMDLIST_RECENT );
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;

			default:
				break;
		}

		if ( key == 0x1F91B ) { 	// Graph Y Store
			i=CommandType;
			CB_StoreString( 1, buffer );
			CommandType=i;
			key=0;
		}
		if ( key == 0x2F91B ) { 	// Graph Y Recall
			i=CommandType;
			string = CB_RecallString( 1 );
			goto pastestring;
		}
		if ( key == 0x4F91B ) { 	// Graph Y See
			i=CommandType;
			string = CB_SeeString( 1, &selectStr, buffer );
		  pastestring:
			CommandType=i;
			if ( string != NULL ) {
				EditPaste1( buffer, string, &ptrX, MaxStrlen );
				UpdateLineNum=1;
			}
			key=0;
		}
		if ( key == 0xFFFF7F5D ) goto RGBselect;
		if ( ( displaystatus == 0 ) && ( ( alpha_mode || exp_mode ) ) ) {
			if ( (key&0xFF00FF00) == 0x5C005C00 ) goto gbnext;	// escape GB code
			keyH=(key&0xFF00) >>8 ;
			keyL=(key&0x00FF) ;
			if ( IsGBCode1(keyH,keyL) ) goto gbnext;
			switch ( keyH ) {		// ----- 2byte code -----
				case 0x5C:		// 
				case 0x7F:		// 
				case 0xF7:		// 
				case 0xF9:		// 
				case 0xE5:		// 
				case 0xE6:		// 
				case 0xE7:		// 
				case 0xFF:	// 
				  gbnext:
					if (ClipStartPtr>=0) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
						break;
					}
					if ( InsertMode ) {		// insert mode
						i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
					} else {					// overwrite mode
						if ( buffer[ptrX] != 0x00 ) DeleteOpcode1( buffer, MaxStrlen, &ptrX);
						i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
					}
					if ( i==0 ) NextOpcodeGB( buffer, &ptrX );
					alphalock = 0 ;
					alphastatus = 0;
					key=0;
					break;
				default:
					break;
			}
		}

		if (													// ----- 1 byte code -----
		   ( KEY_CHAR_0 <= key  ) && ( key <= KEY_CHAR_9     ) ||
		   ( hex_mode && ( KEY_CHAR_A <= key  ) && ( key <= KEY_CHAR_F ) ) ||
		   ( hex_mode && ( KEY_CTRL_XTT == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_LOG == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_LN  == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_SIN == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_COS == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_TAN == key ) ) ||
		   ( alpha_mode && ( 0x01 <= key  ) && ( key <= 0xFF ) ) ||
		   ( float_mode && ( key == KEY_CHAR_DP ) ) || ( key == KEY_CHAR_MINUS ) || ( key == KEY_CHAR_PMINUS ) || ( key == KEY_CHAR_PLUS ) ||
		   ( float_mode && exp_mode && ( key == KEY_CHAR_EXP )  ) ||
		   ( float_mode && ( 0x07 <= key  ) && ( key <= 0x0D ) ) ||
		   ( float_mode && ( 0x0F <= key  ) && ( key <= 0x13 ) ) ||
		   ( float_mode && ( 0x01 == key  ) ) ||
		   ( float_mode && ( 0x03 == key  ) ) ||
		   ( float_mode && exp_mode && ( 'A' <= key  ) && ( key <= 'Z' ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_FRAC == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_LOG == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_LN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_SIN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_COS == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_TAN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_SQUARE == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_POW == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_PI == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_CUBEROOT == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_RECIP == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_EXPN10 == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_EXPN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ASIN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ACOS == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ATAN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ROOT == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_POWROOT == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ASIN == key  ) ) ||
		   ( float_mode && exp_mode && ( 0x80 <= key  ) && ( key <= 0xFF ) ) ||
		   (  ( KEY_CHAR_MULT == key  ) ) ||
		   (  ( KEY_CHAR_DIV  == key  ) ) ||
		   (  ( KEY_CHAR_POW  == key  ) ) ||
		   (  ( KEY_CTRL_XTT  == key  ) )
		   ) {
			if ( displaystatus ) {
				if ( ( 0x21<=key )&&( key<=0x7E )&&(key!='"')&&(key!='*')&&(key!=':')&&(key!='<')&&(key!='>')&&(key!='?')&&(key!='|')&&(key!=0x5C) ) goto codeok;
				if ( ( slash )&&((key=='/')||(key==0x5C)) ) goto codeok;
			} else {
				if ( hex_mode && ( KEY_CTRL_XTT == key ) ) key=KEY_CHAR_A;
				if ( hex_mode && ( KEY_CHAR_LOG == key ) ) key=KEY_CHAR_B;
				if ( hex_mode && ( KEY_CHAR_LN  == key ) ) key=KEY_CHAR_C;
				if ( hex_mode && ( KEY_CHAR_SIN == key ) ) key=KEY_CHAR_D;
				if ( hex_mode && ( KEY_CHAR_COS == key ) ) key=KEY_CHAR_E;
				if ( hex_mode && ( KEY_CHAR_TAN == key ) ) key=KEY_CHAR_F;
				if ( hex_mode && ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) ) key+=('a'-'A');
//				if ( float_mode && ( key == KEY_CHAR_POW ) )    key='^';
				if ( ( key == KEY_CTRL_XTT ) ) key=XTTKey( key ); 	// 'X' or 0x90 or <r> or <Theta> or T
			  codeok:
				if ( ClipStartPtr >= 0 ) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
				} else {
					if ( InsertMode ) {		// insert mode
							i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
					} else {					// overwrite mode
						if ( buffer[ptrX] != 0x00 ) DeleteOpcode1( buffer, MaxStrlen, &ptrX);
						i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
					}
					if ( i==0 ) NextOpcodeGB( buffer, &ptrX );
					if ( alphalock == 0 ) alphastatus = 0;
					key=0;
				}
			}
		} else {
			if ( key != 0 ) {
				if ( alphalock == 0 ) alphastatus = 0;
//				alphalock = 0 ;
//				alphastatus = 0;
			}
		}

	}

	EditFontSize=EditFontSize_bk;
	Cursor_SetFlashOff();		// cursor flashing off
//	if ( alpha_mode ) { FkeyClear( FKeyNo4 ); FkeyClear( FKeyNo5 ); FkeyClear( FKeyNo6 );}

	CB_SetStatusDisp( StatusDisp );
	if ( StatusOS ) {
		EnableStatusArea(0);	// enable StatusArea
	} else {
		EnableStatusArea(3);	// disable StatusArea
	}
	
	memcpy( PictAry[0], stbuf,  384*2*24);						// status line image restore
	memcpy( PictAry[0]+384*2*24*(7+1), fnbuf, 384*2*24);		// fn key image restore
	buffer[length]='\0';
	return key ;	// EXE:0
}


int InputStrSub_mini(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char* SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel, int ac_cancel, int miniflag) {
	int key;
	int color=CB_ColorIndex;	// current color index 
	unsigned short bcolor=CB_BackColorIndex;	// current color index 
	if ( miniflag==0 ) { x=(x-1)*18; y=(y-1)*24; }
	key=InputStrSubC( x, y, width, ptrX, buffer, MaxStrlen, SPC, rev_mode, float_mode, exp_mode, alpha_mode, hex_mode, pallet_mode, exit_cancel, ac_cancel, FN_CANCEL_ON, -1, 0xFFFF, miniflag, 0 );
	CB_ColorIndex=color;	// current color index restore
	CB_BackColorIndex=bcolor;	// current color index restore
	return key;
}
int InputStrSub(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char* SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel, int ac_cancel) {
	return InputStrSub_mini( x, y, width, ptrX, buffer, MaxStrlen, SPC, rev_mode, float_mode, exp_mode, alpha_mode, hex_mode, pallet_mode, exit_cancel, ac_cancel, 0 );
}

int InputStrSubFn(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char* SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel, int ac_cancel) {
	int key;
	int color=CB_ColorIndex;	// current color index 
	unsigned short bcolor=CB_BackColorIndex;	// current color index 
	key=InputStrSubC( (x-1)*18, (y-1)*24, width, ptrX, buffer, MaxStrlen, SPC, rev_mode, float_mode, exp_mode, alpha_mode, hex_mode, pallet_mode, exit_cancel, ac_cancel, FN_CANCEL_OFF, -1, 0xFFFF, 0, 0 );
	CB_ColorIndex=color;	// current color index restore
	CB_BackColorIndex=bcolor;	// current color index restore
	return key;
}
int InputStrSub_status(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char* SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel, int slash ) {
	int key;
	int color=CB_ColorIndex;	// current color index 
	unsigned short bcolor=CB_BackColorIndex;	// current color index 
	key=InputStrSubC( (x-1)*18, (y-1)*24, width, ptrX, buffer, MaxStrlen, SPC, rev_mode, float_mode, exp_mode, alpha_mode, hex_mode, pallet_mode, exit_cancel, AC_CANCEL_OFF, FN_CANCEL_OFF, -1, 0xFFFF, 0, 1+ slash*0x100 );
	CB_ColorIndex=color;	// current color index restore
	CB_BackColorIndex=bcolor;	// current color index restore
	return key;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
double InputNumD(int x, int y, int width, double defaultNum, char* SPC, int rev_mode, int float_mode, int exp_mode ,int *key ) {		// 0123456789.(-)exp
	char buffer[64];
	int csrX=0;
	double result;
	buffer[csrX]='\0';
	if ( defaultNum != 0 ) {
		sprintG(buffer, defaultNum, width, LEFT_ALIGN);
		csrX=strlenOp(buffer);
	}
	do {
		*key= InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, float_mode, exp_mode, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF ) ;
		if ( ( *key == KEY_CTRL_EXIT ) || ( *key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = Eval( buffer );
		csrX   = ErrorPtr;
	} while ( ErrorNo || (buffer[0]=='\0') ) ;	// error loop
	return result; // value ok
}

int InputStr(int x, int y, int width,  char* buffer, int MaxStrlen, char* SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	buffer[MaxStrlen]='\0';
	csrX=strlenOp(buffer);
	return  InputStrSub( x, y, width, csrX, buffer, MaxStrlen, SPC, rev_mode, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF );
}
int InputStr_CB(int x, int y, int width,  char* buffer, int MaxStrlen, char* SPC, int rev_mode ) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	buffer[MaxStrlen]='\0';
	csrX=strlenOp(buffer);
	return InputStrSubC( (x-1)*18, (y-1)*24, width, csrX, buffer, MaxStrlen, SPC, rev_mode, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF, FN_CANCEL_ON, CB_ColorIndex, CB_BackColorIndex, 0, 0 );
}
int InputStr_CB2(int px, int py, int width,  char* buffer, int MaxStrlen, char* SPC, int rev_mode, int miniflag ) {		// ABCDEF0123456789.(-)exp	+mini
	int csrX=0;
	buffer[MaxStrlen]='\0';
	csrX=strlenOp(buffer);
	return InputStrSubC( px, py, width, csrX, buffer, MaxStrlen, SPC, rev_mode, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF, FN_CANCEL_ON, CB_ColorIndex, CB_BackColorIndex, miniflag, 0 );
}
//----------------------------------------------------------------------------------------------
complex InputNumC_sub_mini(int x, int y, int width, int ptrX, complex defaultNum, int miniflag ) {		//  1st char key in
	unsigned int key;
	complex result;
	do {
		key= InputStrSub_mini( x, y, width, ptrX, ExpBuffer, ExpMax-1, " ", REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF, miniflag ) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = Cplx_Eval( ExpBuffer );
		ptrX = ErrorPtr;
	} while ( ErrorNo || (ExpBuffer[0]=='\0') ) ;	// error loop
	CB_CurrentValue = result ;
	return result; // value ok
}
complex InputNumC_sub(int x, int y, int width, int ptrX, complex defaultNum  ) {		//  1st char key in
	return InputNumC_sub_mini(x, y, width, ptrX, defaultNum, 0 );
}

complex InputNumC_fullsub_mini(int x, int y, int width, complex defaultNum, int miniflag ) {		// full number display
	return InputNumC_sub_mini(x, y, width, 0, defaultNum, miniflag );
}
complex InputNumC_fullsub(int x, int y, int width, complex defaultNum ) {		// full number display
	return InputNumC_sub_mini(x, y, width, 0, defaultNum, 0 );
}

complex InputNumC_full_mini(int x, int y, int width, complex defaultNum, int miniflag ) {		// full number display
	int eng=ENG;
	if (ENG==3) ENG=0;
	Cplx_sprintGR1s(ExpBuffer, defaultNum, ExpMax-1, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT, 0, 1 );	// input mode
	ENG=eng;
	return InputNumC_sub_mini(x, y, width, 0, defaultNum, miniflag );
}
complex InputNumC_full(int x, int y, int width, complex defaultNum ) {		// full number display
	return InputNumC_full_mini( x,  y, width, defaultNum, 0 );
}

double InputNumD_full_mini(int x, int y, int width, double defaultNum, int miniflag) {		// full number display
	int eng=ENG;
	if (ENG==3) ENG=0;
	sprintG(ExpBuffer, defaultNum, ExpMax-1, LEFT_ALIGN);
	ENG=eng;
	return InputNumC_sub_mini(x, y, width, 0, Dbl2Cplx(defaultNum), miniflag ).real;
}
double InputNumD_full(int x, int y, int width, double defaultNum) {		// full number display
	return InputNumD_full_mini( x, y, width, defaultNum, 0 );
}

complex InputNumC_fullhex_mini(int x, int y, int width, complex defaultNum, int hex, int miniflag) {		// full number display hex
	if ( hex ) {
		if ( ( defaultNum.imag==0 ) && ( (defaultNum.real-floor(defaultNum.real))==0 ) && ( -2147483648. <= defaultNum.real ) && ( defaultNum.real <= 2147483647. ) ) {
			sprintf(ExpBuffer,"0x%08X",(int)defaultNum.real);
			return InputNumC_sub_mini(x, y, width, 0, Dbl2Cplx(defaultNum.real), miniflag );
		}
	}
	return InputNumC_full_mini(x, y, width, defaultNum, miniflag );
}
complex InputNumC_fullhex(int x, int y, int width, complex defaultNum, int hex) {		// full number display hex
	return InputNumC_fullhex_mini( x,  y, width, defaultNum, hex, 0 ); 
}

double InputNumD_Char_mini(int x, int y, int width, double defaultNum, int code, int miniflag ){		//  1st char key in
	ExpBuffer[0]='\0';
	InsertOpcode1( ExpBuffer, ExpMax, 0, code );
	return InputNumC_sub_mini( x, y, width, OpcodeLenGB(code), Dbl2Cplx(defaultNum), miniflag ).real;
}
double InputNumD_Char(int x, int y, int width, double defaultNum, int code) {		//  1st char key in
	return InputNumD_Char_mini( x, y, width, defaultNum, code, 0 );
}

complex InputNumC_Char_mini(int x, int y, int width, complex defaultNum, int code, int miniflag ){		//  1st char key in
	ExpBuffer[0]='\0';
	InsertOpcode1( ExpBuffer, ExpMax, 0, code );
	return InputNumC_sub_mini( x, y, width, OpcodeLenGB(code), defaultNum, miniflag );
}
complex InputNumC_Char(int x, int y, int width, complex defaultNum, int code) {		//  1st char key in
	return InputNumC_Char_mini( x, y, width, defaultNum, code, 0 );
}

double InputNumD_replay_mini(int x, int y, int width, double defaultNum, int miniflag) {		//  replay expression
	return InputNumC_sub_mini( x, y, width, strlenOp((char*)ExpBuffer), Dbl2Cplx(defaultNum), miniflag ).real;
}
double InputNumD_replay(int x, int y, int width, double defaultNum) {		//  replay expression
	return InputNumD_replay_mini( x, y, width, defaultNum, 0 );
}

complex InputNumC_replay_mini(int x, int y, int width, complex defaultNum, int miniflag) {		//  replay expression
	return InputNumC_sub_mini( x, y, width, strlenOp((char*)ExpBuffer), defaultNum, miniflag );
}
complex InputNumC_replay(int x, int y, int width, complex defaultNum) {		//  replay expression
	return InputNumC_replay_mini( x, y, width, defaultNum, 0 );
}

//-----------------------------------------------------------------------------
complex InputNumC_CB(int x, int y, int width, int MaxStrlen, char* SPC, int REV, complex defaultNum) {		//  Basic Input
	unsigned int key;
	complex result;
	ExpBuffer[0]='\0';
	ErrorPtr=0;
	do {
		key=InputStrSubC( (x-1)*18, (y-1)*24, width, ErrorPtr, ExpBuffer, MaxStrlen, SPC, REV, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON, AC_CANCEL_OFF, FN_CANCEL_ON, CB_ColorIndex, CB_BackColorIndex, 0, 0 );
		if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return Int2Cplx(0); }
		result = Cplx_Eval( ExpBuffer );
	} while ( ErrorNo || (ExpBuffer[0]=='\0') ) ;	// error loop
	return result; // value ok
}
complex InputNumC_CB2_sub(int px, int py, int width, int MaxStrlen, int ptrX, char* SPC, int REV, complex defaultNum, int miniflag ) {		//  Basic Input sub
	int key,i;
	complex result;
	int csrX;
	do {
		key=InputStrSubC( px, py, width, ptrX, ExpBuffer, MaxStrlen, SPC, REV, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON, AC_CANCEL_OFF, FN_CANCEL_ON, CB_ColorIndex, CB_BackColorIndex, miniflag, 0 );
		if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return Int2Cplx(0); }
		if ( ExpBuffer[0]=='\0' ) if ( key==KEY_CTRL_EXE  ) { result=(defaultNum); goto exit2; }
		result = Cplx_Eval( ExpBuffer );
		ptrX   = ErrorPtr;
	} while ( ErrorNo || (ExpBuffer[0]=='\0') ) ;	// error loop
  exit2:
	if ( REV ){ i=CB_ColorIndex; CB_ColorIndex=CB_BackColorIndex; CB_BackColorIndex=i; }	// reverse set
	i=PrintOpcode( px, py, ExpBuffer, width, 0, 0, &csrX, REV , " ", miniflag, -1,-1 );
	if ( i==0 ) Cplx_sprintGR1cutlim( ExpBuffer, result, MaxStrlen, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );	//
	if ( REV ){ i=CB_ColorIndex; CB_ColorIndex=CB_BackColorIndex; CB_BackColorIndex=i; }	// reverse set
	return result; // value ok
}

complex InputNumC_CB1(int x, int y, int width, int MaxStrlen, char* SPC, int REV, complex defaultNum) {		//  Basic Input 1
	ExpBuffer[0]='\0';
	return InputNumC_CB2_sub( (x-1)*18, (y-1)*24, width, MaxStrlen, 0, SPC, REV, defaultNum, 0 );
}
complex InputNumC_CB2(int px, int py, int width, int MaxStrlen, char* SPC, int REV, complex defaultNum, int miniflag, int dispzero ) {		//  Basic Input 2 +mini
	if ( ( dispzero==0 ) && ( Cplx_fcmpEQ_0( defaultNum ).real ) ) ExpBuffer[0]='\0';
	else Cplx_sprintGR1s(ExpBuffer, defaultNum, MaxStrlen, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT, 0, 1 );	// input mode
	return InputNumC_CB2_sub( px, py, width, MaxStrlen, strlenOp((char*)ExpBuffer), SPC, REV, defaultNum, miniflag);
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void  CB_Input( char *SRC ){
	int key;
	int c;
	complex DefaultValue={0,0};
	int flag=0,flagint=0;
	int reg,bptr,mptr;
	int dimA,dimB,base;
	char buffer[256];
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int		width=255,length=ExpMax-1,option=0,rev=REV_OFF;
	char	spcchr[]={0x20,0,0};
	int	pxmode=0,miniflag=0;
	int miniCsrX,miniCsrY;
	char buffer2[128];
 	int bk_GBcode=GBcode;
	int aryN,aryS;
	char *StrFnPtr;
 	
	KeyRecover();
	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
	if ( CursorX==22 ) CursorX=1;
	if ( CursorX==23 ) Scrl_Y();
	miniCsrX=(CursorX-1)*18;
	miniCsrY=(CursorY-1)*24;

	CB_ChangeTextMode( SRC ) ;
//	CB_SelectTextDD();	// Select Text Screen
	EnableDisableGB( SRC );	// GB mode enable ##    GB mode disable %%
	c=SRC[ExecPtr];
	if ( c=='(' ) {	// ?option([@][csrX][,csrY][,width][,spcchr][,length][,R][,M])
		c=SRC[++ExecPtr];
		if ( c=='@' ) { pxmode=1; c=SRC[++ExecPtr]; }	// 0~383 pixel mode
//		else CB_SelectTextDD();	// Select Text Screen
		if ( ( c==')' ) || ( c==0x0E ) ) goto optionexit;
		if ( c!=',' ) {
			if ( pxmode ) {
				miniCsrX = CB_EvalInt( SRC );
				if ( ( miniCsrX<0 ) || ( 360<miniCsrX ) ) { CB_Error(OutOfDomainERR); return ; } // Out of Domain error
			} else  { 
				CursorX = CB_EvalInt( SRC );
				if ( ( CursorX<1 ) || ( 21<CursorX ) ) { CB_Error(OutOfDomainERR); return ; } // Out of Domain error
			}
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {
			if ( pxmode ) {
				miniCsrY = CB_EvalInt( SRC );
				if ( ( miniCsrY<-24 ) || ( 191<miniCsrY ) ) { CB_Error(OutOfDomainERR); return ; } // Out of Domain error
			} else  { 
				CursorY = CB_EvalInt( SRC );
				if ( ( CursorY<0 ) || ( 8<CursorY ) ) { CB_Error(OutOfDomainERR); return ; } // Out of Domain error
			}
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {
			width=CB_EvalInt( SRC ); if ( ( width<1 ) || (  32<width ) ) { CB_Error(OutOfDomainERR); return ; } // Out of Domain error
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {	
			c=CB_IsStr( SRC, ExecPtr );
			if ( c ) {	// string
				CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
			} else { CB_Error(ArgumentERR); return ; } // Argument error
			spcchr[0]=buffer[0];
			spcchr[1]=buffer[1];
			spcchr[2]=buffer[2];
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {
			length=CB_EvalInt( SRC ); if ( ( length<1 ) || ( ExpMax-1<length ) ) { CB_Error(ArgumentERR); return ; } // Argument error
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {	
			if ( ( c=='R' ) || ( c=='r' ) ) { // reverse
				ExecPtr++;
				rev=REV_ON;
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
			}
		}
		c=SRC[++ExecPtr];
		if ( c!=')' ) {	
			if ( ( c=='M' ) || ( c=='m' ) ) { // mini font
				ExecPtr++;
				miniflag=pxmode;
			}
		}
	  optionexit:
		option=1;
		if ( pxmode==0 ) { miniCsrX=(CursorX-1)*18; miniCsrY=(CursorY-1)*24; }
		if ( SRC[ExecPtr]==')' ) ExecPtr++;
	} else {
		locate( CursorX, CursorY); Prints((unsigned char*)"?");
		Scrl_Y();
	}
	
	c=SRC[ExecPtr];
	bptr=ExecPtr;
	reg=RegVar(c);
	if ( c==0x0E ) {	// ->
		flag=0;
		if ( CB_IsStr( SRC, ExecPtr+1 ) ) flag=2;
	} else 
	if ( reg>=0 ) {
	  regj:
		flag=1;
		c=SRC[ExecPtr+1];
		if (CB_INT==1) {
			if ( c=='#' ) {
				DefaultValue = LocalDbl[reg][0] ;
			} else { flagint=1; 
				if ( c=='[' ) {
					ExecPtr+=2;
					MatOprandInt2( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else
				if ( ( '0'<=c )&&( c<='9' ) ) {
					ExecPtr++;
					dimA=c-'0';
					MatOprand1num( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else DefaultValue = Int2Cplx( LocalInt[reg][0] );
			}
		} else {
			if ( c=='%' ) { flagint=1; 
				DefaultValue = Int2Cplx( LocalInt[reg][0] );
			} else {
				if ( c=='[' ) {
					ExecPtr+=2;
					MatOprand2( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else
				if ( ( '0'<=c )&&( c<='9' ) ) {
					ExecPtr++;
					dimA=c-'0';
					MatOprand1num( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else DefaultValue = LocalDbl[reg][0] ;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( ( c == 0x40 ) || ( c == 0xFFFFFF84 ) || ( ( c == 0x51 ) || ( (0x6A<=c)&&(c<=0x6F) ) ) ) {	// Mat A[a,b] or Vct A[a] or List 1[a]
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			DefaultValue = Cplx_ReadMatrix( reg, dimA, dimB);
			ExecPtr=bptr;
		} else if ( ( 0xFFFFFF91 <= c ) && ( c <= 0xFFFFFF93 ) ) {	// F Start~F pitch
				ExecPtr+=2;
				DefaultValue.real = REGf[c-0xFFFFFF90] ;
		} else if ( c == 0x00 ) {	// Xmin
				DefaultValue.real = Xmin ;
		} else if ( c == 0x01 ) {	// Xmax
				DefaultValue.real = Xmax ;
		} else if ( c == 0x02 ) {	// Xscl
				DefaultValue.real = Xscl ;
		} else if ( c == 0x04 ) {	// Ymin
				DefaultValue.real = Ymin ;
		} else if ( c == 0x05 ) {	// Ymax
				DefaultValue.real = Ymax ;
		} else if ( c == 0x06) {	// Yscl
				DefaultValue.real = Yscl ;
		} else if ( c == 0x08) {	// Thetamin
				DefaultValue.real  = TThetamin ;
		} else if ( c == 0x09) {	// Thetamax
				DefaultValue.real  = TThetamax ;
		} else if ( c == 0x0A) {	// Thetaptch
				DefaultValue.real  = TThetaptch ;
		} else if ( c == 0x0B ) {	// Xfct
				DefaultValue.real = Xfct ;
		} else if ( c == 0x0C ) {	// Yfct
				DefaultValue.real = Yfct ;
		} else if ( c == 0xFFFFFFF0 ) {	// GraphY
			reg=defaultGraphAry;
			aryN=defaultGraphAryN;
			aryS=defaultGraphArySize;
		  	c=6;		// +offset
			goto strj;
		} else {
			goto exitj;
		}
		flag=1;
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x3F ) {	// Str 1-20
			reg=defaultStrAry;
			aryN=defaultStrAryN;
			aryS=defaultStrArySize;
		  strj0:
		  	c=0;
		  strj:
			ExecPtr+=2;
			StrFnPtr = GetStrYFnPtr( SRC, reg, aryN+1-MatBase, aryS ) +c;
			if ( ErrorNo ) return ;			// error
			flag=3;
			ExecPtr=bptr;
		} else
		if ( c == 0x1B ) {	// fn
			reg=defaultFnAry;
			aryN=defaultFnAryN;
			aryS=defaultFnArySize;
			goto strj0;
		} else
		if ( c == 0x41 ) {	// DATE
			ExecPtr+=2;
			flag=4;
			ExecPtr=bptr;
		} else
		if ( c == 0x42 ) {	// TIME
			ExecPtr+=2;
			flag=5;
			ExecPtr=bptr;
		} else
		if ( c == 0x21 ) {	// Xdot
				DefaultValue.real = Xdot ;
				flag=1;
		} else goto exitj;
	} else
	if ( c=='$' ) {
		ExecPtr++;
		MatrixOprand( SRC, &reg, &dimA, &dimB );
		if ( ErrorNo ) return ; // error
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		if ( MatAry[reg].ElementSize != 8 ) { CB_Error(ArgumentERR); return; }	// element size error
		flag=3;
		ExecPtr=bptr;
	} else {
	  exitj:
		reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
		CB_Error(SyntaxERR); return; }	// Syntax error

	switch ( flag ) {
		case 0:	// ? -> A value
			if ( option ) {
				CB_CurrentValue = InputNumC_CB2( miniCsrX, miniCsrY, width, length, spcchr, rev, Int2Cplx(0), miniflag, 0 );	// zero not disp
			} else {
				CB_CurrentValue = InputNumC_CB(  CursorX, CursorY, width, length, spcchr, rev, Int2Cplx(0) );
			}
			ExecPtr++;
			if ( CB_INT==1 ) flagint=1;
		  vinp:
			ErrorNo=0; // error cancel
			if ( BreakPtr > 0 ) { ExecPtr=BreakPtr; goto exit; }
			CBint_CurrentValue = CB_CurrentValue.real ;
			if ( flagint ) {
				CBint_Store( SRC );
				break;
			}
			CB_Store( SRC );
			break;
		case 1:	// ?A value
			if ( option ) {
				CB_CurrentValue = InputNumC_CB2( miniCsrX, miniCsrY, width, length, spcchr, rev, DefaultValue, miniflag, 1 );	// zero disp
			} else {
				buffer2[0]='\0';
				Cplx_sprintGR2( buffer, buffer2, DefaultValue, 22-CursorX, RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
				CB_Prints_ext( CursorX, CursorY, (unsigned char*)buffer, 0x000 );	// 
				if ( buffer2[0] != '\0' ){
					Scrl_Y();
					CB_Prints_ext( CursorX, CursorY, (unsigned char*)buffer2, 0x000 );	// 
				}
				Scrl_Y();
				CB_CurrentValue = InputNumC_CB1( CursorX, CursorY, width, length, spcchr, rev, DefaultValue );
			}
			goto vinp;
			break;
		case 2:	// ? -> str 
			CB_CurrentStr=buffer;
			CB_CurrentStr[0]='\0';
	Inpj1:	if ( option == 0 ) CursorX=1;
			if ( option ) key=InputStr_CB2( miniCsrX, miniCsrY, width, CB_CurrentStr, length, spcchr, rev, miniflag );
			else		  key=InputStr_CB(  CursorX, CursorY, width, CB_CurrentStr, length, spcchr, rev);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr;  goto exit; }
			if ( SRC[ExecPtr]==0x0E ) ExecPtr++;	// -> skip
			CB_StorStr( SRC );
			break;
		case 3:	// ?$Mat  ?Str1-20
			CB_CurrentStr=buffer;
//			OpcodeStringToAsciiString(buffer, StrFnPtr, 255);
			strncpy( buffer,  StrFnPtr, 255);
			if ( width > MatAry[reg].SizeB-1 ) width=MatAry[reg].SizeB-1;
			if ( option ) key=InputStr_CB2( miniCsrX, miniCsrY, width, CB_CurrentStr, length, spcchr, rev, miniflag );
			else		  key=InputStr_CB(  CursorX, CursorY, width, CB_CurrentStr, length, spcchr, rev);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr;  goto exit; }
			CB_StorStr( SRC );
			break;
		case 4:	// ?DATE
			CB_DateToStr();
			CB_CurrentStr[10]='\0';	// week cancel
	Inpj2:
			goto Inpj1;
			break;
		case 5:	// ?TIME
			CB_TimeToStr();
			goto Inpj2;
			break;
	}
	if ( option == 0 ) Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
  exit:
	GBcode=bk_GBcode;
	CB_ColorIndex=-1;
	return ;
}


//---------------------------------------------------------------------------------------------- align dummy
int InpObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int InpObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
int InpObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int InpObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int InpObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
//int InpObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
//int InpObjectAlign4m( unsigned int n ){ return n; }	// align +4byte

}
