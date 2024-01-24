/*****************************************************************/
/*                                                               */
/*   inp Library  ver 1.3                                        */
/*                                                               */
/*   written by sentaro21                                        */
/*                                                               */
/*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"
#include "CB_inp.h"
#include "CB_io.h"
#include "CB_Kana.h"

//----------------------------------------------------------------------------------------------
//int inpObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int inpObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int inpObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

double Round( double num, int round_mode, int digit){
	int minus=0,exp,ex2;
	double fabsnum=fabs(num),tmp,tmp2,tmp3,exf;

	if ( num < 0 ) minus=-1;

	switch ( round_mode ) {
		case Fix: // fix
			if (fabsnum<1e15) {
				tmp=pow(10,digit);
				num=floor(fabsnum*tmp+.5)/tmp;
				fabsnum=num;
				if ( minus ) num=-num;
			}
			break;
		case Norm:	// normal
			if ( digit==1 ) digit=10;
			if ( digit==2 ) digit=10;
		case Sci: // sci
			if ( digit==0 ) break;
			if ( digit>=16 ) break;
			exf=log10(fabsnum);
			exp=exf;
			if ( exf > 0 ) exp++;
			exf=pow(10,exp);
			tmp=fabsnum/exf;
			tmp2=pow(10,digit);
			tmp3=floor(tmp*tmp2+.5)/tmp2;
			num=tmp3*exf;
			if ( minus ) num=-num;
			break;
	}
	return num;
}

void sprintGRS( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) { // + round
	int i,j,w,adj,minus=0,p,digit=round_digit;
	char buffer2[32],fstr[16],tmp[16];
	double fabsnum,pw;
	unsigned char c;
	char *nptr;
	char *dptr,*eptr;
	double dpoint=0.01;

	if ( num < 0 ) minus=-1;
	switch ( round_mode ) {
		case Norm:
			if ( round_digit==1 ) { dpoint=0.01;        digit=10; }
			if ( round_digit==2 ) { dpoint=0.000000001; digit=10; }
			if ( round_digit==0 ) digit=16;
//			if ( round_digit==2 ) { dpoint=0.000000001; digit=10; i=11; c='f'; break; }
//			num = Round( num, round_mode, digit);
			fabsnum=fabs(num);
		 	w=15; if ( w > width )  w=width;
		 	pw=pow(10,w+minus);
			if ( ( fabsnum==0 ) || ( ( dpoint <= fabsnum ) && ( fabsnum < pw ) ) ) {
				w = floor((log10(fabsnum))) + (15-digit);
				if ( digit >= width ) w= w+(digit-width);
				i=14-w;
				if ( i >= 18 ) i=18;
				c='f';
				if ( i <  0  ) { i=digit-1; c='e'; }
				if ( i <  0  ) i=15;
				if ( round_digit==2 ) if ( i>11 ) i=11;
			} else {
				adj = 1 - minus+ floor(log10(fabs(log10(fabsnum))))-1;
				if ( ( 1e-10 <= fabsnum ) && ( fabsnum < dpoint )) adj++;
				i=width-adj-5;
				if ( i > digit-1 ) i=digit-1;
				if ( i >= 18 ) i=18;
				if ( i <  1  ) i=0;
				if ( round_digit==2 ) if ( i>11 ) i=11;
				c='e';
			}
			break;
		case Fix:
				i=digit;
				c='f';
			break;
		case Sci:
				num = Round( num, round_mode, digit);
				i=digit-1; if ( i < 0 ) i=15;
				c='e';
			break;
	}
	
	p=0;
	fstr[p++]='%';
	fstr[p++]='.';
	if ( i >= 10 ) fstr[p++]='0'+i/10;
	fstr[p++]='0'+i%10;
	fstr[p++]=c;
	fstr[p++]='\0';
	sprintf((char*)buffer, fstr, num);

	if ( round_mode == Norm ) {
		dptr=strchr((char*)buffer,'.');
		if ( dptr ) {
			eptr=strchr((char*)buffer,'e');
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
	}

	if ( ENG==3 )  {	// 3 digit separate
		for(i=0; i<22; i++) buffer2[i]=buffer[i]; // 
		nptr=strchr(buffer,'.');
		if ( nptr==NULL ) w=strlen(buffer)+minus; else w=nptr-buffer+minus;
		if ( w < 4  ) goto align;
		i=0; j=0;
		if ( minus ) buffer[i++]=buffer2[j++];
		do {
			buffer[i++]=buffer2[j++];
			w--;
			if ( ( w==3 ) || ( w==6 )|| ( w==9 )|| ( w==12 )|| ( w==15 ) ) buffer[i++] = ',';
		} while ( buffer2[j] ) ;
		buffer[i]='\0';
	}
	align:
	if ( align_mode == RIGHT_ALIGN ) {
		for(i=0; i<22; i++) buffer2[i]=buffer[i]; // 
		w=strlen((char*)buffer2);
		if ( w < width ) {
			for ( i=1; i<=w; i++)
			 buffer2[width-i]=buffer2[w-i];
			for ( i=0; i<width-w; i++) buffer2[i]=' ';
		}
		for(i=0; i<22; i++) buffer[i]=buffer2[i]; // 
	}
	
	buffer[width]='\0';

	i=-1;
	while (i<width) {
		c=buffer[++i];
		switch ( c ) {
			case '-':
				buffer[i]=0x87;	// (-)
				break;
//			case '+':
//				buffer[i]=0x89;	// (+)
//				break;
			case 'e':	// exp
				buffer[i]=0x0F;	// (exp)
				break;
		}
	}

	return ;
}

void sprintGR( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) { // + round  ENG
	double fabsnum;
	unsigned char c,d=0;
	if ( ENG==1 ) { // ENG mode
		fabsnum=fabs(num);
		if ( ( 1e-15 <= fabsnum  ) && ( fabsnum < 1e21 ) ) {
			width-- ; 
			if      ( fabsnum >= 1e18 ) { num/=1e18;  c=0x0B; }	//  Exa
			else if ( fabsnum >= 1e15 ) { num/=1e15;  c=0x0A; }	//  Peta
			else if ( fabsnum >= 1e12 ) { num/=1e12;  c=0x09; }	//  Tera
			else if ( fabsnum >= 1e09 ) { num/=1e09;  c=0x08; }	//  Giga
			else if ( fabsnum >= 1e06 ) { num/=1e06;  c=0x07; }	//  Mega
			else if ( fabsnum >= 1e03 ) { num/=1e03;  c=0x6B; }	//  Kiro
			else if ( fabsnum >= 1    ) {             c=' ';  }
			else if ( fabsnum >= 1e-3 ) { num/=1e-3;  c=0x6d; }	//  milli
			else if ( fabsnum >= 1e-6 ) { num/=1e-6;  c=0xE6; d=0x4B; }	//  micro
			else if ( fabsnum >= 1e-9 ) { num/=1e-9;  c=0x03; }	//  nano
			else if ( fabsnum >= 1e-12) { num/=1e-12; c=0x70; }	//  pico
			else if ( fabsnum >= 1e-15) { num/=1e-15; c=0x66; }	//  femto
			sprintGRS( buffer, num, width, align_mode, round_mode, round_digit);
			width=strlen((char*)buffer);
			buffer[width++]=c;
			buffer[width++]=d;
			buffer[width]='\0';
			return ;
		}
	}
	sprintGRS( buffer, num, width, align_mode, round_mode, round_digit);
}

void sprintG( char* buffer, double num, int width, int align_mode) {
	sprintGRS(buffer, num, width, align_mode, Norm, 15); // + round
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void Fkey_KDISPN(int n,char *buffer) {
	Fkey_Clear(n);
	CB_PrintXY(n*21+3,7*8+1,(unsigned char *)buffer,0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+2,7*8+7);
	Bdisp_ClearLineVRAM(n*21+3,7*8+1,n*21+3,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+1,n*21+20,7*8+7);
}

void Fkey_dispN_Aa(int n, char *buffer) {
	Fkey_dispN(n,buffer);
	Bdisp_AreaReverseVRAM( n*21+3, 7*8+1, n*21+7, 7*8+7);	// reverse
}
void Fkey_dispN_aA(int n, char *buffer) {
	Fkey_dispN(n,buffer);
	Bdisp_AreaReverseVRAM( n*21+15, 7*8+1, n*21+20, 7*8+7);	// reverse
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
const short CharMATH[]= {
	0x002B,0x002D,0x00A9,0x00B9,0x005E,0x002A,0x002F,0x003D,0x0011,0x003C,0x003E,0x0010,0x0012,0x0087,0xE5BE,0xE5BF,0xE6B0,0xE6B1,0xE5A3,
	0xE5A4,0x7F50,0xE5B0,0x000F,0xE64F,0x7F53,0xE542,0x0086,0xE551,0xE54F,0xE6BB,0xE6B7,0xE6B8,0xE6B9,0xE5C0,0xE5C1,0xE5C2,0xE5C3,0xE5C4,
	0xE5C5,0xE5C6,0xE5C7,0xE5C8,0xE5C9,0xE5CA,0xE5CB,0xE5CC,0xE5CD,0xE5CE,0xE5CF,0xE5D0,0xE5D1,0xE5D2,0xE5D3,0xE5D4,0xE5D5,0xE5D6,0xE5D7,
	0xE5D8,0xE5D9,0xE5DA,0xE5DB,0xE5DC,0xE5DD,0xE5DE,0xE5DF,0x00C2,0x00C3,0x00CB,0x00CC,0x7FC7,0x7F54,0x008C,0x009C,0x00AC,0x00BC,0xE6BD,
	0xE6BE,0xE6BF,0xE6C0,0xE6C1,0xE6C2,0xE6C3,0xE6C4,0xE6C5,0xE6C6,0xE6C7,0xE6C8,0xE6C9,0xE6CA,0xE6CB,0xE6D6,0xE6CC,0xE6CD,0xE6CE,0xE6CF,
	0xE6D0,0xE6D1,0xE6D2,0xE6D3,0xE6D4,0xE6D5,0xE6B2,0xE6B3,0xE6B4,0xE6B5,0xE6BC,0xE6B6,0xE6D7,0xE6D8,0xE6D9,0xE6DA,0xE6DB,0xE6DC,0xE6DD,
	0xE6DE,0x0000 };
	
const short CharSYBL[]= {
	0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002C,0x002E,0x003A,0x003B,0x003F,0x0040,0x005B,0x005C,0x005D,0x005F,
	0x0060,0x007B,0x007C,0x007D,0x007E,0x0013,0x00B5,0x00BB,0xE594,0xE595,0xE596,0xE597,0xE598,0xE590,0xE591,0xE592,0xE593,0xE599,0xE59A,
	0xE59B,0xE59C,0xE59D,0xE59E,0xE5A1,0xE59F,0xE5A2,0xE5A0,0xE5A5,0xE5A6,0xE5A7,0xE5B5,0xE5B6,0xE5B8,0xE5B9,0xE5BA,0xE5BB,0xE5BC,0xE690,
	0xE691,0xE692,0xE693,0xE694,0xE695,0xE696,0xE697,0xE698,0xE699,0xE69A,0xE69B,0xE69C,0xE69D,0xE69E,0xE69F,0xE6A0,0xE6A1,0xE6A2,0xE6A3,
	0xE6A4,0xE6A5,0xE6A6,0xE6A7,0xE6A8,0xE6A9,0xE6AA,0x0000 };

const short CharABT[]= {
	0xE540,0xE541,0xE542,0xE543,0xE544,0xE545,0xE546,0xE547,0xE548,0xE549,0xE54A,0xE54B,0xE54C,0xE54D,0xE54E,0xE54F,0xE550,0xE551,0xE553,
	0xE554,0xE555,0xE556,0xE557,0xE558,0xE501,0xE502,0xE503,0xE504,0xE505,0xE506,0xE507,0xE508,0xE509,0xE50A,0xE50B,0xE50C,0xE50D,0xE50E,
	0xE50F,0xE510,0xE511,0xE512,0xE513,0xE514,0xE515,0xE516,0xE517,0xE518,0xE519,0xE51A,0xE51B,0xE51C,0xE51D,0xE51E,0xE520,0xE521,0xE522,
	0xE523,0xE524,0xE525,0xE526,0xE527,0xE528,0xE529,0xE52A,0xE52B,0xE52C,0xE52D,0xE52E,0xE52F,0xE530,0xE531,0xE532,0xE533,0xE534,0xE535,
	0xE560,0xE561,0xE562,0xE563,0xE564,0xE565,0xE566,0xE567,0xE568,0xE569,0xE56A,0xE56B,0xE56C,0xE56D,0xE56E,0xE56F,0xE570,0xE571,0xE572,
	0xE573,0xE574,0xE575,0xE576,0xE577,0xE578,0xE579,0xE57A,0xE57B,0xE57C,0xE57D,0xE57E,0xE580,0xE581,0xE582,0x0000 };

const short Charabr[]= {
	0xE640,0xE641,0xE642,0xE643,0xE644,0xE645,0xE646,0xE647,0xE648,0xE649,0xE64A,0xE64B,0xE64C,0xE64D,0xE64E,0xE64F,0xE650,0xE651,0xE652,
	0xE653,0xE654,0xE655,0xE656,0xE657,0xE658,0xE601,0xE602,0xE603,0xE604,0xE605,0xE606,0xE607,0xE608,0xE609,0xE60A,0xE60B,0xE60C,0xE60D,
	0xE60E,0xE60F,0xE610,0xE611,0xE612,0xE613,0xE614,0xE615,0xE616,0xE617,0xE618,0xE619,0xE61A,0xE61B,0xE61C,0xE61D,0xE61E,0xE61F,0xE620,
	0xE621,0xE622,0xE623,0xE624,0xE625,0xE626,0xE627,0xE628,0xE629,0xE62A,0xE62B,0xE62C,0xE62D,0xE62E,0xE62F,0xE630,0xE631,0xE632,0xE633,
	0xE634,0xE635,0xE660,0xE661,0xE662,0xE663,0xE664,0xE665,0xE666,0xE667,0xE668,0xE669,0xE66A,0xE66B,0xE66C,0xE66D,0xE66E,0xE66F,0xE670,
	0xE671,0xE672,0xE673,0xE674,0xE675,0xE676,0xE677,0xE678,0xE679,0xE67A,0xE67B,0xE67C,0xE67D,0xE67E,0xE680,0xE681,0xE682,0x0000 };

const short CharABC[]= {
	0xE741,0xE742,0xE743,0xE744,0xE745,0xE746,0xE747,0xE748,0xE749,0xE74A,0xE74B,0xE74C,0xE74D,0xE74E,0xE74F,0xE750,0xE751,0xE752,0xE753,
	0xE754,0xE755,0xE756,0xE757,0xE758,0xE759,0xE75A,0xE761,0xE762,0xE763,0xE764,0xE765,0xE766,0xE767,0xE768,0xE769,0xE76A,0xE76B,0xE76C,
	0xE76D,0xE76E,0xE76F,0xE770,0xE771,0xE772,0xE773,0xE774,0xE775,0xE776,0xE777,0xE778,0xE779,0xE77A,0x0000 };

const short CharKANA[]= {
	0x0020,0xFFA1,0xFFA2,0xFFA3,0xFFA4,0xFFA5,0xFFA6,0xFFA7,0xFFA8,0xFFA9,0xFFAA,0xFFAB,0xFFAC,0xFFAD,0xFFAE,0xFFAF,0xFFB0,0xFFB1,0xFFB2,
	0xFFB3,0xFFB4,0xFFB5,0xFFB6,0xFFB7,0xFFB8,0xFFB9,0xFFBA,0xFFBB,0xFFBC,0xFFBD,0xFFBE,0xFFBF,0xFFC0,0xFFC1,0xFFC2,0xFFC3,0xFFC4,0xFFC5,
	0xFFC6,0xFFC7,0xFFC8,0xFFC9,0xFFCA,0xFFCB,0xFFCC,0xFFCD,0xFFCE,0xFFCF,0xFFD0,0xFFD1,0xFFD2,0xFFD3,0xFFD4,0xFFD5,0xFFD6,0xFFD7,0xFFD8,
	0xFFD9,0xFFDA,0xFFDB,0xFFDC,0xFFDD,0xFFDE,0xFFDF,0x0000 };
	

short *oplist=CharMATH;
int CharPtr=0;

unsigned int SelectChar( int *ContinuousSelect ) {

	int cont=1, x,y,cx,cy,ptr,scrl=0;
	unsigned int key;
	char tmpbuf[18];
	short opcode;
	int opNum=0, n ;
	int mini=0;

	SaveDisp(SAVEDISP_PAGE1);		// --------
	Cursor_SetFlashMode(0); 		// cursor flashing off

	while (cont) {
		opNum=0;
		while ( oplist[opNum++] ) ;
		opNum-=2;
		if ( CharPtr > opNum ) CharPtr=opNum;
		if ( scrl == 0 ) if ( CharPtr>=114 ) scrl=1;
		if ( scrl == 1 ) if ( CharPtr<  19 ) scrl=0;
		Bdisp_AllClr_VRAM();
		switch ( mini ) {
			case 2:
				if ( *ContinuousSelect ) {
					PrintMini( 1,1, (unsigned char*)"===Continuous Select= C.Basic =", MINI_REV );
				} else {
					PrintMini( 1,1, (unsigned char*)"===Character Select== C.Basic =", MINI_OVER );
				}
				break;
			case 1:
				if ( *ContinuousSelect ) {
					PrintMini( 1,1, (unsigned char*)"===Continuous Select= Default =", MINI_REV );
				} else {
					PrintMini( 1,1, (unsigned char*)"===Character Select== Default =", MINI_OVER );
				}
				break;
			default:
				if ( *ContinuousSelect ) {
					locate(1,1); PrintRev((unsigned char*)"==Continuous Select==");
				} else {
					locate(1,1);    Print((unsigned char*)"==Character Select===");
				}
		}
		x=2; y=2;
		opcode=1;
		while ( opcode ) {
			ptr=(y-2+scrl)*19+x-2 ;
			opcode=oplist[ ptr ];
			CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		switch ( mini ) {
				case 2:
					if ( CharPtr == ptr )	CB_PrintMini( (x-1)*6+1, (y-1)*8+1, (unsigned char*)tmpbuf , MINI_REV );
					else 					CB_PrintMini( (x-1)*6+1, (y-1)*8+1, (unsigned char*)tmpbuf , MINI_OVER );
					break;
				case 1:
					if ( CharPtr == ptr )	PrintMini( (x-1)*6+1, (y-1)*8+1, (unsigned char*)tmpbuf , MINI_REV );
					else 					PrintMini( (x-1)*6+1, (y-1)*8+1, (unsigned char*)tmpbuf , MINI_OVER );
					break;
				default:
					if ( CharPtr == ptr )	CB_PrintRevC( x, y, (unsigned char*)tmpbuf );
					else 					CB_PrintC( x, y, (unsigned char*)tmpbuf );
					break;
			}
			x++;
			if (x>20) { x=2; y++; if ( y>7 ) break ; }
//			Bdisp_PutDisp_DD();
		}
		if ( opNum>=114 ) {
			if ( scrl ) {
				locate(21,2); Print((unsigned char*)"\xE6\x92"); // 
			} else {
				locate(21,7); Print((unsigned char*)"\xE6\x93"); // 
			}
		}
		Fkey_dispN( 0, "MATH");
		Fkey_dispN( 1, "SYBL");
		Fkey_dispN( 2, "AB\xCD");
		Fkey_DISPN( 3, "\xE6\x40\xE6\x41\xE6\x42");
		Fkey_dispN( 4, "ABC");
		Fkey_KDISPN(5, "\xFF\xA7\xFF\xA8\xFF\xA9");
		
		if ( *ContinuousSelect ) {
			if ( oplist == CharMATH ) n=0;
			if ( oplist == CharSYBL ) n=1;
			if ( oplist == CharABT  ) n=2;
			if ( oplist == Charabr  ) n=3;
			if ( oplist == CharABC  ) n=4;
			if ( oplist == CharKANA ) n=5;
			Bdisp_AreaReverseVRAM( n*21+3, 7*8+1, n*21+20, 7*8+7);	// reverse Fkey
		}
		
		cx=(CharPtr%19)+2;
		cy=(CharPtr/19)+2-scrl;
		locate(cx,cy);

		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
				RestoreDisp(SAVEDISP_PAGE1);	// --------
				*ContinuousSelect=0;
				return 0;	// input cancel
				break;
			case KEY_CTRL_EXIT:
			case KEY_CTRL_QUIT:
				RestoreDisp(SAVEDISP_PAGE1);	// --------
				*ContinuousSelect=0;
				return 0;	// input cancel
				break;
			case KEY_CTRL_EXE:
			case KEY_CHAR_CR:
				cont=0;
				break;
			case KEY_CTRL_F1:	// CharMATH
				if (oplist==CharMATH) *ContinuousSelect=1-*ContinuousSelect;
				oplist=CharMATH;
				break;
			case KEY_CTRL_F2:	// CharSYBL
				if (oplist==CharSYBL) *ContinuousSelect=1-*ContinuousSelect;
				oplist=CharSYBL;
				break;
			case KEY_CTRL_F3:	// CharABT
				if (oplist==CharABT) *ContinuousSelect=1-*ContinuousSelect;
				oplist=CharABT;
				break;
			case KEY_CTRL_F4:	// Charabr
				if (oplist==Charabr) *ContinuousSelect=1-*ContinuousSelect;
				oplist=Charabr;
				break;
			case KEY_CTRL_F5:	// CharABC
				if (oplist==CharABC) *ContinuousSelect=1-*ContinuousSelect;
				oplist=CharABC;
				break;
			case KEY_CTRL_F6:	// CharKANA
				if (oplist==CharKANA) *ContinuousSelect=1-*ContinuousSelect;
				oplist=CharKANA;
				break;
			case KEY_CTRL_LEFT:
				CharPtr--; if ( CharPtr < 0 ) CharPtr=opNum;
				break;
			case KEY_CTRL_RIGHT:
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
			case KEY_CTRL_SHIFT:
				*ContinuousSelect=1-*ContinuousSelect;
				break;
			case KEY_CTRL_OPTN:
				switch ( mini ) {
					case 0:
					case 1:
						mini=2;
						mini=2;
						break;
					case 2:
						mini=0;
				}
				break;
			case KEY_CTRL_VARS:
				switch ( mini ) {
					case 0:
					case 2:
						mini=1;
						mini=1;
						break;
					case 1:
						mini=0;
				}
				break;
			default:
				break;
		}
	}
	
	RestoreDisp(SAVEDISP_PAGE1);	// --------
	return ( (unsigned int)oplist[ CharPtr ] );
}

//----------------------------------------------------------------------------------------------
void DMS_Opcode( char * buffer, short code ) {
	if ( code == 0x9C ) { strcat( buffer,"(deg)"); }
	if ( code == 0xAC ) { strcat( buffer,"(rad)"); }
	if ( code == 0xBC ) { strcat( buffer,"(gra)"); }
}

int SelectOpcode( int listselect, int flag ) {
	int *select;
	short *oplist;
	int opNum;
	char buffer[22];
	char tmpbuf[18];
	unsigned int key;
	int	cont,cont2=1;
	int i,j,y;
	int seltop;

	while ( cont2 ) {

		switch ( listselect ){
			case CMDLIST_OPTN:
				oplist = oplistOPTN;
				select =&selectOPTN;
				break;
			case CMDLIST_VARS:
				oplist = oplistVARS;
				select =&selectVARS;
				break;
			case CMDLIST_PRGM:
				oplist = oplistPRGM;
				select =&selectPRGM;
				break;
		}
		
		opNum=0 ;
		while ( oplist[opNum++] ) ;
		opNum-=2;
		seltop=*select;
		cont=1;

		Cursor_SetFlashMode(0); 		// cursor flashing off

		SaveDisp(SAVEDISP_PAGE1);
		PopUpWin(6);

		while (cont) {
			if (  (*select)<seltop ) seltop=(*select);
			if ( ((*select)-seltop) > 5 ) seltop=(*select)-5;
			if ( (opNum-seltop) < 5 ) seltop = opNum-5; 
			for ( i=0; i<6; i++ ) {
				CB_Print(3,2+i,(unsigned char *)"                 ");
				j=oplist[seltop+i];
				if ( j == 0xFFFFFFFF ) {
					CB_Print(3,2+i,(unsigned char *)"-----------------");
				} else {
					CB_OpcodeToStr( j, tmpbuf ) ; // SYSCALL
					tmpbuf[12]='\0'; 
					DMS_Opcode( tmpbuf, j);
					j=0; if ( tmpbuf[0]==' ' ) j++;
//	//				sprintf(buffer,"%04X:%-12s",(unsigned short)oplist[seltop+i],tmpbuf+j ) ;
					sprintf(buffer,"%-17s",tmpbuf+j ) ;
					CB_Print(3,2+i,(unsigned char *)buffer);
				}
			}
			Bdisp_PutDisp_DD();	
			
			y = ((*select)-seltop) + 1 ;
			Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse *select line 
			Bdisp_PutDisp_DD();

			GetKey( &key );
			switch (key) {
				case KEY_CTRL_EXIT:
				case KEY_CTRL_QUIT:
					RestoreDisp(SAVEDISP_PAGE1);
					if ( flag ) return 0x10000;
					return 0;
					
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
					GetKey(&key);
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
					if ( flag ) return 0;
					key=SelectOpcode5800P( 1 );
					if ( key > 0x10000 ) { 
						listselect=key-0x10000; key=0;
						cont=0;
					}
					RestoreDisp(SAVEDISP_PAGE1);
					if ( key ) return key & 0xFFFF;
				break;
				default:
					break;
			}
		}
		RestoreDisp(SAVEDISP_PAGE1);
	}
	Bdisp_PutDisp_DD();
	
	return oplist[(*select)];
}

//--------------------------------------------------------------------------

const short oplistOPTN[]={
		0x97,	// Abs
		0xA6,	// Int
		0xDE,	// Intg
		0xB6,	// frac
		0xAB,	// !
		0x7F3A,	// MOD(
		
		0xFFFF,	// 				-
		0x7FB0,	// And
		0x7FB1,	// Or
		0x7FB3,	// Not
		0x7FB4,	// Xor
		0xC1,	// Ran#
		0x7F87,	// RanInt#(		
		0x7F88,	// RanList#(		
		
		0xFFFF,	// 				-
		0xBA,	// and
		0xAA,	// or
		0xA7,	// not
		0x9A,	// xor
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
		0x7F41,	// Trn 
		0x7F47,	// Fill(
		0x7F58,	// ElemSize(
		0x7F59,	// ColSize(
		0x7F5A,	// RowSize(
		0x7F5B,	// MatBase(
		0x7FE9,	// CellSum(

		0xFFFF,	// 				-
		0x7F2C,	// Seq(
		0x7F49,	// Argument(
		0x7F4A,	// List->Mat(
		0x7F4B,	// Mat->List(
		0x7F20,	// Max(
		0x7F2D,	// Min(
		0x7F2E,	// Mean(
		0x7F4C,	// Sum
		0x7F4D,	// Prod
		0x7FB0,	// SortA(
		0x7FB1,	// SortB(
		0x7F29,	// Sigma(

		0xFFFF,	// 				-
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh

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

		0xFFFF,	// 				-
		0x23,	// #
		0x24,	// $
		0x25,	// %
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
		0x3C,	// <
		0x3E,	// >
		0x10,	// <=
		0x11,	// !=
		0x12,	// >=
		
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

		0xFFFF,	// 				-
		0xF718,	// ClrText
		0xF719,	// ClrGraph
//		0xF71A,	// ClrList
		0xF91E,	// ClrMat
		0xF710,	// Locate
		0x7F8F,	// Getkey
		0xF7E4,	// Disp
		0xF94F,	// Wait 
		
		0xFFFF,	// 				-
		0x7F9F,	// KeyRow(
		0xF90F,	// AliasVar
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
		0xF93A,	// StrDwr(
		0xF93B,	// StrInv(
		0xF93C,	// StrShift(
		0xF93D,	// StrRotate(
		0xF943,	// Sprintf(
		0xF940,	// Str(
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
		0xF7C3,	// CoordOn
		0xF7D3,	// CoordOff
		0xF77D,	// GridOn
		0xF77A,	// GridOff
		0xF7C2,	// AxesOn
		0xF7D2,	// AxesOff
		0xF7C4,	// LabelOn
		0xF7D4,	// LabelOff
		0xF770,	// G-Connect
		0xF771,	// G-Plot

		0xFFFF,	// 				-
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		
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
//		0xF79F,	// RclCapt
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
		0xF9D5,	// _Bmp 
		0xF9D6,	// _Bmp8
		0xF9D7,	// _Bmp16
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
//		0x3D,	// =			1
//		0x11,	// !=			2
//		0x3E,	// >			3
//		0x3C,	// <			4
//		0x12,	// >=			5
//		0x10,	// <=			6
		0x7FB0,	// And			6
		0xBA,	// and
		0x7FB1,	// Or			7
		0xAA,	// or
		0x7FB3,	// Not			8
		0xA7,	// not
		0x7FB4,	// Xor			9
		0x9A,	// xor
		0x26,	// &
		0x7C,	// |
		0x23,	// #
		0x25,	// %
		
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
		0x23,	// #
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
		0x7F49,	// Argument(
		0x7F2C,	// Seq(
		0x7F4B,	// Mat->List(
		0xF7B0,	// SortA(
		0xF7B1,	// SortB(
		0x7F20,	// Max(
		0x7F2D,	// Min(
		0x7F2E,	// Mean(
		0x7F88,	// RanList#(
		0x23,	// #
		0x25,	// %

//											6
		0xF91E,	// ClrMat	
		0xF71A,	// ClrList	
		0x7F4A,	// List->Mat(
		0x7F4B,	// Mat->List(
		0x7F41,	// Trn 
		0x7F47,	// Fill(
		0x7F4C,	// Sum
		0x7F4D,	// Prod
		0x7F29,	// Sigma( 
		0x7E,	// ~
		0x23,	// #
		0x25,	// %

//											7	GR
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
//											8
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
		0x23,	// #
		0x25,	// %

//											9
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

//											10
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

//											11
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
		
//											12
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

//											13	FN
		0x97,	// Abs
		0xA6,	// Int
		0xB6,	// frac
		0xDE,	// Intg
		0x7F3A,	// MOD(
		0xAB,	// !
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh

//											14
		0xC1,	// Ran#
		0x7F87,	// RanInt#(
		0xD3,	// Rnd
		0x7F86,	// RndFix(
		0xD9,	// Norm
		0xE3,	// Fix
		0xE4,	// Sci
		0xDD,	// Eng
		0xF90B,	// EngOn
		0xF90C,	// EngOff
		0x23,	// #
		0x25,	// %

//											15
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

//											16
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

//											17	STR
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
		
//											18
		0xF937,	// Exp>Str(
		0xF938,	// Exp(
		0xF939,	// StrUpr(
		0xF93A,	// StrDwr(
		0xF93B,	// StrInv(
		0xF93C,	// StrShift(
		0xF93D,	// StrRotate(
		0xF943,	// Sprintf(
		0xF940,	// Str(
		0x24,	// $
		0x23,	// #
		0x25,	// %	

//											19	EX
		0xF90F,	// AliasVar
		0x7F5F,	// Ticks
		0xF941,	// DATE
		0xF942,	// TIME
		0x7FF5,	// IsExist(
		0xF7DF,	// Delete 
		0xF7EE,	// Save
		0xF7EF,	// Load(
		0xF7FB,	// Screen
		0xF7FC,	// PutDispDD
		0x23,	// #
		0x25,	// %
		
		0xF7E1,	// Rect(
		0xF7E2,	// FillRect(
		0xF7E8,	// ReadGraph(
		0xF7E9,	// WriteGraph(
		0xF73E,	// DotGet(
		0xF73B,	// DotPut(
		0xF73D,	// DotTrim(
		0xF7E0,	// DotLife(
		0x7FE9,	// CellSum(
		0xF94F,	// Wait 
		0x23,	// #
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
		
		0xF7FD, // FKey(
		0xF7F2,	// PopUpWin(
		0xF7F8,	// RefreshCtrl
		0xF7FA,	// RefreshTime
		0x7F9F,	// KeyRow(
		0xF7FE,	// BackLight
		0x7F58,	// ElemSize(
		0x7F59,	// ColSize(
		0x7F5A,	// RowSize(
		0x7F5B,	// MatBase(
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
		0xF9D5,	// _Bmp 
		0xF9D6,	// _Bmp8
		0xF9D7,	// _Bmp16
		0x25,	// %
		
		
//		0xF797,	// StoV-Win
//		0xF798,	// RclV-Win
//		0xF79F,	// RclCapt

		0};

void FkeyRel(){
	Fkey_DISPR( 0, " = ");
	Fkey_DISPR( 1, " \x11 ");
	Fkey_DISPR( 2, " > ");
	Fkey_DISPR( 3, " < ");
	Fkey_DISPR( 4, " \x12 ");
	Fkey_DISPR( 5, " \x10 ");
}

#define CMD_STD  0
#define CMD_GR   7
#define CMD_FN  13
#define CMD_STR 17
#define CMD_EX  19

int SelectOpcode5800P( int flag ) {
	int *select=&selectCMD;
	short *oplist=oplistCMD;
	int opNum=0 ;
	char buffer[22];
	char tmpbuf[18];
	unsigned int key;
	int	cont=1;
	int i,j,y,n;
	int seltop;
	int shift=0;

	while ( oplist[opNum++] ) ;
	opNum-=2;
	seltop=*select;

	Cursor_SetFlashMode(0); 		// cursor flashing off

	SaveDisp(SAVEDISP_PAGE1);
//	Bdisp_AllClr_DDVRAM();
	
//	PopUpWin(6);

	while (cont) {
		Bdisp_AllClr_VRAM();
		(*select)=(*select)/12*12;
//		locate(1,1); Print((unsigned char *)"== Command Select ==");
		locate(1,1); Print((unsigned char *)"==STD GR FN STR EX==");
		if ( (*select) < CMD_GR*12 )       { locate( 3,1); PrintRev((unsigned char *)"STD"); }
		else if ( (*select) < CMD_FN *12 ) { locate( 7,1); PrintRev((unsigned char *)"GR"); }
		else if ( (*select) < CMD_STR*12 ) { locate(10,1); PrintRev((unsigned char *)"FN"); }
		else if ( (*select) < CMD_EX *12 ) { locate(13,1); PrintRev((unsigned char *)"STR"); }
		else                               { locate(17,1); PrintRev((unsigned char *)"EX"); }
		
		for ( i=0; i<12; i++ ) {
			n=oplist[(*select)+i];
			tmpbuf[0]='\0'; 
			if ( n == 0xFFFF ) n=' ';
			else CB_OpcodeToStr( n, tmpbuf ) ; // SYSCALL
			tmpbuf[8]='\0'; 
			DMS_Opcode( tmpbuf, n);
			n=i+1; if (n>9) n=0;
			j=0; if ( tmpbuf[0]==' ' ) j++;
			if ( i< 10 ) sprintf(buffer,"%d:%-9s",n,tmpbuf+j ) ;
			if ( i==10 ) sprintf(buffer,".:%-9s",tmpbuf+j ) ;
			if ( i==11 ) sprintf(buffer,"\x0F:%-9s",tmpbuf+j ) ;
			locate(0+(i%2)*12,2+i/2); 
			Print((unsigned char *)buffer);
			if ( i==11 ) { locate(0+(i%2)*12,2+i/2); Print((unsigned char *)"\x0F"); }
		}
		if ( shift ) {
			Fkey_DISPR( 0, " = ");
			Fkey_DISPR( 1, " \x11 ");
			Fkey_DISPR( 2, " > ");
			Fkey_DISPR( 3, " < ");
			Fkey_DISPR( 4, " \x12 ");
			Fkey_DISPR( 5, " \x10 ");
		} else {
			Fkey_DISPR( 0, " ? ");
			Fkey_DISPR( 1, " \x0C ");
			Fkey_DISPR( 2, " : ");
			Fkey_DISPR( 3, " \x13 ");
			Fkey_DISPR( 4, " ' ");
			Fkey_DISPR( 5, " / ");
		}
		Bdisp_PutDisp_DD();	
		
		y = ((*select)-seltop) + 1 ;
//		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse *select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
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
				if ( flag ) return 0x10000+CMDLIST_OPTN;
				key=SelectOpcode( CMDLIST_OPTN, 1);
				if ( key ) return key & 0xFFFF;
				break;
			case KEY_CTRL_VARS:
			case KEY_CTRL_PRGM:
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
	return i;
}


//----------------------------------------------------------------------------------------------

int GetOpcodeLen( char *SRC, int ptr, int *opcode ){
	*opcode=SRC[ptr];
	switch ( *opcode ) {
		case 0x00:		// <EOF>
			return 0 ;
			break;
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			*opcode= ((unsigned char)(*opcode)<<8)+(unsigned char)SRC[ptr+1] ;
			return 2;
			break;
	}
	return 1 ;
}
int OpcodeLen( int opcode ){
	switch ( (opcode&0xFF00) >> 8 ) {
		case 0x7F:		// 
		case 0xF7:		// 
		case 0xF9:		// 
		case 0xE5:		// 
		case 0xE6:		// 
		case 0xE7:		// 
		case 0xFF:	// 
			return 2 ;
			break;
	}
	return 1 ;
}
int GetOpcode( char *SRC, int ptr ){
	int c=SRC[ptr];
	switch ( c ) {
		case 0x00:		// <EOF>
			return 0 ;
			break;
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			return ((unsigned char)c<<8)+(unsigned char)SRC[ptr+1];
			break;
	}
	return (unsigned char)c ;
}

int OpcodeStrlen(int c) {
	char tmpbuf[18];
	CB_OpcodeToStr( c & 0xFFFF, tmpbuf ) ;		// SYSCALL+
	return CB_MB_ElementCount( tmpbuf ) ;		// SYSCALL
}

int OpcodeStrLenBuf(char *SRC, int offset) {
		int opcode;
		int c,d;
		c=SRC[offset];
		d=SRC[offset+1];
		switch ( c ) {
				case 0x7F:		// 
				case 0xFFFFFFF7:		// 
				case 0xFFFFFFF9:		// 
				case 0xFFFFFFE5:		// 
				case 0xFFFFFFE6:		// 
				case 0xFFFFFFE7:		// 
				case 0xFFFFFFFF:	// 
						opcode= ((unsigned char)c<<8)+(unsigned char)d;
						break;
				default:
						opcode= (unsigned char)c;
						break;
		}
		return OpcodeStrlen( opcode ) ;
}


int NextOpcode( char *SRC, int *offset ){
	switch ( SRC[(*offset)++] ) {
		case 0x00:		// <EOF>
			(*offset)--;
			return 0 ;
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
			break;
	}
	return 1 ;
}

int NextLine( char *SRC, int *offset ){
	int c;
	int ofst=*offset,ofst2;
	int n;
	while (1) {
		 if ( NextOpcode( SRC, &(*offset) ) ==0 ) return (*offset)-ofst ;
		ofst2=*offset;
		PrevOpcode( SRC, &ofst2 );
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

int PrevOpcode( char *SRC, int *offset ){
	int c;
	(*offset)-=2; 
	if ( *offset < 0 ) { (*offset)=0; return 0;}
		c=SRC[*offset];
		switch ( c ) {
			case 0x7F:		// 
			case 0xFFFFFFF7:		// 
			case 0xFFFFFFF9:		// 
			case 0xFFFFFFE5:		// 
			case 0xFFFFFFE6:		// 
			case 0xFFFFFFE7:		// 
			case 0xFFFFFFFF:	// 
				return 2 ;
		}
	(*offset)++;
	return 1 ;
}

int PrevLine( char *SRC, int *offset ){
	int c,d;
	int ofst=*offset,ofst2;
	while (1) {
		PrevOpcode( SRC, &(*offset) );
		if ( *offset <= 0 ) { *offset=0; return ofst-(*offset) ; }
		c=SRC[*offset];
		ofst2=*offset;
		PrevOpcode( SRC, &ofst2 );
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

//---------------------------------------------------------------------------------------------

typedef struct {
	short code;
	char str[14];
} topcodes;

const topcodes OpCodeStrList[] = {
	{ 0x7F3A, "MOD(" }, 		// SDK emu not support
	{ 0x7F87, "RanInt#(" }, 	// SDK emu not support
	{ 0x7F88, "RanList#(" }, 	// SDK emu not support
	{ 0x7F58, "ElemSize(" }, 
	{ 0x7F59, "ColSize(" }, 
	{ 0x7F5A, "RowSize(" }, 
	{ 0x7F5B, "MatBase(" }, 
	{ 0x7F5F, "Ticks" }, 
	{ 0x7F9F, "KeyRow(" }, 
	{ 0x7FB4, " Xor " }, 		// SDK emu not support
	{ 0x7FF5, "IsExist(" }, 
	{ 0x7FF6, "Peek(" }, 
	{ 0x7FF8, "VarPtr(" }, 
	{ 0x7FFA, "ProgPtr(" }, 
	{ 0xF70C, "Return " }, 		// add space
	{ 0xF717, "ACBreak" }, 
	{ 0xF73B, "DotPut(" }, 
	{ 0xF73D, "DotTrim(" }, 
	{ 0xF73E, "DotGet(" }, 
	{ 0xF79E, "Menu " },		// SDK emu not support
	{ 0xF7DF, "Delete " }, 
	{ 0xF7E0, "DotLife(" }, 
	{ 0xF7E1, "Rect " }, 
	{ 0xF7E2, "FillRect " }, 
	{ 0xF7E3, "LocateYX " }, 
	{ 0xF7E4, "Disp " }, 
	{ 0xF7E8, "ReadGraph(" }, 
	{ 0xF7E9, "WriteGraph " }, 
	{ 0xF70F, "ElseIf " }, 
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
	{ 0xF90F, "AliasVar ",}, 
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
	{ 0xF940, "Str(" }, 
	{ 0xF941, "DATE" }, 
	{ 0xF942, "TIME" }, 
	{ 0xF943, "Sprintf(" }, 
	{ 0xF94F, "Wait " }, 
	{ 0xF9C0, "_ClrVram" },
	{ 0xF9C1, "_ClrScreen" },
	{ 0xF9C2, "_DispVram" },
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
	{ 0x00FA, "Gosub "},
	{ 0x00A7, "not "}, 			// small
	{ 0x009A, " xor "}, 		// add space
	{ 0x00AA, " or "}, 			// add space
	{ 0x00BA, " and "}, 		// add space
	{ 0, "" }
};

int CB_OpcodeToStr( int opcode, char *string  ) {
	int i;
	int code;
	opcode &= 0xFFFF;
	i=0;
	do {
		code = OpCodeStrList[i].code & 0xFFFF ;
		if ( code == opcode ) { strcpy( string, OpCodeStrList[i].str ); return ; }
		i++;
	} while ( code ) ;
	if ( opcode >= 0xFF00 ) {	// kana
		string[0]=0xFF;
		string[1]=opcode&0xFF;
		string[2]='\0';
	} else
		return OpcodeToStr( (short)opcode, (unsigned char *)string ) ; // SYSCALL
}

int CB_MB_ElementCount( char *str ) {
	int ptr=0,len=0;
	int c;
	while ( 1 ) {
		c=str[ptr++];
		if (c==0x00) break;
		else 
		if ( (c==0x7F)||(c==0xFFFFFFF7)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) ) {
			ptr++; }
		len++;
	}
	return len;
}
int CB_MB_strlen( char *str ) {
	int ptr=0,len=0;
	int c;
	while ( 1 ) {
		c=str[ptr];
		if (c==0x00) break;
		else 
		if ( (c==0x7F)||(c==0xFFFFFFF7)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) ) {
			ptr++;
		}
		ptr++;
	}
	return ptr;
}

//----------------------------------------------------------------------------------------------

int strlenOp( char *buffer ) {
	int	i=0;
	while ( 1 ) {
		if ( NextOpcode( buffer, &i ) == 0x00 ) break;
	}
	return i;
}

int InsertOpcode1( char *buffer, int Maxstrlen, int ptr, int opcode ){
	int len,i,j;
	int opH,opL;
	len=OpcodeLen( opcode );
	if ( ( len + strlenOp( buffer ) ) > Maxstrlen ) return -1 ;		// buffer over
	j=ptr+len;
	for ( i=Maxstrlen; i>=j; i-- )
		 buffer[i]=buffer[i-len];
		 
	opH = (opcode&0xFF00) >> 8 ;
	opL = (opcode&0x00FF) ;
	switch (len) {
		case 1:
			buffer[ptr  ]=opL;
			break;
		case 2:
			buffer[ptr ]=opH;
			buffer[ptr+1]=opL;
			break;
	}
	return 0;
}
void DeleteOpcode1( char *buffer, int Maxstrlen, int *ptr){
	int len,i;
	int opH,opL;
	unsigned short opcode;
	opcode=GetOpcode( buffer, *ptr );
	if ( opcode == 0 ) {
		PrevOpcode( buffer, &(*ptr) );
		opcode=GetOpcode( buffer, *ptr );
	}
	len=OpcodeLen( opcode );
	if ( len == 0 ) return ;
	for ( i=(*ptr); i<=Maxstrlen; i++ ) buffer[i]=buffer[i+len];
}

//----------------------------------------------------------------------------------------------
int PrintOpcode(int x, int y, char *buffer, int width, int ofst, int ptrX, int *csrX, int rev_mode, char SPC) {
	char tmpbuf[18],*tmpb;
	char spcbuf[2];
	int i,len,xmax=x+width;
	int opcode=1;
	int  c=1;
	while ( 1 ) {
		if (ofst==ptrX) *csrX=x;
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		if ( x+len > xmax ) break;
		tmpb=tmpbuf;
		while ( i < len ) {
			if ( x < xmax ) {
				if ( rev_mode ) CB_PrintRevC( x,y, (unsigned char*)(tmpb+i) ) ;
					else        CB_PrintC(    x,y, (unsigned char*)(tmpb+i) ) ;
				x++ ;
			}
			c=tmpb[i]&0xFF;
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) tmpb++;
			i++ ;
		}
	}
	spcbuf[0]=SPC;
	while ( x < xmax ) {
				if ( rev_mode ) CB_PrintRevC( x,y, (unsigned char*)spcbuf ) ;
					else        CB_PrintC(    x,y, (unsigned char*)spcbuf ) ;
				x++ ;
	}
	return x;
}

//----------------------------------------------------------------------------------------------

void PutAlphamode1( int CursorStyle ){
	if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
}

int selectCMD=0;
int selectOPTN=0;
int selectVARS=0;
int selectPRGM=0;
char lowercase=0;

int InputStrSub(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel) {
	char buffer2[256];
	char buf[22];
	unsigned int key=0;
	int keyH,keyL;
	int cont=1;
	int i,j,k;
	int csrX=ptrX;	// ptrX:  current buffer ptr      csrX: ccursor X
	int offsetX=0;	// offsetX:  buffer offset for display
	int length;		// buffer length (<MaxStrlen)
	int csrwidth;	// 
	int CursorStyle;
	int key2,multibyte=0;	// 0:mono   non 0:multi
	int dspX;
	int oplen,oplenL,oplenR;
	int selectOPTN=0;
	int alphalock = 0 ;
	int ContinuousSelect=0;

	if ( x + width > 22 ) width=22-x;
	csrwidth=width; if ( x + csrwidth > 20 ) csrwidth=21-x;

	if ( MaxStrlen > 255 ) MaxStrlen = 255;
	for(i=0; i<=MaxStrlen; i++) buffer2[i]=buffer[i]; // backup

	CursorStyle=Cursor_GetFlashStyle();
	if (CursorStyle<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 				Cursor_SetFlashOn(0x6);		// overwrite mode cursor 

	if ( ( float_mode == 0 ) && ( exp_mode == 0 ) && ( alpha_mode ) ) {
		PutKey( KEY_CTRL_SHIFT, 1 );
		PutKey( KEY_CTRL_ALPHA, 1 );
	}
	
	while (cont) {
		Cursor_SetFlashMode(1);			// cursor flashing on
		length=strlenOp( buffer );
		csrX=0;
		while (1) {
			PrintOpcode(x, y, buffer, width, offsetX, ptrX, &csrX, rev_mode , SPC);
			if ( ( 0 < csrX ) && ( csrX < 22 ) )   break;
			if ( offsetX < ptrX )	NextOpcode( buffer, &offsetX);
				else				PrevOpcode( buffer, &offsetX);
		}
		
		if ( ( pallet_mode ) && ( alpha_mode ) ) if ( lowercase ) Fkey_dispN_aA(3,"A<>a"); else Fkey_dispN_Aa(3,"A<>a");
		if ( ( pallet_mode ) && ( alpha_mode ) ) { Fkey_dispR(4,"CHAR"); }


		CursorStyle=Cursor_GetFlashStyle();
		if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
		if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
		if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
		if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
		Cursor_SetPosition(csrX-1,y-1);

//		sprintf(buf,"len=%2d ptr=%2d off=%2d   csr=%2d  ",length,ptrX,offsetX,csrX); PrintMini( 0,7*8+2,(unsigned char *)buf, MINI_OVER);

		if ( ContinuousSelect ) key=KEY_CTRL_F5; else GetKey(&key);
		
		switch (key) {
			case KEY_CTRL_AC:
				if ( length==0 ) cont=0;
				ptrX=0; offsetX=0;
				buffer[0]='\0';
				alphalock = 0 ;
				break;
			case KEY_CTRL_EXIT:
			case KEY_CTRL_QUIT:
			 	if ( exit_cancel == 0 ) {
					cont=0;
					for(i=0; i<=MaxStrlen; i++) buffer[i]=buffer2[i]; // restore
				}
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_DEL:
				if ( length ) {
					if ( CursorStyle < 0x6 ) {		// insert mode
						PrevOpcode( buffer, &ptrX );
						if ( offsetX ) if ( ptrX == offsetX ) PrevOpcode( buffer, &offsetX );
					}
					DeleteOpcode1( buffer, MaxStrlen, &ptrX );
				}
				break;
			case KEY_CTRL_LEFT:
				PrevOpcode( buffer, &ptrX );
				break;
			case KEY_CTRL_RIGHT:
				if ( buffer[ptrX] != 0x00 )	NextOpcode( buffer, &ptrX );
				break;
			case KEY_CTRL_UP:
					ptrX=0;
					offsetX=0;
				break;
			case KEY_CTRL_DOWN:
					ptrX=length;
				break;
			case KEY_CTRL_F1:
				if ( length ) cont=0;
				break;
			case KEY_CTRL_F4:
				if ( ( pallet_mode ) && ( alpha_mode ) ) {
					lowercase=1-lowercase;
					if ( alphalock == 0 ) if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
				}
				break;
			case KEY_CTRL_F5:
				Cursor_SetFlashMode(0); 		// cursor flashing off
				if ( ( pallet_mode ) && ( alpha_mode ) ) {
					key=SelectChar( &ContinuousSelect);
					if ( alphalock == 0 ) if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
				}
				break;
			case KEY_CTRL_F6:	// !=
				key='/';
				break;
			case KEY_CTRL_SHIFT:
				alphalock = 0 ;
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_ALPHA:
						if ( ( pallet_mode ) && ( alpha_mode ) ) alphalock = 1 ;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}

		if ( key == KEY_CTRL_F3 )  {
				key=SelectOpcode5800P( 0 );
				if ( ( pallet_mode ) && ( alpha_mode ) ) if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
		}
		if ( key == KEY_CTRL_OPTN ) {
				key=SelectOpcode( CMDLIST_OPTN, 0 );
				if ( ( pallet_mode ) && ( alpha_mode ) ) if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
		}
		if ( key == KEY_CTRL_VARS ) {
				key=SelectOpcode( CMDLIST_VARS, 0 );
				if ( ( pallet_mode ) && ( alpha_mode ) ) if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
		}
		if ( key == KEY_CTRL_PRGM ) {
				key=SelectOpcode( CMDLIST_PRGM, 0 );
				if ( ( pallet_mode ) && ( alpha_mode ) ) if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
		}
		if ( alpha_mode || exp_mode ) {
			keyH=(key&0xFF00) >>8 ;
			keyL=(key&0x00FF) ;
			switch ( keyH ) {		// ----- 2byte code -----
				case 0x7F:		// 
				case 0xF7:		// 
				case 0xF9:		// 
				case 0xE5:		// 
				case 0xE6:		// 
				case 0xE7:		// 
				case 0xFF:	// 
					if ( CursorStyle < 0x6 ) {		// insert mode
						i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
					} else {					// overwrite mode
						if ( buffer[ptrX] != 0x00 ) DeleteOpcode1( buffer, MaxStrlen, &ptrX);
						i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
					}
					if ( i==0 ) NextOpcode( buffer, &ptrX );
					key=0;
					alphalock = 0 ;
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
			if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
			if ( hex_mode && ( KEY_CTRL_XTT == key ) ) key=KEY_CHAR_A;
			if ( hex_mode && ( KEY_CHAR_LOG == key ) ) key=KEY_CHAR_B;
			if ( hex_mode && ( KEY_CHAR_LN  == key ) ) key=KEY_CHAR_C;
			if ( hex_mode && ( KEY_CHAR_SIN == key ) ) key=KEY_CHAR_D;
			if ( hex_mode && ( KEY_CHAR_COS == key ) ) key=KEY_CHAR_E;
			if ( hex_mode && ( KEY_CHAR_TAN == key ) ) key=KEY_CHAR_F;
//			if ( float_mode && ( key == KEY_CHAR_POW ) )    key='^';
			if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
			if ( ( key == KEY_CTRL_XTT ) )   key='X'; // ^
			if ( CursorStyle < 0x6 ) {		// insert mode
					i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
			} else {					// overwrite mode
				if ( buffer[ptrX] != 0x00 ) DeleteOpcode1( buffer, MaxStrlen, &ptrX);
				i=InsertOpcode1( buffer, MaxStrlen, ptrX, key );
			}
			if ( i==0 ) NextOpcode( buffer, &ptrX );
			key=0;
		}

	}

	Cursor_SetFlashMode(0);		// cursor flashing off
	if ( alpha_mode ) { Fkey_Clear(3); Fkey_Clear(4); Fkey_Clear(5);}
	
	buffer[length]='\0';
	return key ;	// EXE:0
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode) {		// 0123456789.(-)exp
	char buffer[32];
	int csrX=0;
	unsigned int key;

	buffer[csrX]='\0';
	if ( defaultNum != 0 ) {
		sprintG(buffer, defaultNum, width, LEFT_ALIGN);
		csrX=strlenOp(buffer);
	}
	key= InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, float_mode, exp_mode, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);  // exit
	return atof( (char*)buffer );
}

unsigned int InputStr(int x, int y, int width,  char* buffer, char SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key;
	
	buffer[width]='\0';
	csrX=strlenOp(buffer);
	key= InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	return ( key );
}

//---------------------------------------------------------------------------------------------- align dummy
//int InpObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int InpObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int InpObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
