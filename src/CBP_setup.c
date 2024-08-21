

#include "prizm.h"
#include "CBP.h"

//---------------------------------------------------------------------------------------------
int KeyRepeatFirstCount=20;		// pointer to repeat time of first repeat (20:default)
int KeyRepeatNextCount = 5;		// pointer to repeat time of second repeat( 5:default)

char  ComplexMode   = 0;		// Complex mode  real:0   a+bi:1  a_b:2
char  MaxHeapRam    = MAXHEAPDEFAULT;	// Max Heap Ram size Kbyte
char  EditGBFont    = 0;		// enable GB font
char  EnableExtFont = 0;		// enable external font
char  DirectGBconvert=0;		// GB code direct conversion for Text

short PageUpDownNum = PageUpDownNumDefault;	// PageUp/Down counter
char  EditTopLine = 1;		// 1: use top of line (edit)
char  EditFontSize = 0;		// 0:standard  1:mini  2:minifix 4:rev    +0x10:line-number
char  EditListChar = 0;		// List character change
char  EditExtFont = 0;		// Edit ext Font enable:1

char  ExtendPict = 0;		// 0:20  21~99							( use hidden ram only )
char  ExtendList = 5;		// 0:52(default) 1:+52 ~ 19:+988=1040	( use hidden ram only )
char  ForceReturnMode = 0;	// 0:none  1:F1 2:EXE  3:Both
char  ForceReturn;			// 0:none  1:return

char  CB_RecoverSetup = 1;	// setup recover flag 0:none 1:recover

char  CB_disableOC = 0;		// auto overclock disable :1

char  CB_fx5800P = 0;		// fx-5800P mode

char  CB_StatusDisp = 1;		// StatusDisp off:0  on:1

int  IsEmu=0;		//

char  CBDUMMY001=0;
char  CB_G3M_TEXT=0;
char  CBDUMMY002=0;
char  CB_ClrPict=0;	// 0:clear	 1:non clear
char  CBDUMMY003=0;
char  CB_HelpOn=0;	// Help function
char  CB_EditIndent=0;		// indent function  0:Non  1~4:indent space

int selectSetup=0;
int selectVar=0;
int selectMatrix=0;

//int StackPtr;

char BG_filename[64]="";

const char VerMSG[]="C.Basic CG v1.47\xE6\x41";	// VERSION
//---------------------------------------------------------------------------------------------

void VBattDispSub( int x, int y) {
	char buffer[32];
	sprintf3(buffer,"%4.2fv",(float)CB_BatteryStatus(buffer)/100);
	PrintMinix3( x, y, (unsigned char*)buffer, MINI_OVER );
}

void VerDispSub() {
	int freearea;
	char buffer[32];
	locate( 3, 2 ); Prints( (unsigned char*)VerMSG );
//	locate( 3, 3 ); Prints( (unsigned char*)"(C.Basic FX" );
//	locate( 3, 4 ); Prints( (unsigned char*)"     compatible+)" );
	locate( 3, 5 ); Prints( (unsigned char*)"     by sentaro21" );
	locate( 3, 6 ); Prints( (unsigned char*)"          (c)2023" );

	PrintMinix3(13*6+2, 2*8+1, (unsigned char*)" build 02 ", MINI_OVER );
	PrintMinix3( 2*6+2, 3*8+1, (unsigned char*)"(C.Basic FX compatible+)", MINI_OVER );

//	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		freearea = HiddenRAM_MatTopPtr - HiddenRAM_ProgNextPtr ;
		sprintf3(buffer,"%d bytes free",freearea);
		PrintMinix3( 2*6+2, 6*8+3, (unsigned char*)buffer, MINI_OVER );
		VBattDispSub( 15*6+2, 6*8+3 );
//		locate( 3, 5 ); Prints( (unsigned char*)buffer );
//	}

//	Bdisp_PutDisp_DD();
}
void VerDisp() {
	int key;
	PopUpWin( 6 );
	VerDispSub();
	EnableDisplayStatusArea();
	GetKey_DisableCatalog(&key);
//	EnableStatusArea(3);	// disable StatusArea
}

//----------------------------------------------------------------------------------------------
int GetMemFree() ;

int CB_Version() {	// Version
	return VERSION;
}
int OS_VersionMinor() {
	int OS1,OS2,OS3,OS4;
	SysCalljmp( 0, 0, (int)&OS3, 0, 0x002B);	// GlibGetOSVersionInfo
	return (OS3>>28)*1000 + ((OS3>>24)&0xF)*100 + ((OS3>>20)&0xF)*10 + ((OS3>>16)&0xF);
}

int System( int n ) {
	int a,r=0;
	if (( 101<=n)&&(n<120)) {	// Pict
		return (int)PictAry[n-100];
	}
	switch ( n ) {
		case -9:
			r=TryFlag; if ( r>1 ) r--;
			break;
		case -8:
			r=CB_StatusDisp;
			break;
		case -7:
			r=(IsEmu!=0);
			break;
		case -5:
			r=GetMemFree();
			break;
		case -4:
			r=MAXHEAP-MaxHeapRam;
			break;
		case -3:
			r=MaxHeapRam;
			break;
		case -22:
			r=OS_VersionMinor();
			break;
		case -2:
			r=OS_Version();
			break;
		case -1:	// CG1020 / CG50
			r=0;
			a=(int)GetVRAMAddress();
			if ( a == 0xA8000000 ) {
				r=20; if ( IsCG10() ) r=10;
			}
			if ( a == 0xAC000000 ) {
				r=50;
				if ( OS_VersionMinor()/1000 == 2 ) r=90;	// Graph 90+e
			}
			break;
		case 0:	// Version
			r=VERSION;
			break;
		case 1:	// VRAM
			r = (int)PictAry[0];
			break;
		case 2:	// TVRAM
			r = (int)TVRAM;
			break;
		case 3:	// GVRAM
			r = (int)GVRAM;
			break;
		case 4:	// WorkBuf
			r = (int)WorkBuf;
			break;
		case 9:	// HeapRAM
			r = (int)HeapRAM;
			break;
		case 10:	//
			r = (int)ClipBuffer;
			break;
		case 12:	//
			r = (int)files;
			break;

		case 20:	// EnableExtFont
			r = (int)EnableExtFont;
			break;
		case 21:	// &EnableExtFont
			r = (int)&EnableExtFont;
			break;

		case 30:	//
			r = (int)CB_TransparentColorIndex;
			break;

		case 40:	//
			r = (int)loadg3p_array.LoadG3B_buffer_ptr;
			break;
		case 41:	//
			r = (int)loadg3p_array.colormode;
			break;
		case 42:	//
			r = (int)loadg3p_array.c2;
			break;
		case 43:	//
			r = (int)loadg3p_array.c3;
			break;
		case 44:	//
			r = (int)loadg3p_array.c4;
			break;
		case 45:	//
			r = (int)loadg3p_array.i3;
			break;
		case 46:	//
			r = (int)loadg3p_array.width;
			break;
		case 47:	//
			r = (int)loadg3p_array.height;
			break;
		case 48:	//
			r = (int)loadg3p_array.i5;
			break;

		case 100:	//
			r = (int)GetStackPtr();
			break;
		case 101:	//
			r = (int)MatAry;
			break;

//		case 999:	//
//			r =StackPtr;
//			break;
		case 1000:	//
			r = (int)HiddenRAM_Top;
			break;
		case 1001:	//
			r = (int)HiddenRAM_End;
			break;
		case 1002:	//
			r = (int)HiddenRAM_ProgNextPtr;
			break;
		case 1003:	//
			r = (int)HiddenRAM_MatTopPtr;
			break;
		case 1010:	//
			r = (int)ProgfileMax[0];
			break;
		case 1011:	//
			r = (int)ProgfileMax[1];
			break;
		case 10001:	//
			r = FuncType;
			break;
		default:
			r = 0;
	}
	return r;
}

int CB_System( char *SRC ) {	// System( n )
	int r;
	int c = SRC[ExecPtr];
	int n = CB_EvalInt( SRC );
	if ( n==10000 ) {	// load config data to ListAns   Ststem(10000,1) Ststem(10000,2) Ststem(10000,3)
		if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
		ExecPtr++;
		c = CB_EvalInt( SRC );
		LoadConfig1data( c );
		goto exit;
	}
	r = System( n );
  exit:
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return r;
}

//----------------------------------------------------------------------------------------------
//		Setup
//----------------------------------------------------------------------------------------------

void FkeyS_L_(){
	int temp;
	temp=S_L_Style;
	if ( CB_G1MorG3M ==1 ) {
		Fkey_Icon( FKeyNo1, 977 );	//	Fkey_dispN( FKeyNo1, ""); Linesub(0*21+5,7*8+4,0*21+18,7*8+4, S_L_Normal,1);	// -----
		Fkey_Icon( FKeyNo2, 978 );	//	Fkey_dispN( FKeyNo2, ""); Linesub(1*21+5,7*8+4,1*21+18,7*8+4, S_L_Thick,1);	// =====
		Fkey_Icon( FKeyNo3, 979 );	//	Fkey_dispN( FKeyNo3, ""); Linesub(2*21+5,7*8+4,2*21+18,7*8+4, S_L_Broken,1);	// : : : :
		Fkey_Icon( FKeyNo4, 982 );	//	Fkey_dispN( FKeyNo4, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Dot,1);	// . . . .
	} else {
		Fkey_Icon( FKeyNo1, 1101);	//	Fkey_dispN( FKeyNo1, ""); Linesub(0*21+5,7*8+4,0*21+18,7*8+4, S_L_Normal,1);	// -----
		Fkey_Icon( FKeyNo2, 1102);	//	Fkey_dispN( FKeyNo2, ""); Linesub(1*21+5,7*8+4,1*21+18,7*8+4, S_L_Thick,1);	// =====
		Fkey_Icon( FKeyNo3, 1103);	//	Fkey_dispN( FKeyNo3, ""); Linesub(2*21+5,7*8+4,2*21+18,7*8+4, S_L_Broken,1);	// : : : :
		Fkey_Icon( FKeyNo4, 1104);	//	Fkey_dispN( FKeyNo4, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Dot,1);	// . . . .
	}
	Fkey_Icon( FKeyNo5, 1105);	//	Fkey_dispN( FKeyNo4, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Thin,1);	// -----
	FkeyClear( FKeyNo6 );
	S_L_Style=temp;
}

void SetLineStyle() {
	int key;

	FkeyS_L_();

	EnableDisplayStatusArea();
	GetKey_DisableCatalog(&key);
	switch (key) {
		case KEY_CTRL_EXIT:
			break;
		case KEY_CTRL_F1:
			S_L_Style=S_L_Normal;
			break;
		case KEY_CTRL_F2:
			S_L_Style=S_L_Thick ;
			break;
		case KEY_CTRL_F3:
			S_L_Style=S_L_Broken;
			break;
		case KEY_CTRL_F4:
			S_L_Style=S_L_Dot;
			break;
		case KEY_CTRL_F5:
			S_L_Style=S_L_Thin;
			break;
		default:
		break;
	}
}

//-----------------------------------------------------------------------------

void SetXdotYdot(){
		Xdot  = (Xmax-Xmin)/(126.*CB_G1MorG3M);
		Ydot  = (Ymax-Ymin)/( 62.*CB_G1MorG3M);
		CB_ClrGraphStat();
}
void SetVeiwWindowInit(){	// Initialize	return 0: no change  -1 : change
		Xmin  =-6.3;
		Xmax  = 6.3;
		Xscl  = 1.0;
		Ymin  =-3.1;
		Ymax  = 3.1;
		Yscl  = 1.0;
		TThetamin = 0;
		TThetamax = finvdegree( 360);
		TThetaptch= TThetamax/100;
		SetXdotYdot();
}
void SetVeiwWindowTrig(){	// trig Initialize
		Xmin  = finvdegree(-540);
		Xmax  = finvdegree( 540);
		Xscl  = finvdegree(  90);
		Ymin  =-1.6;
		Ymax  = 1.6;
		Yscl  = 0.5;
		TThetamin = 0;
		TThetamax = finvdegree( 360);
		TThetaptch= TThetamax/100;
		SetXdotYdot();
}
void SetVeiwWindowSTD(){	// STD Initialize
		Xmin  =-10;
		Xmax  = 10;
		Xscl  =  1;
		Ymin  =-10;
		Ymax  = 10;
		Yscl  =  1;
		TThetamin = 0;
		TThetamax = finvdegree( 360);
		TThetaptch= TThetamax/100;
		SetXdotYdot();
}


unsigned int XTTKey( unsigned int  key) {
	switch ( FuncType ) {
		case 1:
			key=0xCE;	// Theta
			break;
		case 2:
			key='T';
			break;
		case 3:
		case 8:
		case 9:
		case 10:
		case 11:
			key='Y';
			break;
		default:
			if ( ( CB_G1MorG3M==1 ) || ( XInputMethod ) ) key='X'; else key=0x90; // 'X' or 0x90
	}
	return key;
}

unsigned int MathKey( unsigned int  key) {
	switch ( key ) {
			case KEY_CHAR_IMGNRY:
//				return 0x7F50;
			case KEY_CHAR_MAT:
//				return 0x7F40;
			case KEY_CHAR_LIST:
//				return 0x7F51;
			case KEY_CHAR_ANGLE: //
//				return 0x7F54;
			case KEY_CHAR_DP:
			case KEY_CHAR_EXP:
			case KEY_CHAR_PLUS:
			case KEY_CHAR_MINUS:
			case KEY_CHAR_MULT:
			case KEY_CHAR_DIV:
			case KEY_CHAR_PMINUS:
			case KEY_CHAR_ANS:
			case KEY_CTRL_XTT:
			case KEY_CHAR_VALR:
			case KEY_CHAR_THETA:
			case KEY_CHAR_EQUAL:
			case KEY_CHAR_FRAC:   //    0xbb
			case KEY_CHAR_LPAR:   //    0x28
			case KEY_CHAR_RPAR:   //    0x29
			case KEY_CHAR_LOG:    //    0x95
			case KEY_CHAR_LN:     //    0x85
			case KEY_CHAR_SIN:    //    0x81
			case KEY_CHAR_COS:    //    0x82
			case KEY_CHAR_TAN:    //    0x83
			case KEY_CHAR_SQUARE: //    0x8b
			case KEY_CHAR_POW:    //    0xa8
			case KEY_CHAR_PI:     //    0xd0
			case KEY_CHAR_CUBEROOT: //  0x96
			case KEY_CHAR_EXPN10: //    0xb5
			case KEY_CHAR_EXPN:   //    0xa5
			case KEY_CHAR_ASIN:   //    0x91
			case KEY_CHAR_ACOS:   //    0x92
			case KEY_CHAR_ATAN:   //    0x93
			case KEY_CHAR_ROOT:   //    0x86
			case KEY_CHAR_POWROOT: //   0xb8
			case KEY_CHAR_LBRCKT: //
			case KEY_CHAR_RBRCKT: //
			case KEY_CHAR_LBRACE: //
			case KEY_CHAR_RBRACE: //
			case KEY_CHAR_RECIP: //
			case KEY_CHAR_DQUATE: //
				if ( key == KEY_CTRL_XTT ) key=XTTKey( key ); 	// 'X' or 0x90 or <r> or <Theta> or T
//				if ( key == KEY_CHAR_PLUS  )  key='+';
//				if ( key == KEY_CHAR_MINUS )  key='-';
//				if ( key == KEY_CHAR_PMINUS ) key=0x87; // (-)
				return key;
			default:
				if ( ( (  KEY_CHAR_0 <= key ) && ( key <= KEY_CHAR_9 ) ) ||
					 ( (  KEY_CHAR_A <= key ) && ( key <= KEY_CHAR_Z ) ) ) return key;
				break;
		}
	return 0;
}

void AllClr_GRAM(){
	int *vram=(int *)GVRAM;;
	int backcolor=(CB_BackColorIndex<<16)|CB_BackColorIndex;
	int i;
	for (i=0; i<216*384/2; i++ ) *(vram++) = backcolor;
}

int SetViewWindow(void){		// ----------- Set  View Window variable	return 0: no change  -1 : change
	char buffer[32];
	int key;
	int	cont=1;
	int select=0;
	int scrl=0;
	int y,n;
	double tmp;

	double	xmin      = Xmin      ;
	double	xmax      = Xmax      ;
	double	xscl      = Xscl      ;
	double	xdot      = Xdot      ;
	double	ymin      = Ymin      ;
	double	ymax      = Ymax      ;
	double	ydot      = Ydot      ;
	double	yscl      = Yscl      ;
	double	tThetamin = TThetamin ;
	double	tThetamax = TThetamax ;
	double	tThetaptch= TThetaptch;

	int bk_CB_ColorIndex=CB_ColorIndex;						// current color index
	unsigned short bk_CB_BackColorIndex=CB_BackColorIndex;	// current backcolor index
	unsigned short bk_CB_ColorIndexPlot=CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// White
	CB_ColorIndexPlot=0x001F;	// Blue

	Cursor_SetFlashOff(); 		// cursor flashing off
	while (cont) {
		Bdisp_AllClr_VRAM2();
		CB_ColorIndex=0x001F;	// Blue
		CB_Prints( 1, 1, (unsigned char*)"View Window");

		if ( scrl <=0 ) {
			locate( 1, 2-scrl); Prints((unsigned char*)"Xmin  :");
			sprintG(buffer,Xmin,  10,LEFT_ALIGN); locate( 8, 2-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl <=1 ) {
			locate( 1, 3-scrl); Prints((unsigned char*)" max  :");
			sprintG(buffer,Xmax,  10,LEFT_ALIGN); locate( 8, 3-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl <=2 ) {
			locate( 1, 4-scrl); Prints((unsigned char*)" scale:");
			sprintG(buffer,Xscl,10,LEFT_ALIGN); locate( 8, 4-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl <=3 ) {
			locate( 1, 5-scrl); Prints((unsigned char*)" dot  :");
			sprintG(buffer,Xdot,  10,LEFT_ALIGN); locate( 8, 5-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl <=4 ) {
			locate( 1, 6-scrl); Prints((unsigned char*)"Ymin  :");
			sprintG(buffer,Ymin,  10,LEFT_ALIGN); locate( 8, 6-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl <=5 ) {
			locate( 1, 7-scrl); Prints((unsigned char*)" max  :");
			sprintG(buffer,Ymax,  10,LEFT_ALIGN); locate( 8, 7-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl >=1 ) {
			locate( 1, 8-scrl); Prints((unsigned char*)" scale:");
			sprintG(buffer,Yscl,10,LEFT_ALIGN); locate( 8, 8-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl >=2 ) {
			locate( 1, 9-scrl); Prints((unsigned char*)"T\xE6\x47min :");
			sprintG(buffer,TThetamin,  10,LEFT_ALIGN); locate( 8, 9-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl >=3 ) {
			locate( 1, 10-scrl); Prints((unsigned char*)"  max :");
			sprintG(buffer,TThetamax,  10,LEFT_ALIGN); locate( 8, 10-scrl); Prints((unsigned char*)buffer);
		}
		if ( scrl >=4 ) {
			locate( 1, 11-scrl); Prints((unsigned char*)"  ptch:");
			sprintG(buffer,TThetaptch,  10,LEFT_ALIGN); locate( 8, 11-scrl); Prints((unsigned char*)buffer);
		}

		y = select-scrl+1;
		Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line

		Fkey_Icon( FKeyNo1,  95 );	//	Fkey_dispN( FKeyNo1, "Init");
		Fkey_Icon( FKeyNo2,  96 );	//	Fkey_dispN( FKeyNo2, "Trig");
		Fkey_Icon( FKeyNo3,  97 );	//	Fkey_dispN( FKeyNo3, "STD");
		Fkey_Icon( FKeyNo4,  69 );	//	Fkey_dispR( FKeyNo4, "STO");
		Fkey_Icon( FKeyNo5,  70 );	//	Fkey_dispR( FKeyNo5, "RCL");

		EnableDisplayStatusArea();
		GetKey_DisableCatalog( &key );
		switch (key) {
//			case KEY_CTRL_SHIFT:
//				GetKey_DisableCatalog( &key );
//				break;
			case KEY_CTRL_EXIT:
				goto exit;	// no change
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;

			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=9; scrl=4;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 9 ) {select=0; scrl=0;}
				if ((select - scrl) > 5 ) scrl+=1;
				if ( scrl > 4 ) scrl=4;
				break;

			case KEY_CTRL_F1:	// Initialize
				SetVeiwWindowInit();
				break;
			case KEY_CTRL_F2:	// trig Initialize
				SetVeiwWindowTrig();
				break;
			case KEY_CTRL_F3:	// STD Initialize
				SetVeiwWindowSTD();
				break;
			case KEY_CTRL_F4:	// Sto V-MEM
				n = SelectNum1( "Store", 0, 1, 6, &key );
				if ( key==KEY_CTRL_EXIT ) break;
				StoVwin( n );
				break;
			case KEY_CTRL_F5:	// Rcl V-MEM
				n = SelectNum1( "Recall", 0, 1, 6, &key );
				if ( key==KEY_CTRL_EXIT ) break;
				if ( VWinflag[n-1] == 0 ) break;
				RclVwin( n );
				break;

			case KEY_CTRL_LEFT:
				PutKey( KEY_CTRL_DOWN, 1 );
			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line
				FkeyClearAll();
				y++;
				switch (select) {
					case 0: // Xmin
						Xmin      =InputNumD_full( 8, y, 14, Xmin);	//
						SetXdotYdot();
						break;
					case 1: // Xmax
						Xmax      =InputNumD_full( 8, y, 14, Xmax);	//
						SetXdotYdot();
						break;
					case 2: // Xscl
						Xscl      =fabs(InputNumD_full( 8, y, 14, Xscl));	//
						break;
					case 3: // Xdot
						Xdot      =InputNumD_full( 8, y, 14, Xdot);	//
						Xmax = Xmin + Xdot*126.*CB_G1MorG3M;
						break;
					case 4: // Ymin
						Ymin      =InputNumD_full( 8, y, 14, Ymin);	//
						SetXdotYdot();
						break;
					case 5: // Ymax
						Ymax      =InputNumD_full( 8, y, 14, Ymax);	//
						SetXdotYdot();
						break;
					case 6: // Yscl
						Yscl      =fabs(InputNumD_full( 8, y, 14, Yscl));	//
						break;
					case 7: // TThetamin
						TThetamin =InputNumD_full( 8, y, 14, TThetamin);	//
						break;
					case 8: // TThetamax
						TThetamax =InputNumD_full( 8, y, 14, TThetamax);	//
						break;
					case 9: // TThetaptch
						tmp = InputNumD_full( 8, y, 14, TThetaptch);	//
						if ( tmp == 0 ) { CB_ErrMsg(OutOfDomainERR); break ; } // // Out of Domain error
						TThetaptch = tmp;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line
				FkeyClearAll();
				y++;
				switch (select) {
					case 0: // Xmin
						Xmin      =InputNumD_Char( 8, y, 14, Xmin, key);	//
						SetXdotYdot();
						break;
					case 1: // Xmax
						Xmax      =InputNumD_Char( 8, y, 14, Xmax, key);	//
						SetXdotYdot();
						break;
					case 2: // Xscl
						Xscl      =fabs(InputNumD_Char( 8, y, 14, Xscl, key));	//
						break;
					case 3: // Xdot
						Xdot      =InputNumD_Char( 8, y, 14, Xdot, key);	//
						Xmax = Xmin + Xdot*126.*CB_G1MorG3M;
						break;
					case 4: // Ymin
						Ymin      =InputNumD_Char( 8, y, 14, Ymin, key);	//
						SetXdotYdot();
						break;
					case 5: // Ymax
						Ymax      =InputNumD_Char( 8, y, 14, Ymax, key);	//
						SetXdotYdot();
						break;
					case 6: // Yscl
						Yscl      =fabs(InputNumD_Char( 8, y, 14, Yscl, key));	//
						break;
					case 7: // TThetamin
						TThetamin =InputNumD_Char( 8, y, 14, TThetamin, key);	//
						break;
					case 8: // TThetamax
						TThetamax =InputNumD_Char( 8, y, 14, TThetamax, key);	//
						break;
					case 9: // TThetaptch
						tmp = InputNumD_Char( 8, y, 14, TThetaptch, key);	//
						if ( tmp == 0 ) { CB_ErrMsg(OutOfDomainERR); break ; } // // Out of Domain error
						TThetaptch = tmp;
						break;
					default:
						break;
				}
			}
	}
  exit:
	CB_ColorIndex    =bk_CB_ColorIndex;		// current color index
	CB_BackColorIndex=bk_CB_BackColorIndex;	// current backcolor index
	CB_ColorIndexPlot=bk_CB_ColorIndexPlot;	// Plot color index (default blue)
	if( ( xmin      != Xmin      ) ||
	    ( xmax      != Xmax      ) ||
	    ( xscl      != Xscl      ) ||
	    ( xdot      != Xdot      ) ||
	    ( ymin      != Ymin      ) ||
	    ( ymax      != Ymax      ) ||
	    ( ydot      != Ydot      ) ||
	    ( yscl      != Yscl      ) ||
	    ( tThetamin != TThetamin ) ||
	    ( tThetamax != TThetamax ) ||
	    ( tThetaptch!= TThetaptch) )  {
			AllClr_GRAM();
			ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
			SaveConfig();
			return -1; // change value
	}
	return 0;	// no change
}

//-----------------------------------------------------------------------------
void SetFactor(){
	char buffer[32];
	int key;
	int	cont=1;
	int select=0;
	int y;

	PopUpWin(3);

	Cursor_SetFlashOff(); 		// cursor flashing off
	while (cont) {
		locate(3,3); Prints((unsigned char *)"Factor");
		locate(3,4); Prints((unsigned char *) "Xfact:           ");
		sprintG(buffer,Xfct,  10,LEFT_ALIGN); locate( 9, 4); Prints((unsigned char*)buffer);
		locate(3,5); Prints((unsigned char *) "Yfact:           ");
		sprintG(buffer,Yfct,  10,LEFT_ALIGN); locate( 9, 5); Prints((unsigned char*)buffer);

		y = select + 3 ;
		Bdisp_AreaReverseVRAMx3(12, y*8, 113, y*8+7);	// reverse select line
//		Bdisp_PutDisp_DD();

		EnableDisplayStatusArea();
		GetKey_DisableCatalog( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;

			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) select=1;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 1 ) select=0;
				break;

			case KEY_CTRL_LEFT:
				PutKey( KEY_CTRL_DOWN, 1 );
			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAMx3(12, y*8, 113, y*8+7);	// reverse select line
//				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // Xfct
						Xfct  =InputNumD_full( 9, y, 10, Xfct);	//
						select+=1;
						break;
					case 1: // Yfct
						Xmax  =InputNumD_full( 9, y, 10, Yfct);	//
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAMx3(12, y*8, 113, y*8+7);	// reverse select line
//				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // Xfct
						Xfct  =InputNumD_Char( 9, y, 10, Xfct, key);	//
						select+=1;
						break;
					case 1: // Yfct
						Yfct  =InputNumD_Char( 9, y, 10, Yfct, key);	//
						break;
					default:
						break;
				}
		}

	}
	SaveConfig();
}

//-----------------------------------------------------------------------------
void SetVarDsp(int VarMode) {
	if ( VarMode ) {
		Prints((unsigned char*)"[INT%]");
	} else {
		if ( CB_INT==0 ) Prints((unsigned char*)"[DBL#]");
		else	Prints((unsigned char*)"[CPLX]");
	}
}
void InitVar( complex value, int VarMode, int small) {
	char buffer[64];
	int key;
	int	cont=1;
	int i,reg;

	PopUpWin(3);
	Cursor_SetFlashOff(); 		// cursor flashing off
	while (cont) {
		locate( 3,3); Prints((unsigned char *)"Init All Variable");
		locate( 3,5); Prints((unsigned char *)"value:           ");
		Cplx_sprintGR1cutlim( buffer, value, 11,LEFT_ALIGN, Norm, 10 );
		locate( 9, 5); Prints((unsigned char*)buffer);
		locate(1,8); PrintLine((unsigned char *)" ",21);
		locate(1,8); SetVarDsp(VarMode);
//		Bdisp_PutDisp_DD();

		EnableDisplayStatusArea();
		GetKey_DisableCatalog( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return ;
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				PutKey( KEY_CTRL_DOWN, 1 );
			case KEY_CTRL_RIGHT:
				value  =InputNumC_full( 9, 5, 11, value);	//
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
			value  =InputNumC_Char( 9, 5, 11, value, key);	//
		}
	}

	if ( YesNo("Initialize Ok?") ) {
		if ( VarMode ) {
			for ( i=0; i<26; i++) LocalInt[i+small][0]=value.real;
		} else {
			for ( i=0; i<26; i++) LocalDbl[i+small][0]=value;
		}
	}
}


void SetVarSel(int VarMode, int y, int miniflag ) {
	if ( miniflag )	Bdisp_AreaReverseVRAM(0, y*18+26, 383, y*18+17+26);	// reverse select line
	else			Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line
	ML_rectangle_CG( 384-6, 0, 383, 168, 0, 0, 1, 0xFFFF, 0xFFFF );
	locate(1,8); PrintLine((unsigned char *)" ",21);
	locate(1,8); SetVarDsp(VarMode);
}

void SetVarDblToHex( char * buffer, double n ) {
	if ( ( (n-floor(n))==0 ) && ( -2147483648. <= n ) && ( n <= 2147483647. ) ) {
		sprintf3(buffer,"0x%08X        ",(int)n);
	} else {
		sprintG(buffer, n,      18,LEFT_ALIGN);
	}
}
int CmpAliasVar( int reg ) {	// AliasVar ?
	int c,i,j;
	for ( i=0; i<=AliasVarMAX; i++ ) {
		if ( AliasVarCode[i].org==reg ) return i;
		if ( AliasVarCode[i].org<0 ) return -1;
	}
	return -1;
}
int CmpAliasVarMat( int reg ) {	// AliasVar Mat?
	int c,i,j;
	for ( i=0; i<=AliasVarMAXMat; i++ ) {
		if ( AliasVarCodeMat[i].org==reg ) return i;
		if ( AliasVarCodeMat[i].org<0 ) return -1;
	}
	return -1;
}

int SetVarCharMat( char *buffer, int c ) {
	int ptr=0,len;
	short alias_code;
	char buffer2[32];
	int i=CmpAliasVarMat( c );
	if ( i>=0 ) {	// 	Alias variable
		if ( alias_code=AliasVarCodeMat[i].alias == 0x4040 ) {	// @ABCD
			len=AliasVarCodeMat[i].len;
			memcpy( buffer, AliasVarCodeMat[i].name, len);
			buffer[len]='\0';
			ptr=len;
		} else {
			CB_OpcodeToStr( alias_code, buffer2 ) ;		// SYSCALL+
			StrMid( buffer, buffer2, 1, 8 );
			ptr=CB_MB_strlen( buffer );
		}
	} else
	if ( ( c == 26 ) ) {	// <r>
		buffer[ptr++]=0xCD;	//	'r'
	} else
	if ( ( c == 27 ) ) { // Theta
		buffer[ptr++]=0xCE;		// Theta
//		buffer[ptr++]=0xE6;		// Theta
//		buffer[ptr++]=0x47;
	} else
	if ( ( 28 <= c ) && ( c <= 31 ) ) { // Ans
		buffer[ptr++]='A';		//
		buffer[ptr++]='n';
		buffer[ptr++]='s';
	} else
	if ( ( 84 <= c ) && ( c <= 109 ) ) {		// vct
		buffer[ptr++]='A'+c-84;
	} else 
//	if ( ( 58 <= c ) && ( c <= 83 ) ) {		// List
//		buffer[ptr++]='A'+c-84;
//	} else 
	if ( c >= 110 ) {
		sprintf( buffer+ptr, "%d", c-57 );
		ptr = strlen(buffer);
	} else {
		buffer[ptr++]='A'+c;
	}
	return ptr;
}
/*
int SetVarCharVct( char *buffer, int c ) {
	int ptr=0,len;
	short alias_code;
	char buffer2[32];
	int i=CmpAliasVarMat( c );
	if ( i>=0 ) {	// 	Alias variable
		if ( alias_code=AliasVarCodeMat[i].alias == 0x4040 ) {	// @ABCD
			len=AliasVarCodeMat[i].len;
			memcpy( buffer, AliasVarCodeMat[i].name, len);
			buffer[len]='\0';
			ptr=len;
		} else {
			CB_OpcodeToStr( alias_code, buffer2 ) ;		// SYSCALL+
			StrMid( buffer, buffer2, 1, 8 );
			ptr=CB_MB_strlen( buffer );
		}
	} else
	if ( ( c == 26 ) ) {	// <r>
		buffer[ptr++]=0xCD;	//	'r'
	} else
	if ( ( c == 27 ) ) { // Theta
		buffer[ptr++]=0xCE;		// Theta
//		buffer[ptr++]=0xE6;		// Theta
//		buffer[ptr++]=0x47;
	} else
	if ( ( 28 <= c ) && ( c <= 31 ) ) { // Ans
		buffer[ptr++]='A';		//
		buffer[ptr++]='n';
		buffer[ptr++]='s';
	} else
	if ( c >= 58 ) {
		buffer[ptr++]='A'+c-84;
	} else {
		buffer[ptr++]='A'+c;
	}
	return ptr;
}
*/
int SetVarChar( char *buffer, int c ) {
	int ptr=0,len;
	short alias_code;
	char buffer2[32];
	int i=CmpAliasVar( c );
	if ( i>=0 ) {	// 	Alias variable
		alias_code=AliasVarCode[i].alias ;
		if ( alias_code == 0x4040 ) {	// _ABCD
			len=AliasVarCode[i].len;
			memcpy( buffer, AliasVarCode[i].name, len);
			buffer[len]='\0';
			ptr=len;
		} else {
			CB_OpcodeToStr( alias_code, buffer2 ) ;		// SYSCALL+
			StrMid( buffer, buffer2, 1, 8 );
			ptr=CB_MB_strlen( buffer );
		}
	} else
	if ( ( c == 26 ) ) {	// <r>
		buffer[ptr++]=0xCD;	//	'r'
	} else
	if ( ( c == 27 ) ) { // Theta
		buffer[ptr++]=0xCE;		// Theta
//		buffer[ptr++]=0xE6;		// Theta
//		buffer[ptr++]=0x47;
	} else
	if ( ( 28 <= c ) && ( c <= 31 ) ) { // Ans
		buffer[ptr++]='A';		//
		buffer[ptr++]='n';
		buffer[ptr++]='s';
	} else {
		buffer[ptr++]='A'+c;
	}
	return ptr;
}
int SetVarCharStr( char *buffer, int VarMode, int k) {
	int f,j=0;
	j+=SetVarChar( buffer+j, k );
	if ( VarMode ) {		// Int variable
		f=( LocalInt[k] == &REGINT[k] );
	} else {				// Double variable
		f=( LocalDbl[k] == &REG[k] );
	}
	if ( VarMode ) {		// Int variable
		buffer[j++]='%';
	}
	if ( f ) {
		buffer[j++]='=';
	} else {
		buffer[j++]=0xE5;		// reverse '='
		buffer[j++]=0xB8;
	}

	buffer[j++]='\0';
	return CB_MB_ElementCount( buffer )+1;
}

int SetVar(int select){		// ----------- Set Variable
	char buffer[64];
	char buffer2[64];
	char buffer3[64];
	int key;
	int	cont=1;
	int scrl=0;
	int seltop=select;
	int i,j,k,f,y,x;
	int selectreplay=-1;
	int opNum=25+3,lnum;
	int small=0;
	int ynum;
	int yk;
	int wx;
	int miniflag=(EditFontSize & 0x07);
	complex value={0,0};
	int bk_CB_INT=CB_INT;
	int VarMode=CB_INT;	// 0:double or complex  1:int
	int hex=0;	// 0:normal  1:hex
	int bk_CB_ColorIndex=CB_ColorIndex;						// current color index
	int bk_G1MorG3M=CB_G1MorG3M;
	unsigned short bk_CB_BackColorIndex=CB_BackColorIndex;	// current backcolor index
	unsigned short bk_CB_ColorIndexPlot=CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// White
	CB_ColorIndexPlot=0x001F;	// Blue
	Cursor_SetFlashOff(); 		// cursor flashing off

	KeyRecover();
	Setup_SetEntry(0x14, 0x00 ); alphastatus = 0; alphalock = 0;

	if ( VarMode==2 ) VarMode=0;	// complex ->double

	if ( select >= IsExtVar ) select=IsExtVar;
	if ( select >= 58 ) { small=58; }
	else
	if ( select >= 32 ) { small=32; }
	select-=small;

	while (cont) {
		if ( miniflag ) { ynum=9; yk=18; wx=32; } else { ynum=7; yk=24; wx=22; }
		Bdisp_AllClr_VRAM2();

		opNum=25+3;
		if ( small==58 ) { opNum=IsExtVar-57;}
		if ( opNum < ynum-1 ) lnum=opNum; else lnum=ynum-1;
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > lnum ) seltop = select-lnum;
		if ( (opNum -seltop) < lnum ) seltop = opNum -lnum;

		for ( i=0; i<ynum; i++ ) {
			k=seltop+i+small;
			if ( ( small==58 ) && ( k > IsExtVar+1 ) ) break;	// ext   Var

			if ( ( small==32 ) && ( k >= 58 ) ) k-=(32);						// small Var
			else
			if ( ( small==58 ) && ( k > IsExtVar ) ) k=28;	// ext   Var
			x=SetVarCharStr( buffer, VarMode, k);
			CB_ColorIndex=0x0000;
			if ( miniflag ) CB_PrintMini_Fix( (1-1)*12, i*yk+3, (unsigned char*)buffer, MINI_OVER ) ;	// fixed 12 dot
			else			CB_Prints( 1, 1+i, (unsigned char*)buffer);


			if ( VarMode ) {
				locate(x, 1+i);		// int
				if ( hex )	sprintf3(buffer,"0x%08X        ",(int)LocalInt[k][0]);
					else	sprintG(buffer, (double)LocalInt[k][0], 18,LEFT_ALIGN);
			} else {
				locate(x, 1+i);		// dbl
				if ( hex )	SetVarDblToHex( buffer, LocalDbl[k][0].real );
				else Cplx_sprintGR1cutlim( buffer, LocalDbl[k][0], wx-x,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
			}
			if ( miniflag ) CB_PrintMini_Fix( (x-1)*12, i*yk+3, (unsigned char*)buffer, MINI_OVER ) ;	// fixed 12 dot
			else			Prints((unsigned char*)buffer);
		}

//		Fkey_Icon( FKeyNo1, 775 );	//	Fkey_dispN( FKeyNo1, "A <> a");
		if ( small ==  0  ) Fkey_Icon( FKeyNo1, 775 );	//	Fkey_dispN( FKeyNo1, "A <> a");
		else
		if ( small == 32  ) Fkey_Icon( FKeyNo1, 775 );	//	Fkey_dispN( FKeyNo1, "A <> a");
		else
		if ( small == 58  ) Fkey_dispN( FKeyNo1, "_Var");

//		if ( small == 58  ) {	// extended variable
//			Fkey_dispN( FKeyNo1, "_Var");
//			sprintf3(buffer,"%2d:%2d",IsExtVar-57, AliasVarMAX+1 );
//			Fkey_dispN( FKeyNo1, buffer );
//		}
		Fkey_Icon( FKeyNo2,  95 );	//	Fkey_dispN( FKeyNo2, "Init");
//		if ( VarMode ) Fkey_dispN_aA( 2, "D <> I"); else Fkey_dispN_Aa( 2, "D <> I");
		if ( hex ) Fkey_dispN( FKeyNo6, "\xE6\x91\x44\x65\x63"); else Fkey_dispN( FKeyNo6, "\xE6\x91Hex");

//		locate(12,8); SetVarDsp( VarMode );
		Fkey_dispN( FKeyNo3, "INT%");
		Fkey_dispN( FKeyNo4, "DBL#");
		Fkey_dispN( FKeyNo5, "CPLX");
		if ( VarMode ) {
//			locate(12,8); Prints((unsigned char*)"[INT%]");
//			CB_PrintMinix3( 4*12, 8*7+1, (unsigned char*)"[INT%]");
			Fkey_dispRS( FKeyNo3, "INT%");
		} else {
			if ( CB_INT==0 ) Fkey_dispRS( FKeyNo4, "DBL#");
			else	Fkey_dispRS( FKeyNo5, "CPLX");
		}

		y = (select-seltop) ;
		if ( miniflag )	{
			Bdisp_AreaReverseVRAM(0, y*18+26, 383, y*18+17+26);	// reverse select line
			CB_ScrollBar( opNum+1, seltop, 9, 384-6, 0, 168, 6 ) ;
		} else {
			Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line
			CB_ScrollBar( opNum+1, seltop, 7, 384-6, 0, 168, 6 ) ;
		}

		k=select;
		if ( small==58 ) {	// ext   Var
			k+=small;
			if ( k > IsExtVar ) k=28;	// ext   Var  Ans
		} else {
			 if ( select<=25 ) k+=small;
		}
		CB_G1MorG3M = bk_G1MorG3M;
		StatusArea_Time();
		EnableDisplayStatusArea();
		GetKey_DisableCatalog( &key );
//		EnableStatusArea(3);	// disable StatusArea
		FkeyClear( FKeyNo6 );
		CB_G1MorG3M=3;
		switch (key) {

//			case KEY_CTRL_SHIFT:
			case KEY_CTRL_OPTN:
				miniflag=1;
				goto VARjp;
			case KEY_CTRL_VARS:
				miniflag=0;
//				if ( small == 58  ) {	// extended variable
				  VARjp:
					FkeyClearAll();
					sprintf3(buffer,"Alias Var:%2d      Extended Var(_):%2d",IsExtVar-57, AliasVarMAX+1 );
//					CB_PrintMini( 16*18+6, (cnt-scrl)*24-18,(unsigned char*)buffer,MINI_OVER);
					PrintMinix3(0*6+2, 7*8+1, (unsigned char*)buffer, MINI_OVER );
//					Fkey_dispN( FKeyNo1, buffer );
//				}
				Bdisp_PutDisp_DD();
				while ( CheckKeyRow7305(8) & (32+16) );	// [OPTN] [VARS]
				break;

			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;

//			case KEY_CTRL_DEL:
//				i=CmpAliasVar( k );
//				if ( i>=0 ) {	// 	Alias variable delete
//					DeleteAliasVar( k );
//				}
//				break;

//			case 0x756E:	// SHIFT + <-
			case KEY_SHIFT_LEFT:
				miniflag=1;
				break;
//			case 0x756F:	// SHIFT + ->
			case KEY_SHIFT_RIGHT:
				miniflag=0;
				break;

			case KEY_CTRL_F1:
				if ( small ==  0  ) small=32;
				else
				if ( small == 32  ) { if ( IsExtVar < 58 ) small =0; else { small=58; if ( select > IsExtVar-58 ) select=IsExtVar-58; } }
				else
				if ( small == 58  ) { small= 0; if ( select >= 28 ) select=28; }
				break;
			case KEY_CTRL_F2:
				InitVar(value,VarMode, small);
				break;
			case KEY_CTRL_F3:
				CB_INT = 1;		// INT#
				VarMode=1;
				ExpBuffer[0]='\0';
				break;
			case KEY_CTRL_F4:
				CB_INT = 0;		// DBL#
				VarMode=0;
				ExpBuffer[0]='\0';
				break;
			case KEY_CTRL_F5:
				CB_INT = 2;		// CPLX
				VarMode=0;
				ExpBuffer[0]='\0';
				break;
			case KEY_CTRL_F6:
				hex=1-hex;
				break;
			case KEY_CTRL_UP:
				select--;
				if ( select < 0 ) select = opNum;
				selectreplay = -1; // replay cancel
				break;
			case KEY_CTRL_DOWN:
				select++;
				if ( select > opNum ) select =0;
				selectreplay = -1; // replay cancel
				break;
			case KEY_CTRL_PAGEUP:
				select-=ynum;
				if ( select < 0 ) select = 0;
				selectreplay = -1; // replay cancel
				break;
			case KEY_CTRL_PAGEDOWN:
				select+=ynum;
				if ( select > opNum ) select =opNum;
				selectreplay = -1; // replay cancel
				break;

			case KEY_CTRL_RIGHT:
				SetVarSel(VarMode, y, miniflag);
				x=SetVarCharStr( buffer, VarMode, k);
				y++;
				selectreplay = select;
				if ( ( 0 <= select ) && ( select <=opNum ) ) {	// regA to regZ
					if ( VarMode )
						if ( miniflag )	LocalInt[k][0]= InputNumC_fullhex_mini( (x-1)*12, (y-1)*yk+3, 31, Int2Cplx(LocalInt[k][0]), hex, miniflag ).real;
						else			LocalInt[k][0]= InputNumC_fullhex( x, y, 18, Int2Cplx(LocalInt[k][0]), hex).real;
					else
						if ( miniflag )	LocalDbl[k][0]= InputNumC_fullhex_mini( (x-1)*12, (y-1)*yk+3, 31, LocalDbl[k][0], hex, miniflag);
						else			LocalDbl[k][0]= InputNumC_fullhex( x, y, 19, LocalDbl[k][0], hex);
				} else {
						selectreplay = -1; // replay cancel
				}
				break;

			case KEY_CTRL_LEFT:
				if (selectreplay<0) { PutKey( KEY_CTRL_RIGHT, 1 ); PutKey( KEY_CTRL_DOWN, 1 ); break; }
				SetVarSel(VarMode, y, miniflag);
				x=SetVarCharStr( buffer, VarMode, k);
				y++;
				selectreplay = select;
				if ( ( 0 <= select ) && ( select <=opNum ) ) {	// regA to regZ
					if ( VarMode )
						if ( miniflag ) LocalInt[k][0]= InputNumD_replay_mini( (x-1)*12, (y-1)*yk+3, 31, (double)LocalInt[k][0], miniflag);
						else			LocalInt[k][0]= InputNumD_replay( x, y, 18, (double)LocalInt[k][0]);
					else
						if ( miniflag ) LocalDbl[k][0]= InputNumC_replay_mini( (x-1)*12, (y-1)*yk+3, 31, LocalDbl[k][0], miniflag);
						else			LocalDbl[k][0]= InputNumC_replay( x, y, 19, LocalDbl[k][0]);
				} else {
						selectreplay = -1; // replay cancel
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				SetVarSel(VarMode, y, miniflag);
				x=SetVarCharStr( buffer, VarMode, k);
				y++;
				selectreplay = select;
				if ( ( 0 <= select ) && ( select <=opNum ) ) {	// regA to regZ
					if ( VarMode )
						if ( miniflag ) LocalInt[k][0]= InputNumD_Char_mini( (x-1)*12, (y-1)*yk+3, 30, (double)LocalInt[k][0], key, miniflag );
						else			LocalInt[k][0]= InputNumD_Char( x, y, 18, (double)LocalInt[k][0], key);
					else
						if ( miniflag ) LocalDbl[k][0]= InputNumC_Char_mini( (x-1)*12, (y-1)*yk+3, 30, LocalDbl[k][0], key, miniflag );
						else			LocalDbl[k][0]= InputNumC_Char( x, y, 19, LocalDbl[k][0], key);
				} else {
						selectreplay = -1; // replay cancel
				}
		}
	}
	CB_ColorIndex    =bk_CB_ColorIndex;		// current color index
	CB_BackColorIndex=bk_CB_BackColorIndex;	// current backcolor index
	CB_ColorIndexPlot=bk_CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_G1MorG3M      =bk_G1MorG3M;
	CB_INT=bk_CB_INT;
	SaveConfig();
	if ( small ) select+=small;
	return select;
}

//-----------------------------------------------------------------------------
//	Color Picker
//-----------------------------------------------------------------------------
void PrintColorBoxChar( int x, int y, int rev ) {
	int coloetmp=CB_ColorIndex;
	int bk_GBcode=GBcode;
	GBcode=0;
	if ( rev ) CB_ColorIndex^=0xFFFF;
	if ( CB_ColorIndex == 0xFFFF )	CB_Prints( x, y, (unsigned char*)"\xE6\xA5");
	else 							CB_Prints( x, y, (unsigned char*)"\xE6\xA6");
	CB_ColorIndex=coloetmp;
	GBcode=bk_GBcode;
}
void disp_setupcolor( int select, int scrl, int cnt, int ColIndex  ){
    const char *color[]   ={"Black","Blue","Green","Cyan","Red","Magenta","Yellow","White","Color"};
	int	i=GetColor2Index(ColIndex);
	CB_ColorIndex=ColIndex;
   	PrintColorBoxChar( 14, cnt-scrl, ( (select-scrl) == (cnt-scrl-1) ) );
	if ( i==8 ) PrintColorBoxChar( 20, cnt-scrl, ( (select-scrl) == (cnt-scrl-1) ) );
	CB_ColorIndex=-1;
	CB_Prints( 15, cnt-scrl, (unsigned char*)color[i]);
}

int SetRGBColorMode =0;	// 0:RGB  1:HSV   2:HSL
//int SetRGBColor_color=0x001F;
char reentrant_SetRGBColor = 0;	//	reentrant ok

int SetRGBColor( unsigned short *color ){	// 1-
	char buffer[22];
	char *selectstr="\xE5\xED";
	int key;
	int	cont=1;
	int select=0,cselect=0;
	int i,x,y;
	int c,d,r,g,b;
	int h,s,v,l;
	int H,S,V,L;
	int c1,c2;
	int repeat;
	int prevH,prevS,prevV,prevL;
	unsigned char colortable[][3] = {
		{  0,  0,  0},
		{  0,  0,  0},{  0,  0,160},{160,  0,  0},{160,  0,160},{  0,160,  0},{  0,160,160},{160,160,  0},{160,160,160},
		{  0,  0,248},{248,  0,  0},{248,  0,248},{  0,252,  0},{  0,252,248},{248,252,  0},{248,252,248}
		};
	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

	int bk_CB_ColorIndex=CB_ColorIndex;						// current color index
	unsigned short bk_CB_BackColorIndex=CB_BackColorIndex;	// current backcolor index
	unsigned short bk_CB_ColorIndexPlot=CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// White
	CB_ColorIndexPlot=0x001F;	// Blue

	if ( reentrant_SetRGBColor > 0 ) return 0;
	reentrant_SetRGBColor++;	//	reentrant diable

	b = ((*color&0x001F) << 3);
	g = ((*color&0x07E0) >> 3);
	r = ((*color&0xF800) >> 8);
	rgb2hsv( r,g,b, &H,&S,&V);
	rgb2hsl( r,g,b, &H,&S,&L);

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
//	MsgBoxPush( 5 );
	KeyRecover();

	while (cont) {
		ML_rectangle_CG( 35, 17, 347, 145, 0, 0, 1, 0xFFFF, 0xFFFF );

		if ( r<  0 ) r=  0;
		if ( r>248 ) r=248;
		if ( g<  0 ) g=  0;
		if ( g>252 ) g=252;
		if ( b<  0 ) b=  0;
		if ( b>248 ) b=248;
		if ( H<  0 ) H=359;
		if ( H>359 ) H=  0;
		if ( S<  0 ) S=  0;
		if ( S>255 ) S=255;
		if ( V<  0 ) V=  0;
		if ( V>252 ) V=252;
		if ( L<  0 ) L=  0;
		if ( L>250 ) L=250;
		switch ( SetRGBColorMode ) {
			case 0:	// RGB
				r &=0xFE;
				g &=0xFE;
				b &=0xFE;
				break;
			case 1:	// HSV->RGB
				hsv2rgb( H,S,V, &r,&g,&b );
				break;
			case 2:	// HSL->RGB
				hsl2rgb( H,S,L, &r,&g,&b );
				break;
		}

		if ( select==3 ) {
			r=colortable[cselect][0];
			g=colortable[cselect][1];
			b=colortable[cselect][2];
			if ( SetRGBColorMode == 1 ) rgb2hsv( r,g,b, &H,&S,&V);
			if ( SetRGBColorMode == 2 ) rgb2hsl( r,g,b, &H,&S,&L);
		}
		colortable[0][0]=r;
		colortable[0][1]=g;
		colortable[0][2]=b;

		if ( ( select<2 ) && ( SetRGBColorMode ) ) {	// HSv or HSl
			 locate( 6,2); Prints((unsigned char *)"\xE6\x90\xE6\x91\xE6\x92\xE6\x93 +SHIFT");
		} else {
			 locate( 6,2); Prints((unsigned char *)"SelectColor");
		}
		CB_ColorIndex=-1;
		CB_ColorIndex=rgb( r, g, b);   CB_Prints(  3, 2, (unsigned char*)"\xE6\xA6\xE6\xA6\xE6\xA6");
		CB_ColorIndex=rgb( r, g, b);   CB_Prints( 17, 2, (unsigned char*)"\xE6\xA6\xE6\xA6\xE6\xA6");
		switch ( SetRGBColorMode ) {
			case 0:	// RGB
				CB_ColorIndex=rgb( 255,  0,  0); PrintColorBoxChar( 3, 3, select==0 );
				locate( 4,3); Prints((unsigned char *) "R(0~248):");
				sprintG(buffer,r,  10,LEFT_ALIGN); locate(13, 3); Prints((unsigned char*)buffer);
				CB_ColorIndex=rgb(  0, 255,  0); PrintColorBoxChar( 3, 4, select==1 );
				locate( 4,4); Prints((unsigned char *) "G(0~252):");
				sprintG(buffer,g,  10,LEFT_ALIGN); locate(13, 4); Prints((unsigned char*)buffer);
				CB_ColorIndex=rgb(  0,   0,255); PrintColorBoxChar( 3, 5, select==2 );
				locate( 4,5); Prints((unsigned char *) "B(0~248):");
				sprintG(buffer,b,  10,LEFT_ALIGN); locate(13, 5); Prints((unsigned char*)buffer);
				break;

			case 1:	// HSV
//				rgb2hsv( r,g,b, &H,&S,&V);
				for( h=0; h<360; h+=4) {
					for( s=0; s<256; s+=4) {
						CB_ColorIndex=hsv( h, s, V);
						ML_pixel_CG( h/4+36, (255-s)/4+48, ML_BLACK, hsv( h, s, V), 0xFFFF );
					}
				}
				if ( V<200 ) { c1=0xFFFF; c2=0x0000; } else { c1=0x0000; c2=0xFFFF; }
				ML_rectangle_CG( H/4+35, (255-S)/4+47, H/4+37, (255-S)/4+49, 1, 1, -1, c1, c2 );
				CB_ColorIndex=hsv(H,255,255); PrintColorBoxChar( 8, 3, select==0 );
				locate( 9,3); Prints((unsigned char *) "H(0~359)");
				sprintG(buffer,H,  10,LEFT_ALIGN); locate(17, 3); Prints((unsigned char*)buffer);
				CB_ColorIndex=hsv(H,  S,255); PrintColorBoxChar( 8, 4, select==1 );
				locate( 9,4); Prints((unsigned char *) "S(0~255)");
				sprintG(buffer,S,  10,LEFT_ALIGN); locate(17, 4); Prints((unsigned char*)buffer);
				CB_ColorIndex=hsv(0,  0,  V); PrintColorBoxChar( 8, 5, select==2 );
				locate( 9,5); Prints((unsigned char *) "V(0~252)");
				sprintG(buffer,V,  10,LEFT_ALIGN); locate(17, 5); Prints((unsigned char*)buffer);
//				hsv2rgb( H,S,V, &r,&g,&b );
				break;
			case 2:	// HSL
//				rgb2hsl( r,g,b, &H,&S,&L);
				for( h=0; h<360; h+=4) {
					for( s=0; s<256; s+=4) {
						CB_ColorIndex=hsl( h, s, L);
						ML_pixel_CG( h/4+36, (255-s)/4+48, ML_BLACK, hsl( h, s, L), 0xFFFF );
					}
				}
				if ( L<100 ) { c1=0xFFFF; c2=0x0000; } else { c1=0x0000; c2=0xFFFF; }
				ML_rectangle_CG( H/4+35, (255-S)/4+47, H/4+37, (255-S)/4+49, 1, 1, -1, c1, c2 );
				CB_ColorIndex=hsl(H,255,128); PrintColorBoxChar( 8, 3, select==0 );
				locate( 9,3); Prints((unsigned char *) "H(0~359)");
				sprintG(buffer,H,  10,LEFT_ALIGN); locate(17, 3); Prints((unsigned char*)buffer);
				CB_ColorIndex=hsl(H,  S,128); PrintColorBoxChar( 8, 4, select==1 );
				locate( 9,4); Prints((unsigned char *) "S(0~255)");
				sprintG(buffer,S,  10,LEFT_ALIGN); locate(17, 4); Prints((unsigned char*)buffer);
				CB_ColorIndex=hsl(0,  0,  L); PrintColorBoxChar( 8, 5, select==2 );
				locate( 9,5); Prints((unsigned char *) "L(0~250)");
				sprintG(buffer,L,  10,LEFT_ALIGN); locate(17, 5); Prints((unsigned char*)buffer);
//				hsl2rgb( H,S,L, &r,&g,&b );
				break;
		}

		for ( i=0; i<16; i++ ) {
			x=i+3;
			CB_ColorIndex=rgb( colortable[i][0], colortable[i][1], colortable[i][2]);
			PrintColorBoxChar( x, 6, select==3 );
		}
		CB_ColorIndex=-1;
		if ( select==3 ) {
			ML_rectangle_CG( (cselect*6+12)*3+1, 5*8*3+2, (cselect*6+12+5)*3+2, 6*8*3-6, 1, 1, -1, 0x0000, 0xFFFF );
		}

		FkeyClearAll();
		if ( SetRGBColorMode == 0 ) Fkey_dispRS( FKeyNo1, "RGB"); else Fkey_dispN( FKeyNo1, "RGB");
		if ( SetRGBColorMode == 1 ) Fkey_dispRS( FKeyNo2, "HSV"); else Fkey_dispN( FKeyNo2, "HSV");
		if ( SetRGBColorMode == 2 ) Fkey_dispRS( FKeyNo3, "HSL"); else Fkey_dispN( FKeyNo3, "HSL");
		Fkey_dispN( FKeyNo4, "16col");
		if ( SetRGBColorMode == 1 ) Fkey_dispN( FKeyNo5, "hsV\xE6\x90");
		if ( SetRGBColorMode == 2 ) Fkey_dispN( FKeyNo5, "hsL\xE6\x90");
		if ( SetRGBColorMode == 1 ) Fkey_dispN( FKeyNo6, "hsV\xE6\x91");
		if ( SetRGBColorMode == 2 ) Fkey_dispN( FKeyNo6, "hsL\xE6\x91");

		y = select + 3 ;
		if ( SetRGBColorMode ) {	// HSV HSL
			if ( y<6 ) 	Bdisp_AreaReverseVRAM( 42*3, y*24-1, 113*3+4, y*24+22);	// reverse select line
			else 		Bdisp_AreaReverseVRAM( 12*3, y*24-1, 113*3+4, y*24+22);	// reverse select line
		} else			Bdisp_AreaReverseVRAM( 12*3, y*24-1, 113*3+4, y*24+22);	// reverse select line

		EnableDisplayStatusArea();
		i = RTC_GetTicks();
		GetKey_DisableCatalog( &key );
		if ( (RTC_GetTicks()-i)< 10 ) repeat=1; else repeat=0;
		switch (key) {
			case KEY_CTRL_EXE:
				*color=rgb( r, g, b);
				goto exit;
				break;
			case KEY_CTRL_EXIT:
				goto exit;
				break;

			case KEY_CTRL_UP:
				if ( ( select==0 ) && ( SetRGBColorMode ) ) select=1;
				if ( ( select==1 ) && ( SetRGBColorMode ) ) {
					S += 4+4*repeat;
					if ( S>255 ) S=0;
					break;
 				}
			case KEY_CTRL_PAGEUP:
				select-=1;
				if ( select < 0 ) { select=3; cselect=0; }
				break;

			case KEY_CTRL_DOWN:
				if ( ( select==0 ) && ( SetRGBColorMode ) ) select=1;
				if ( ( select==1 ) && ( SetRGBColorMode ) ) {
					S -= 4+4*repeat;
					if ( S<0 ) S=255;
					break;
 				}
			case KEY_CTRL_PAGEDOWN:
				select+=1;
				if ( select > 3 ) select=0;
				if ( select== 3 ) cselect=0;
				break;


			case KEY_CTRL_RIGHT:
				if ( ( select==1 ) && ( SetRGBColorMode ) ) select=0;
				switch (select) {
					case 0: //
						switch ( SetRGBColorMode ) {
							case 0:	// RGB	green
								r+=8;
								break;
							case 1:	// HSV	H
							case 2:	// HSL	H
								H+=4+4*repeat;
								if ( H>=359 ) H-=360;
								break;
						}
						break;
					case 1: //
						switch ( SetRGBColorMode ) {
							case 0:	// RGB	green
								g+=4;
								break;
//							case 1:	// HSV	S
//							case 2:	// HSL	S
//								S+=4;
//								if ( S>255 ) S=255;
//								break;
						}
 						break;
					case 2: //
						switch ( SetRGBColorMode ) {
							case 0:	// RGB	blue
								b+=8;
								break;
							case 1:	// HSV	V
								V += 4+4*repeat;
//								for (d=0; d<=repeat*2; d++){
//									c = hsv(H,S,V);
//									for (i=0;i<=255;i++) { if ( c!=hsv(H,S,V) ) break; else V++; }
//									if ( i>=255 ) V=255;
//								}
								break;
							case 2:	// HSL	L
								L += 2+2*repeat;
//								for (d=0; d<=repeat*2; d++){
//									c = hsl(H,S,L);
//									for (i=0;i<=255;i++) { if ( c!=hsl(H,S,L) ) break; else L++; }
//									if ( i>=255 ) L=255;
//								}
								break;
						}
						break;
 					case 3: // 16color select
						cselect++; if ( cselect>15 ) cselect=0;
						break;
					case 4:	// mode select
						switch ( SetRGBColorMode ) {
							case 0:	// RGB -> HSV
								SetRGBColorMode = 1;
//								if ( r==248 ) r=255;
//								if ( g==252 ) g=255;
//								if ( b==248 ) b=255;
								rgb2hsv( r,g,b, &H,&S,&V);
								break;
							case 1:	// HSV -> HSL
								SetRGBColorMode = 2;
								rgb2hsl( r,g,b, &H,&S,&L);
								break;
							case 2:	// HSL -> RGB
								SetRGBColorMode = 0;
								hsl2rgb( H,S,L, &r,&g,&b );
								break;
						}
					default:
						break;
				}
				break;
			case KEY_CTRL_LEFT:
				if ( ( select==1 ) && ( SetRGBColorMode ) ) select=0;
				switch (select) {
					case 0: //
						switch ( SetRGBColorMode ) {
							case 0:	// RGB	green
								r-=8;
								break;
							case 1:	// HSV	H
							case 2:	// HSL	H
								H-=4+8*repeat;
								if ( H<0 ) H+=360;
								break;
						}
						break;
					case 1: //
						switch ( SetRGBColorMode ) {
							case 0:	// RGB	green
								g-=4;
								break;
//							case 1:	// HSV	S
//							case 2:	// HSL	S
//								S-=4;
//								if ( S<0 ) S=0;
//								break;
						}
 						break;
					case 2: //
						switch ( SetRGBColorMode ) {
							case 0:	// RGB	blue
								b-=8;
								break;
							case 1:	// HSV	V
								V -= 4+4*repeat;
//								for (d=0; d<=repeat*2; d++){
//									c = hsv(H,S,V);
//									for (i=0;i<=255;i++) { if ( c!=hsv(H,S,V) ) break; else V--; }
//									if ( i>=255 ) V=0;
//								}
								break;
							case 2:	// HSL	L
								L -= 2+2*repeat;
//								for (d=0; d<=repeat*2; d++){
//									c = hsl(H,S,L);
//									for (i=0;i<=255;i++) { if ( c!=hsl(H,S,L) ) break; else L--; }
//									if ( i>=255 ) L=0;
//								}
								break;
						}
 						break;
 					case 3: // 16color select
						cselect--; if ( cselect<0 ) cselect=15;
						break;
					case 4:	// mode select
						switch ( SetRGBColorMode ) {
							case 0:	// RGB -> HSL
								SetRGBColorMode = 2;
//								if ( r==248 ) r=255;
//								if ( g==252 ) g=255;
//								if ( b==248 ) b=255;
								rgb2hsl( r,g,b, &H,&S,&L);
								break;
							case 1:	// HSV -> RGB
								SetRGBColorMode = 0;
								hsv2rgb( H,S,V, &r,&g,&b );
								break;
							case 2:	// HSL -> HSV
								SetRGBColorMode = 1;
								rgb2hsv( r,g,b, &H,&S,&V);
								break;
						}
					default:
						break;
				}
				break;

			case KEY_CTRL_F1:
				SetRGBColorMode = 0;
				break;
			case KEY_CTRL_F2:
//				if ( r==248 ) r=255;
//				if ( g==252 ) g=255;
//				if ( b==248 ) b=255;
				rgb2hsv( r,g,b, &H,&S,&V);
				SetRGBColorMode = 1;
				break;
			case KEY_CTRL_F3:
//				if ( r==248 ) r=255;
//				if ( g==252 ) g=255;
//				if ( b==248 ) b=255;
				rgb2hsl( r,g,b, &H,&S,&L);
				SetRGBColorMode = 2;
				break;
			case KEY_CTRL_F4:
				select = 3;
				break;
			case KEY_CTRL_F5:
				switch ( SetRGBColorMode ) {
					case 1:	// HSV	V
						V -= 4+4*repeat;
//						for (d=0; d<=repeat*6; d++){
//							c = hsv(H,S,V);
//							for (i=0;i<=255;i++) { if ( c!=hsv(H,S,V) ) break; else V--; }
//							if ( i>=255 ) V=0;
//						}
						select = 2;
						break;
					case 2:	// HSL	L
						L -= 2+2*repeat;
//						for (d=0; d<=repeat*4; d++){
//							c = hsl(H,S,L);
//							for (i=0;i<=255;i++) { if ( c!=hsl(H,S,L) ) break; else L--; }
//							if ( i>=255 ) L=0;
//						}
						select = 2;
						break;
				}
				break;
			case KEY_CTRL_F6:
				switch ( SetRGBColorMode ) {
					case 1:	// HSV	V
						V += 4+4*repeat;
//						for (d=0; d<=repeat*6; d++){
//							c = hsv(H,S,V);
//							for (i=0;i<=255;i++) { if ( c!=hsv(H,S,V) ) break; else V++; }
//							if ( i>=255 ) V=255;
//						}
						select = 2;
						break;
					case 2:	// HSL	L
						L += 2+2*repeat;
//						for (d=0; d<=repeat*4; d++){
//							c = hsl(H,S,L);
//							for (i=0;i<=255;i++) { if ( c!=hsl(H,S,L) ) break; else L++; }
//							if ( i>=255 ) L=255;
//						}
						select = 2;
						break;
				}
				break;

			default:
				break;
		}
		if ( select<3 ) {
			key=MathKey( key );
			if ( key ) {
				FkeyClearAll();
				switch ( SetRGBColorMode ) {
					case 0:	// RGB
						Bdisp_AreaReverseVRAM( 12*3, y*24-1, 113*3+4, y*24+22);	// reverse select line
						switch (select) {
							case 0: // red
								CB_ColorIndex=rgb( 255,  0,  0); PrintColorBoxChar( 3, 3, 0 );
								r  =InputNumD_Char( 13, y, 7, r, key);	//
								break;
							case 1: // green
								CB_ColorIndex=rgb(  0, 255,  0); PrintColorBoxChar( 3, 4, 0 );
								g  =InputNumD_Char( 13, y, 7, g, key);	//
								break;
							case 2: // blue
								CB_ColorIndex=rgb(  0,   0,255); PrintColorBoxChar( 3, 5, 0 );
								b  =InputNumD_Char( 13, y, 7, b, key);	//
								break;
							default:
								break;
						}
						break;
					case 1:	// HSV
					case 2:	// HSL
						Bdisp_AreaReverseVRAM( 42*3, y*24-1, 113*3+4, y*24+22);	// reverse select line
						switch (select) {
							case 0: // H
								H  =InputNumD_Char( 17, y, 3, H, key);	//
								break;
							case 1: // S
								S  =InputNumD_Char( 17, y, 3, S, key);	//
								break;
							case 2: // V
								if ( SetRGBColorMode == 1 ) V  =InputNumD_Char( 17, y, 3, V, key);	// HSV
								else 						L  =InputNumD_Char( 17, y, 3, L, key);	// HSL
								break;
							default:
								break;
						}
						break;
				}
			}
		}
	}
  exit:
	CB_ColorIndex    =bk_CB_ColorIndex;		// current color index
	CB_BackColorIndex=bk_CB_BackColorIndex;	// current backcolor index
	CB_ColorIndexPlot=bk_CB_ColorIndexPlot;	// Plot color index (default blue)
	RestoreDisp(SAVEDISP_PAGE1);
//	MsgBoxPop();
	reentrant_SetRGBColor = 0;	//	reentrant ok
	return key;
}

//-----------------------------------------------------------------------------
//--------------------------------------------------------------
int DateCursorY;
int TimeCursorY;

void DateTimePrintSub(){		// timer IRQ handler
	char buffer[32];
	char DateStr[16];
	char TimeStr[16];
	int cy,y;
	cy=DateCursorY & 0xFF;
	y =DateCursorY / 0x100;
	if ( ( 1<=cy ) && ( cy<=7 ) ) {
			DateToStr(DateStr);
			sprintf3(buffer,"DATE : %s",DateStr);
			locate( 1, cy);
			if ( (y+1)!=cy ) Prints((unsigned char*)buffer);
			else 			 PrintRevs((unsigned char*)buffer);
//			Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
	}
	cy=TimeCursorY & 0xFF;
	y =TimeCursorY / 0x100;
	if ( ( 1<=cy ) && ( cy<=7 ) ) {
			TimeToStr(TimeStr);
			sprintf3(buffer,"TIME : %s      ",TimeStr);
			locate( 1, cy);
			if ( (y+1)!=cy ) Prints((unsigned char*)buffer);
			else 			 PrintRevs((unsigned char*)buffer);
//			if ( (y+1)!=cy ) Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
	}
}
void DateTimePrints(){		// timer IRQ handler
	DateTimePrintSub();
	Bdisp_PutDisp_DD();
}

void TimePrintSetMode(int set){	// 1:on  0:off
	int dy,ty;
	dy=DateCursorY & 0xFF;
	ty=TimeCursorY & 0xFF;
	switch (set) {
		case 0:
			if ( timer_id ) KillTimer(timer_id);
			timer_id=0;
			break;
		case 1:
			if ( timer_id ) KillTimer(timer_id);
			timer_id=SetTimer(timer_id0, 1000, &DateTimePrints);
			break;
		default:
			break;
	}
}
//--------------------------------------------------------------
int SelectNum1sub( char *msg0, char*msg, int n ,int min, int max, int *key ) {		//
	char buffer[32];
	int n0=n;
	PopUpWin(3);
	FkeyClearAll();
	locate( 3,3); Prints((unsigned char *)msg0);
	locate( 3,5); sprintf3(buffer,"%s[%d~%d]:",msg,min,max); Prints((unsigned char *)buffer);
	while (1) {
		n=InputNumD(3+strlen(buffer), 5, log10(max)+1, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &(*key));		// 0123456789
		if ( n == n0 ) break;
		if ( (min<=n)&&(n<=max) ) break;
		n=n0;
	}
	return n ; // ok
}
int SelectNum1( char*msg, int n ,int min, int max, int *key ) {		//
	return SelectNum1sub( "Select Number", msg, n ,min, max, &(*key) );
}
int SelectNum2( char*msg, int n ,int min, int max ) {		//
	int key;
	return SelectNum1( msg, n ,min, max, &key );
}
int SelectNum3( int n ) {		//
	int key;
	int n0=n;
	PopUpWin(3);
	FkeyClearAll();
	locate( 3,3); Prints((unsigned char *)"SkipUp/Dw Number");
	locate( 6,5); Prints((unsigned char *)"[1~9999]:");
	while (1) {
		n=InputNumD(15, 5, 4, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &key);		// 0123456789
		if ( n == n0 ) break;
		if ( (1<=n)&&(n<=9999) ) break;
		n=n0;
	}
	return n ; // ok
}
int SelectNum4( int n ) {		//
	int key;
	int n0=n;
	PopUpWin(3);
	FkeyClearAll();
	locate( 3,3); Prints((unsigned char *)"Select Number");
	locate( 3,3); Prints((unsigned char *)"RefrshTime n/128s");
	locate( 4,5); Prints((unsigned char *)"[0=\x7F\x53,1~128]:");
	while (1) {
		n=InputNumD(16, 5, 3, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &key);		// 0123456789
		if ( n == n0 ) break;
		if ( (0<=n)&&(n<=128) ) break;
		n=n0;
	}
	return n ; // ok
}

//--------------------------------------------------------------

#define SETUP_Angle			0
#define SETUP_ComplexMode	1
#define SETUP_Variable		2
#define SETUP_FuncType		3
#define SETUP_DrawType		4
#define SETUP_Coord			5
#define SETUP_Grid			6
#define SETUP_Axes			7
#define SETUP_Label			8
#define SETUP_Derivative	9
#define SETUP_Background	10
#define SETUP_PlotLineColor	11
#define SETUP_Sketch		12
#define SETUP_Display		13
#define SETUP_ExpDisplay	14
#define SETUP_Help			15
#define SETUP_RecoverSetup	16
#define SETUP_CMDINPUT		17
#define SETUP_XINPUT		18
#define SETUP_EnableExtFONT	19
#define SETUP_EditExtFont	20
#define SETUP_EditGBFONT	21
#define SETUP_EditFontSize	22
#define SETUP_EditIndent	23
#define SETUP_EditLineNum	24
#define SETUP_EditListChar	25
#define SETUP_EditColorBack 26
#define SETUP_EditColorBase 27
#define SETUP_EditColorNum  28
#define SETUP_EditColorCMD  29
#define SETUP_EditColorQuot 30
#define SETUP_EditColorComt 31
#define SETUP_EditColorLine 32
#define SETUP_EnableOC		33
#define SETUP_MaxHeapRam	34
#define SETUP_UseHidnRam	35
#define SETUP_HidnRamInit	36
#define SETUP_ExtendPict	37
#define SETUP_ExtendList	38
#define SETUP_DisableDebugMode	39
#define SETUP_ExitDebugModeCheck	40
#define SETUP_BreakStop		41
#define SETUP_ExecTimeDsp	42
#define SETUP_IfEndCheck	43
#define SETUP_ACBreak		44
#define SETUP_ForceReturnMode	45
#define SETUP_Key1sttime	46
#define SETUP_KeyReptime	47
#define SETUP_SkipUpDown	48
#define SETUP_MatDspmode	49
#define SETUP_Matrixbase	50
#define SETUP_DATE			51
#define SETUP_TIME			52
#define SETUP_RootFolder	53
#define SETUP_FavoriteColor	54
#define SETUP_AutoSaveMode	55
#define SETUP_Forceg1msave	56
#define SETUP_DirectGBconvert	57
#define SETUP_Pictmode		58
#define SETUP_Storagemode	59
#define SETUP_RefrshCtlDD	60
#define SETUP_DefaultWaitcount	61
#define SETUP_G1MorG3M		62
#define SETUP_Executemode	63

const char *CBmode[]    ={"DBL#","INT%","CPLX"};

int SetupG(int select, int limit ){		// ----------- Setup
    const char *onoff[]       ={"off","on"};
    const char *gridonoff[]   ={"off","on","Line"};
    const char *axesonoff[]   ={"off","on","Scale"};
    const char *cplxmode[]    ={"Real","a+b\x7F\x50","r\x7F\x54\xE6\x47"};
    const char *draw[]        ={"Connect","Plot"};
    const char *style[]       ={"Normal","Thick","Broken","Dot","Thin"};
    const char *degrad[]      ={"Deg","Rad","Gra"};
    const char *display[]     ={"Norm","Fix","Sci"};
    const char *expdisplay[]  ={"1.23\x0F-98","Std \xE6\xFC\xE6\xFE\xE5\xC9\xE5\xC8","All \xE6\xFC\xE6\xFE\xE5\xC9\xE5\xC8"};
    const char *GBonoff[]     ={"off","on","Full"};
    const char *ENGmode[]     ={"  ","/E","  ","/3"};
    const char *CMDinput[]    ={"C.Basic","Standard","CB5800","Std5800"};
    const char *Xinput[]      ={"[\x90]","[X]"};
    const char *CharSize[]    ={"Std Fix ","Minifont",    "MinifontFix","MiniMinifont",    "MiniMiniFix",    "MiniMiniBold",    "MiniMiniBoldFix",   "MiniMini 6x8 FX",
    						 "Std +GB ","Minifont+Gap","MiniFix+Gap","MiniMinifont+Gap","MiniMiniFix+Gap","MiniMiniBold+Gap","MiniMiniBldFix+Gap","MiniMini 6x8 FX+Gap"};
    const char *Matmode[]     ={"[m,n]","[x,y]"};
    const char *Matbase[]     ={"0","1"};
    const char *Pictmode[]    ={"S.Mem","Heap","Both"};
    const char *PictmodeSD[]  ={"SDcard ","Heap","Both"};
    const char *Storagemode[] ={"S.Mem","SDcard ","Main Mem"};
    const char *DDmode[]      ={"off","Grph","All"};
    const char *ListChar[]    ={"List"," \xE5\xB7"," \xFF\xE0"};
    const char *G1MG3Mmode[]  ={"Auto","g1m","","g3m"};
    const char *Returnmode[]  ={"None","F1","EXE","F1&EXE"};
    const char *ExecTimemode[]={"off","on","off%","on reset","  %","on%","  % reset","on%reset"};
    const char *EditIndent[]  ={"off","1","2","3","4","5","6","7","off sav-","1 save-","2 save-","3 save-","4 save-","D","E","F"};
    const char *FuncTypeStr[]  ={"Y=","r=","Param","X=","Y>","Y<","Y\x12","Y\x10","X>","X<","X\x12","X\x10"};
	char buffer[22];
	char folderbuf[16];
	int key;
	int	cont=1;
	int scrl=select-6;
	int i,y,cnt;
	char DateStr[16];
	char TimeStr[16];
	int year,month,day,hour,min,sec;
	int listmax=SETUP_Executemode;
	int mini;
	int coloetmp=CB_ColorIndex;
	unsigned short us,backcoloetmp=CB_BackColorIndex;
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;
	int subselect=0;

	strcpy( folderbuf, folder );	// current folder

	Cursor_SetFlashOff(); 		// cursor flashing off

	if ( select > listmax ) select=0;
	if ( select < scrl ) scrl-=1;
	if ( scrl < 0 ) scrl=0;

	DateCursorY=-1;
	TimeCursorY=-1;

	SaveDisp(SAVEDISP_PAGE1);

	while (cont) {
		Bdisp_AllClr_VRAM3(0,191);
//		SysCalljmp( 0x07, 0, 0, 0, 0x1EF8);	// SetBackGround(

		DateToStr(DateStr);
		TimeToStr(TimeStr);
		DateCursorY = 0x909;
		TimeCursorY = 0x909;

		cnt=1;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Angle       :");		// 0
			locate(14, cnt-scrl); Prints((unsigned char*)degrad[Angle]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Complex Mode:");		// 1
			locate(14, cnt-scrl); Prints((unsigned char*)cplxmode[ComplexMode]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Variable    :");		// 2
			locate(14, cnt-scrl); 
			if ( VarListRange==0 ) {
				Prints((unsigned char*)"Range");
			} else {
				sprintf3((char*)buffer,"List%d", VarListRange);
				Prints((unsigned char*)buffer);
			}
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Func Type   :");		// 3
			locate(14, cnt-scrl); Prints((unsigned char*)FuncTypeStr[(int)FuncType]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Draw Type   :");		// 4
			locate(14, cnt-scrl); Prints((unsigned char*)draw[(int)DrawType]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Coord       :");		// 5
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[Coord]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Prints((unsigned char*)"Grid        :");		// 6
			locate(14, cnt-scrl); Prints((unsigned char*)gridonoff[Grid]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Axes        :");		// 7
			locate(14, cnt-scrl); Prints((unsigned char*)axesonoff[Axes]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Label       :");		// 8
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[Label]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Derivative  :");		// 9
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[Derivative]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			if ( BG_Pict_No == 21+ExtendPict )	{
				locate( 1, cnt-scrl); Prints((unsigned char*)"BG:");			// 10
				i=strlen(BG_filename); if ( i>25 ) i-=25;
				y=(cnt-scrl-1)*24;
				CB_ColorIndex=0x0000;
				CB_PrintMini( 4*18, y+4, (unsigned char*)BG_filename, MINI_OVER );
			} else {
				locate( 1, cnt-scrl); Prints((unsigned char*)"Background  :");
				if ( BG_Pict_No == 0 )	sprintf3((char*)buffer,"None");
				else					sprintf3((char*)buffer,"Pict%d",BG_Pict_No);
				locate(14,cnt-scrl); Prints((unsigned char*)buffer);
			}
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Plot/LineCol:");		// 11
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexPlot );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Sketch Line :");		// 12
			locate(14, cnt-scrl); Prints((unsigned char*)style[S_L_Style]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Display     :");		// 13
			locate(14, cnt-scrl); Prints((unsigned char*)display[CB_Round.MODE]);
			buffer[0]='\0';
			sprintf3((char*)buffer,"%d",CB_Round.DIGIT);
			locate(17+(CB_Round.MODE==0), cnt-scrl); Prints((unsigned char*)buffer);
			locate(20, cnt-scrl);
			Prints((unsigned char*)ENGmode[ENG]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Exp Display :");		// 14
			locate(14, cnt-scrl); Prints((unsigned char*)expdisplay[CB_Round.ExpTYPE]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Syntax Help :");		// 15
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[CB_HelpOn]);
			if ( CB_HelpOn ) {
				disp_setupcolor( select, scrl, cnt, CB_HelpFrameColorIndex );
//				locate(14, cnt-scrl); Prints((unsigned char*)onoff[CB_HelpOn]);
			} else {
				locate(14, cnt-scrl); Prints((unsigned char*)onoff[CB_HelpOn]);
			}
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"SetupRecover:");		// 16
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[CB_RecoverSetup]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Command Inpt:");		// 17
			locate(14, cnt-scrl); Prints((unsigned char*)CMDinput[ (CommandInputMethod + CB_fx5800P*2) & 3 ]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"[\x90]character:");	// 18
			locate(14, cnt-scrl); Prints((unsigned char*)Xinput[XInputMethod]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"EnableExFont:");		// 19
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[EnableExtFont]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Edit ExtFont:");		// 20
//			CB_Prints_ext( 1,cnt-scrl,(unsigned char*)"Edit ExtFont:", EditExtFont*0x100 );		// extflag
			CB_Prints_ext(14,cnt-scrl,(unsigned char*)onoff[EditExtFont], EditExtFont*0x100 );	// extflag
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Edit GB Font:");		// 21
			locate(14, cnt-scrl); Prints((unsigned char*)GBonoff[EditGBFont]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"EditFontSize:");		// 22
			y=(cnt-scrl-1)*24;
			CB_ColorIndex=0x0000;
			switch ( EditFontSize & 0x07 ) {
				case 0:	// Standard
					CB_Prints_ext(14,cnt-scrl,(unsigned char*)CharSize[EditFontSize & 0x0F], EditExtFont*0x100 );	// extflag
					break;
				case 1:	// Mini
					if ( CB_G1MorG3M==1 ) CB_PrintMini_fx( 13*18, y+4, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OVER | EditExtFont*0x100 );	// extflag
					else				  CB_PrintMini(    13*18, y+4, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OVER | EditExtFont*0x100 );	// extflag
					break;
				case 2:	// Mini fix
					CB_PrintMini_Fix( 13*18, y+4, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OR | EditExtFont*0x100);	// extflag
					break;
				case 3:	// MiniMini
					CB_PrintMiniMini( 13*18, y+8, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OVER);
					break;
				case 4:	// MiniMini fix
					CB_PrintMiniMini_Fix( 13*18, y+8, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OR);
					break;
				case 5:	// MiniMiniBold
					CB_PrintMiniMiniBold( 13*18, y+8, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OVER);
					break;
				case 6:	// MiniMiniBold fix
					CB_PrintMiniMiniBold_Fix( 13*18, y+8, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OR);
					break;
				case 7:	// MiniMini 6x8
					CB_PrintMiniMiniFX( 13*18, y+10, (unsigned char*)CharSize[EditFontSize & 0x0F], MINI_OVER | EditExtFont*0x100 );	// extflag
					break;
			}
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Edit +Indent:");		// 23
			locate(14, cnt-scrl); Prints((unsigned char*)EditIndent[CB_EditIndent]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Edit LineNum:");		// 24
			locate(14, cnt-scrl); Prints((unsigned char*)onoff[(EditFontSize & 0xF0)>>4 ]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"EditListChar:");		// 25
			CB_Prints(14, cnt-scrl, (unsigned char*)ListChar[EditListChar]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Edit-backCol:");		// 26
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditBack );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Edit-baseCol:");		// 27
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditBase );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"E-NumericCol:");		// 28
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditNum );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"E-CommandCol:");		// 29
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditCMD );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Ed-QuotColor:");		// 30
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditQuot );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"E-CommentCol:");		// 31
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditComt );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"E-LineNumCol:");		// 32
			disp_setupcolor( select, scrl, cnt, CB_ColorIndexEditLine );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Auto OverClk:");		// 33
			locate(14, cnt-scrl);
			if ( System( -1 ) <50 ) Prints((unsigned char*)onoff[CB_disableOC==0]);
			else                    Prints((unsigned char*)"---");
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"HeapRAM Size:");		// 34
			sprintf3((char*)buffer,"%dKbyte",MaxHeapRam);
			locate(14,cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Use Hidn RAM:");		// 35
			locate(14,cnt-scrl); Prints((unsigned char*)onoff[UseHiddenRAM&0x0F]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"HidnRAM Init:");		// 36
			locate(14,cnt-scrl);
			if ( UseHiddenRAM&0x0F ) Prints((unsigned char*)onoff[!(UseHiddenRAM&0xF0)]);
			else                     Prints((unsigned char*)"---");
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Max Pict No :");		// 37
			sprintf3((char*)buffer,"%d",20+ExtendPict);
			locate(14, cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Max List 52\xA9:");	// 38
			sprintf3((char*)buffer,"%d (%d)", ExtendList+1, 52+ExtendList*52);
			locate(14, cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"AT DebugMode:");		// 39
			locate(14,cnt-scrl); Prints((unsigned char*)onoff[!DisableDebugMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"ExitDM PopUp:");		// 40
			locate(14,cnt-scrl); Prints((unsigned char*)onoff[ExitDebugModeCheck&1]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Break Stop  :");		// 41
			locate(14,cnt-scrl); Prints((unsigned char*)onoff[BreakCheckDefault]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Exec TimeDsp:");		// 42
			locate(14,cnt-scrl); Prints((unsigned char*)ExecTimemode[TimeDsp]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"IfEnd Check :");		// 43
			locate(14,cnt-scrl); Prints((unsigned char*)onoff[CheckIfEnd]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"ACBreak     :");		// 44
			locate(14,cnt-scrl); Prints((unsigned char*)onoff[ACBreak]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Force Return:");		// 45
			locate(14,cnt-scrl); Prints((unsigned char*)Returnmode[ForceReturnMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Key 1st time:");		// 46
			sprintf3((char*)buffer,"%dms",KeyRepeatFirstCount*25);
			locate(14,cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Key Rep time:");		// 47
			sprintf3((char*)buffer,"%dms",KeyRepeatNextCount*25);
			locate(14,cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"SkipUp/Down :");		// 48
			sprintf3((char*)buffer,"%d",PageUpDownNum);
			locate(14,cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Mat Dsp mode:");		// 49
			locate(14,cnt-scrl); Prints((unsigned char*)Matmode[MatXYmode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Matrix base :");		// 50
			locate(14,cnt-scrl); Prints((unsigned char*)Matbase[MatBaseDefault]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){		// DATE						// 51
			DateCursorY = cnt-scrl+0x900;
			DateTimePrintSub();
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){		// TIME						// 52
			TimeCursorY = cnt-scrl+0x900;
			DateTimePrintSub();
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Root Folder:");		// 53
			locate(13,cnt-scrl);
			if ( root2[0] == '\0' ) {
				Prints((unsigned char*)"/");
			} else {
				Prints((unsigned char*)root2);
			}
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Prints((unsigned char*)"Favorite Col:");		// 54
			disp_setupcolor( select, scrl, cnt, CB_FavoriteColorIndex );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Auto file save:");		// 55
			locate(16,cnt-scrl); Prints((unsigned char*)onoff[AutoSaveMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Force g1m save:");		// 56
			locate(16,cnt-scrl); Prints((unsigned char*)onoff[ForceG1Msave]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Direct GB cnvt:");		// 57
			locate(16,cnt-scrl); Prints((unsigned char*)onoff[DirectGBconvert]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Pict mode   :");		// 58
			locate(14,cnt-scrl); Prints((unsigned char*)Pictmode[PictMode]);
			if ( CB_ClrPict==0) { locate( 19,cnt-scrl); Prints((unsigned char*)(unsigned char*)"[C]"); }

		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Storage mode:");		// 59
			locate(14,cnt-scrl); Prints((unsigned char*)Storagemode[StorageMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"RefrshCtl DD:");		// 60
			locate(14,cnt-scrl); Prints((unsigned char*)DDmode[RefreshCtrl]);
			buffer[0]='\0';
			sprintf3((char*)buffer,"%2d/128",Refreshtime+1);
			CB_ColorIndex=0x0000;
			if ( RefreshCtrl ) CB_PrintMini_Fix10( 17*18+6, (cnt-scrl)*24-18,(unsigned char*)buffer,MINI_OR);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Wait count  :");		// 61
			if ( DefaultWaitcount == 0 )	sprintf3((char*)buffer,"No Wait");
			else					sprintf3((char*)buffer,"%d",DefaultWaitcount);
			locate(14,cnt-scrl); Prints((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"G1M/G3M mode:");		// 62
			locate(14,cnt-scrl); Prints((unsigned char*)G1MG3Mmode[CB_G1MorG3MDefault]);
			CB_ColorIndex=0x0000;
			if ( CB_G3M_TEXT ) CB_PrintMini( 16*18+6, (cnt-scrl)*24-18,(unsigned char*)"Text(x3)",MINI_OR);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Prints((unsigned char*)"Execute mode:");		// 63
			locate(14,cnt-scrl); Prints((unsigned char*)CBmode[CB_INTDefault]);
		}
		y = select-scrl;
		Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
		CB_ScrollBar( listmax+1, scrl, 7, 384-6, 0, 168, 6 ) ;

		Fkey_Icon( FKeyNo6, 991 );	//	Fkey_dispN( FKeyNo6, "Ver.");
		switch (select) {
			case SETUP_Variable:
				Fkey_Icon( FKeyNo1, 376 );	//	Fkey_dispN( FKeyNo1, "Range");
				Fkey_Icon( FKeyNo2, 178 );	//	Fkey_dispR( FKeyNo2, "LIST");
				break;
			case SETUP_DrawType: // Draw Type
				Fkey_Icon( FKeyNo1, 357 );	//	Fkey_dispN( FKeyNo1, "Con");
				Fkey_Icon( FKeyNo2, 358 );	//	Fkey_dispN( FKeyNo2, "Plot");
				break;
			case SETUP_ComplexMode: // complex mode
				Fkey_Icon( FKeyNo1, 942 );	//	Fkey_dispN( FKeyNo1, "Real");
				Fkey_Icon( FKeyNo2, 125 );	//	Fkey_dispN( FKeyNo2, "a+bi");
				Fkey_Icon( FKeyNo3, 126 );	//	Fkey_dispN( FKeyNo3, "r_theta");
				break;
			case SETUP_Grid: // Grid
				Fkey_Icon( FKeyNo3, 666 );	//	Fkey_dispN( FKeyNo3, "Line");
				goto onoffj;
			case SETUP_Axes: // Axes
				Fkey_Icon( FKeyNo3, 1108 );	//	Fkey_dispN( FKeyNo3, "Scale");
				goto onoffj;
			case SETUP_HidnRamInit: // HiddenRAMInit
			case SETUP_Coord: // Coord
			case SETUP_Label: // Label
			case SETUP_Derivative: // Derivative
			case SETUP_EnableExtFONT: // Enable External Font
			case SETUP_EditExtFont: // Use Ext Font (edit)
			case SETUP_EditLineNum: // Line number (edit)
			case SETUP_UseHidnRam: // UseHiddenRAM
			case SETUP_DisableDebugMode: // DisableDebugMode
			case SETUP_ExitDebugModeCheck: // ExitDebugModeCheck
			case SETUP_BreakStop: // BreakCheck
			case SETUP_IfEndCheck: // IfEnd Check
			case SETUP_ACBreak: // ACBreak Check
			case SETUP_AutoSaveMode: // Auto save
			case SETUP_Forceg1msave: // Force g1m save
			case SETUP_RecoverSetup: // Setup Mode
			case SETUP_DirectGBconvert: // Direct GB convert
			case SETUP_EnableOC: // enable auto over clock
			  onoffj:
				Fkey_Icon( FKeyNo1, 17 );	//	Fkey_dispN( FKeyNo1, " On ");
				Fkey_Icon( FKeyNo2, 18 );	//	Fkey_dispN( FKeyNo2, " Off");
				break;
			case SETUP_EditGBFONT: // Enable GB Font
				Fkey_Icon( FKeyNo1, 17 );	//	Fkey_dispN( FKeyNo1, " On ");
				Fkey_Icon( FKeyNo2, 18 );	//	Fkey_dispN( FKeyNo2, " Off");
				Fkey_dispN( FKeyNo3, "Full");
				break;
			case SETUP_ExecTimeDsp: // TimeDsp
				Fkey_Icon( FKeyNo1, 17 );	//	Fkey_dispN( FKeyNo1, " On ");
				Fkey_Icon( FKeyNo2, 18 );	//	Fkey_dispN( FKeyNo2, " Off");
				Fkey_dispN( FKeyNo3, "%HR");
				Fkey_dispN( FKeyNo4, "reset");
//				FkeyClear( FKeyNo5 );
				break;
			case SETUP_Background: // BG pict
				Fkey_Icon( FKeyNo1, 362 );	//	Fkey_dispN( FKeyNo1, "None");
//				Fkey_Icon( FKeyNo2, 183 );	//	Fkey_dispR( FKeyNo2, "PICT");
				Fkey_Icon( FKeyNo2,1155 );	//	Fkey_dispR( FKeyNo2, "PICTn");
				Fkey_Icon( FKeyNo3, 945 );	//	Fkey_dispR( FKeyNo3, "OPEN");
				break;
			case SETUP_ExpDisplay: // Exponent display type
				Fkey_DISPN( FKeyNo1, "\x0F");				// e
				Fkey_dispN( FKeyNo2, "Std\x5C\xE6\x5C\xFB\xB5");		// x10
				Fkey_dispN( FKeyNo3, "All\x5C\xE6\x5C\xFB\xB5");		// x10
				break;
			case SETUP_CMDINPUT: // Command input method
				Fkey_dispN( FKeyNo1, "CBas");
				Fkey_Icon( FKeyNo2, 1092 );	//	Fkey_dispN( FKeyNo1, "Std");
				if ( CB_fx5800P ) Fkey_dispN( FKeyNo4, ">9800"); else Fkey_dispN( FKeyNo4, ">5800");
				break;
			case SETUP_XINPUT:	 // Xinput method
				Fkey_dispN( FKeyNo1, "\x90");
				Fkey_dispN( FKeyNo2, "X");
				break;
			case SETUP_EditFontSize: // Edit Font Size
				Fkey_Icon( FKeyNo1, 1092 );	//	Fkey_dispN( FKeyNo1, "Std");
				Fkey_dispN( FKeyNo2, "Mini");
				Fkey_dispN( FKeyNo3, "MiniM");
				Fkey_dispN( FKeyNo4, "MinFX");
				mini=EditFontSize & 0x7;
				if ( (mini==2)||(mini==4)||(mini==6) )	Fkey_dispN( FKeyNo5, "\xE6\x91Std");
				else									Fkey_dispN( FKeyNo5, "\xE6\x91\x46ix");
				Fkey_dispN( FKeyNo6, "+Gap");
//				if ( EditFontSize & 0x8 )	Fkey_Icon( FKeyNo6, 1116 );	//	Fkey_dispN( FKeyNo6, "Normal");
//				else						Fkey_Icon( FKeyNo6, 1115 );	//	Fkey_dispN( FKeyNo6, "Reverse");
				break;
			case SETUP_Sketch: // S_L_ Line	normal
				FkeyS_L_();
				Fkey_Icon( FKeyNo6, 991 );	//	Fkey_dispN( FKeyNo6, "Ver.");
				break;
			case SETUP_Angle: // Angle
				Fkey_Icon( FKeyNo1, 359 );	//	Fkey_dispN( FKeyNo1, "Deg ");
				Fkey_Icon( FKeyNo2, 360 );	//	Fkey_dispN( FKeyNo2, "Rad ");
				Fkey_Icon( FKeyNo3, 361 );	//	Fkey_dispN( FKeyNo3, "Grad");
				FkeyClear( FKeyNo4 );
				break;
			case SETUP_PlotLineColor: // Plot/LineColor
			case SETUP_EditColorBack: // Edit back color
			case SETUP_EditColorBase: // Edit base color
			case SETUP_EditColorNum : // Edit Numeric color
			case SETUP_EditColorCMD:  // Edit command color
			case SETUP_EditColorQuot: // Edit quot color
			case SETUP_EditColorComt: // Edit comment color
			case SETUP_EditColorLine: // Edit Line number color
			case SETUP_FavoriteColor: // Favorite color
				Fkey_Icon( FKeyNo1, 1178);	//	Fkey_dispN( FKeyNo1, "Color");
				Fkey_dispR( FKeyNo2, "Picker");
				break;
			case SETUP_Help:	// Help
				if ( CB_HelpOn ) {
					Fkey_Icon( FKeyNo3, 1178);	//	Fkey_dispN( FKeyNo1, "Color");
					Fkey_dispR( FKeyNo4, "Picker");
				}
				goto onoffj;
				break;
			case SETUP_Display: // Display
				Fkey_Icon( FKeyNo1, 372 );	//	Fkey_dispR( FKeyNo1, "Fix ");
				Fkey_Icon( FKeyNo2, 373 );	//	Fkey_dispR( FKeyNo2, "Sci ");
				Fkey_Icon( FKeyNo3, 374 );	//	Fkey_dispR( FKeyNo3, "Norm ");
				Fkey_Icon( FKeyNo4, 375 );	//	Fkey_dispN( FKeyNo4, "Eng ");
				break;
			case SETUP_Key1sttime:	// Key Repeat mode
			case SETUP_KeyReptime:	// Key Repeat mode
			case SETUP_ExtendPict:		// Max Pict
			case SETUP_ExtendList:		// Max List
				Fkey_DISPN( FKeyNo1,"+");
				Fkey_DISPN( FKeyNo2,"-");
				Fkey_Icon( FKeyNo4,  95 );	//	Fkey_dispN( FKeyNo4, "Init");
				break;
			case SETUP_SkipUpDown: // SkipUp/Down number
			case SETUP_DefaultWaitcount: // Wait count number
				Fkey_DISPN( FKeyNo1,"+");
				Fkey_DISPN( FKeyNo2,"-");
				Fkey_dispR( FKeyNo3,"Num");
				Fkey_Icon( FKeyNo4,  95 );	//	Fkey_dispN( FKeyNo4, "Init");
				break;
			case SETUP_MatDspmode: // Mat display mode
				Fkey_dispN( FKeyNo1, "m,n");
				Fkey_dispN( FKeyNo2, "x,y");
				break;
			case SETUP_Matrixbase: // Mat base
				Fkey_dispN( FKeyNo1, "0");
				Fkey_dispN( FKeyNo2, "1");
				break;
			case SETUP_DATE: // DATE
				Fkey_dispR( FKeyNo1, "Year");
				Fkey_dispR( FKeyNo2, "Month");
				Fkey_dispR( FKeyNo3, "Day");
				break;
			case SETUP_TIME: // TIME
				Fkey_dispR( FKeyNo1, "Hour");
				Fkey_dispR( FKeyNo2, "Min");
				Fkey_dispR( FKeyNo3, "Sec");
				break;
			case SETUP_Pictmode: // Pict mode
				Fkey_dispN( FKeyNo1, "MEM");
				Fkey_dispN( FKeyNo2, "Heap");
//				Fkey_dispN( FKeyNo3, "Clear");
				Fkey_dispN( FKeyNo4, "Clear");
				break;
			case SETUP_Storagemode: // Storage mode
				Fkey_dispN( FKeyNo1, "S.MEM ");
				Fkey_dispN( FKeyNo2, "MainM");
				break;
			case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
				Fkey_Icon( FKeyNo1, 18 );	//	Fkey_dispN( FKeyNo1, "off ");
				Fkey_dispN( FKeyNo2, "Grph");
				Fkey_Icon( FKeyNo3, 920 );	//	Fkey_dispN( FKeyNo3, "All");
				if ( RefreshCtrl ) Fkey_Icon( FKeyNo4, 298 );	// Fkey_dispR( FKeyNo4, "TIME");
				Fkey_Icon( FKeyNo5,  95 );	//	Fkey_dispN( FKeyNo5, "Init");
				break;
			case SETUP_Executemode: // Execute Mode
				Fkey_dispN( FKeyNo1, "DBL#");
				Fkey_dispN( FKeyNo2, "INT%");
				Fkey_dispN( FKeyNo3, "CPLX");
				break;
			case SETUP_EditListChar:
				Fkey_dispN( FKeyNo1, "List");
				Fkey_dispN( FKeyNo2, "\x5C\xE5\x5C\xB7");
				Fkey_dispN( FKeyNo3, "\xFF\xE0");
				break;
			case SETUP_ForceReturnMode:
				Fkey_Icon( FKeyNo1, 362 );	//	Fkey_dispN( FKeyNo1, "None");
				Fkey_dispN( FKeyNo2, "F1");
				Fkey_dispN( FKeyNo3, "EXE");
				Fkey_dispN( FKeyNo4, "Both");
				break;
			case SETUP_G1MorG3M: // G1M/G3M Mode
				Fkey_Icon( FKeyNo1, 53 );	//	Fkey_dispN( FKeyNo1, "Auto");
				Fkey_dispN( FKeyNo2, "g1m");
				Fkey_dispN( FKeyNo3, "g3m");
//				if ( CB_G3M_TEXT )	Fkey_dispN( FKeyNo4, "Text1");
//				else			 	Fkey_dispN( FKeyNo4, "Text3");
				break;
			case SETUP_MaxHeapRam:	// Max Heap Ram size
				Fkey_dispN( FKeyNo1, "96KB");
				Fkey_dispN( FKeyNo2, "117KB");
				Fkey_dispN( FKeyNo3, "127KB");
				Fkey_Icon( FKeyNo4,  95 );	//	Fkey_dispN( FKeyNo5, "Init");
//				Fkey_Icon( FKeyNo1,  79 );	//		Fkey_dispN( FKeyNo2, "MIN");
//				Fkey_Icon( FKeyNo2,  78 );	//		Fkey_dispN( FKeyNo1, "MAX");
//				Fkey_Icon( FKeyNo2, 170 );	//		Fkey_dispR( FKeyNo1, "Max");
				Fkey_Icon( FKeyNo5, 790 );	//		Fkey_dispR( FKeyNo3, "SIZE");
				break;
			case SETUP_RootFolder:
				Fkey_dispN( FKeyNo1, "  / ");
				Fkey_dispN( FKeyNo2, "cur.f");
				break;
			case SETUP_EditIndent: //
				Fkey_Icon( FKeyNo1, 18 );	//	Fkey_dispN( FKeyNo1, "off ");
				Fkey_dispN( FKeyNo2, " 1");
				Fkey_dispN( FKeyNo3, " 2");
				Fkey_dispN( FKeyNo4, " 4");
				Fkey_dispN( FKeyNo5, "Save-");
				break;
			case SETUP_FuncType: //
				switch ( subselect ) {
					case 0:
						Fkey_Icon( FKeyNo1,  61 );	//	Fkey_dispN( FKeyNo1, "Y=");
						Fkey_Icon( FKeyNo2,  62 );	//	Fkey_dispN( FKeyNo2, "r=");
						Fkey_Icon( FKeyNo3,  63 );	//	Fkey_dispN( FKeyNo3, "Param");
						Fkey_Icon( FKeyNo4, 723 );	//	Fkey_dispN( FKeyNo4, "X=");
						FkeyClear( FKeyNo5 );
						Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
						break;
					case 1:
						Fkey_Icon( FKeyNo1,  65 );	//	Fkey_dispN( FKeyNo1, "Y>");
						Fkey_Icon( FKeyNo2,  66 );	//	Fkey_dispN( FKeyNo2, "Y<");
						Fkey_Icon( FKeyNo3,  67 );	//	Fkey_dispN( FKeyNo3, "Y>=");
						Fkey_Icon( FKeyNo4,  68 );	//	Fkey_dispN( FKeyNo4, "Y<=");
						FkeyClear( FKeyNo5 );
						Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
						break;
					case 2:
						Fkey_Icon( FKeyNo1, 724 );	//	Fkey_dispN( FKeyNo1, "X>");
						Fkey_Icon( FKeyNo2, 725 );	//	Fkey_dispN( FKeyNo2, "X<");
						Fkey_Icon( FKeyNo3, 726 );	//	Fkey_dispN( FKeyNo3, "X>=");
						Fkey_Icon( FKeyNo4, 727 );	//	Fkey_dispN( FKeyNo4, "X<=");
						FkeyClear( FKeyNo5 );
						Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
						break;
				}
				break;
			default:
				break;
		}
		
		if ( ( limit ) && ( ( select==SETUP_ExtendList ) || ( select==SETUP_MaxHeapRam ) ) ) goto mask;
		if ( ( limit ) || ( IsCG20 ) || ( select==SETUP_HidnRamInit ) ) {
			switch ( select ) {
				case SETUP_UseHidnRam:		// Hidden RAM
					if ( IsHiddenRAM ) break;
				case SETUP_ExtendPict:		// Max Pict
				case SETUP_HidnRamInit: 	// HiddenRAMInit
				  mask:
					FkeyMask( FKeyNo1 );
					FkeyMask( FKeyNo2 );
					FkeyMask( FKeyNo3 );
					FkeyMask( FKeyNo4 );
					FkeyMask( FKeyNo5 );
					break;
			}
		}
//		Bdisp_PutDisp_DD();

		DateCursorY = y*0x100+(DateCursorY&0xFF);
		TimeCursorY = y*0x100+(TimeCursorY&0xFF);
		StatusArea_Time();
		EnableDisplayStatusArea();
		TimePrintSetMode( 1 ) ;			// Date/Time print IRQ on
		GetKey_DisableMenuCatalog(&key);
		TimePrintSetMode( 0 ) ;			// Date/Time print IRQ off
//		EnableStatusArea(3);	// disable StatusArea
		DateToStr(DateStr);
		TimeToStr(TimeStr);
		switch (key) {
//			case KEY_CTRL_SHIFT:
//				GetKey_DisableMenu(&key);
//				break;
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;

			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=(listmax); scrl=select-6;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > (listmax) ) {select=0; scrl=0;}
				if ((select - scrl) > 6 ) scrl+=1;
				if ( scrl > (listmax) ) scrl=(listmax)-6;
				break;
			case KEY_CTRL_LEFT:
				select=0;
				scrl=0;
				break;
			case KEY_CTRL_RIGHT:
				select=(listmax);
				scrl=select-6;
				break;

			case KEY_CTRL_PAGEUP:
				for ( i=0; i<7; i++) {
					select--;
					if ( select < 0 ) {select=0; scrl=0; break;}
					if ( select < scrl ) scrl-=1;
					if ( scrl < 0 ) scrl=0;
				}
				break;
			case KEY_CTRL_PAGEDOWN:
				for ( i=0; i<7; i++) {
					select++;
					if ( select > (listmax) ) {select=listmax; scrl=select-6; break; }
					if ((select - scrl) > 6 ) scrl+=1;
					if ( scrl > (listmax) ) scrl=(listmax)-6;
				}
				break;

			case KEY_CTRL_F1:
//				Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
				switch (select) {
					case SETUP_Angle: // Angle
						Angle = 0 ; // Deg
						break;
					case SETUP_ComplexMode: // complex mode
						ComplexMode = 0;	// Real
						break;
					case SETUP_Variable:
						VarListRange=0;
						break;
					case SETUP_FuncType: // function Type
						switch ( subselect ) {
							case 0:
								FuncType = 0;
								break;
							case 1:
								FuncType = 4;
								break;
							case 2:
								FuncType = 8;
								break;
						}
						break;
					case SETUP_DrawType: // Draw Type connect
						DrawType = 0 ;
						break;
					case SETUP_Coord: // Coord	on
						Coord = 1 ;
						break;
					case SETUP_Grid: // Grid		on
						Grid = 1 ;
						break;
					case SETUP_Axes: // Axes		on
						Axes = 1 ;
						break;
					case SETUP_Label: // Label	on
						Label = 1 ;
						break;
					case SETUP_Derivative: // 	Derivative on
						Derivative = 1 ;
						break;
					case SETUP_Background: // BG pict
						BG_Pict_No = 0 ;
						BG_Buf = NULL ;
						break;
					case SETUP_Sketch: // Sketch Line	normal
						S_L_Style = S_L_Normal ;
						break;
					case SETUP_Display: // Display
						i = SelectNum1("Fix",CB_Round.DIGIT,0,15,&key);
						if ( key==KEY_CTRL_EXIT ) break;
						CB_Round.DIGIT=i;
						CB_Round.MODE =Fix;
						break;
					case SETUP_ExpDisplay: // Exponent display type
						CB_Round.ExpTYPE = 0;	// e
						break;
					case SETUP_Help:	// Help
						CB_HelpOn = 1;	// help on
						break;
					case SETUP_RecoverSetup: // Setup Mode
						CB_RecoverSetup = 1 ; // recover on
						break;
					case SETUP_EditGBFONT: // Enable GB Font
						EditGBFont = 1;
						GBcode = 1;
						break;
					case SETUP_EnableExtFONT: // Enable External Font
						EnableExtFont  = 1;
						FileListUpdate = 1;
						break;
					case SETUP_PlotLineColor: //
						CB_ColorIndexPlot=GetColorIndex(CB_ColorIndexPlot);
						break;
					case SETUP_EditColorBack: // Edit back color
						CB_ColorIndexEditBack=GetColorIndex(CB_ColorIndexEditBack);
						break;
					case SETUP_EditColorBase: // Edit base color
						CB_ColorIndexEditBase=GetColorIndex(CB_ColorIndexEditBase);
						break;
					case SETUP_EditColorNum:  // Edit Numeric color
						CB_ColorIndexEditNum=GetColorIndex(CB_ColorIndexEditNum);
						break;
					case SETUP_EditColorCMD:  // Edit command color
						CB_ColorIndexEditCMD=GetColorIndex(CB_ColorIndexEditCMD);
						break;
					case SETUP_EditColorQuot: // Edit quot color
						CB_ColorIndexEditQuot=GetColorIndex(CB_ColorIndexEditQuot);
						break;
					case SETUP_EditColorComt: // Edit comment color
						CB_ColorIndexEditComt=GetColorIndex(CB_ColorIndexEditComt);
						break;
					case SETUP_EditColorLine: // Edit Line number color
						CB_ColorIndexEditLine=GetColorIndex(CB_ColorIndexEditLine);
						break;
					case SETUP_FavoriteColor: // Favorite color
						CB_FavoriteColorIndex=GetColorIndex(CB_FavoriteColorIndex);
						break;
					case SETUP_CMDINPUT:	// Command input method
						CommandInputMethod=0;
						break;
					case SETUP_XINPUT: 		// X input method
						XInputMethod=0;
						break;
					case SETUP_EditIndent: //
						i = CB_EditIndent&0x8;
						CB_EditIndent = 0 | i;
						break;
					case SETUP_EditExtFont: // Use Ext Font (edit)
						if ( EnableExtFont ) EditExtFont=1;
						break;
					case SETUP_EditFontSize: // Edit Font Size
						EditFontSize &= 0xF8;	// Standard
						UpdateLineNum=1;
						break;
					case SETUP_EditLineNum: // Line number (edit)
						EditFontSize |= 0x10;	// on
						break;
					case SETUP_EditListChar:
						EditListChar=0;		// normal
						break;
					case SETUP_ExtendPict:		// Max Pict
						if ( limit ) break;
//						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
//						ExtendPict+=10;
//						if ( 79<ExtendPict ) ExtendPict=79;
//						HiddenRAM_MatAryInit();
						break;
					case SETUP_ExtendList:		// Max List
						if ( limit ) break;
//						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						ExtendList++;
						if ( IsCG20 ) {
							if (  5<ExtendList ) ExtendList=5;
						} else {
							if ( 19<ExtendList ) ExtendList=19;
						}
						HiddenRAM_MatAryInit();
						break;
					case SETUP_EnableOC: // enable auto over clock
						CB_disableOC = 0;	// on
						break;
					case SETUP_MaxHeapRam:	// Max Heap Ram size
						if ( limit ) break;
						HeapRAM_malloc( MAXHEAP2 );	// 96KB
						break;
					case SETUP_UseHidnRam: // Hidden RAM
						if ( limit ) break;
						if ( IsHiddenRAM ) UseHiddenRAM = 1 ; // on
						if ( YesNo("Initialize Ok?")==0 ) break;
						ExtendList=(6-1);
						HiddenRAM_MatAryClear();
						break;
					case SETUP_HidnRamInit: // HiddenRAMInit
						if ( limit ) break;
						if ( UseHiddenRAM ) UseHiddenRAM &= 0x0F;	// on
						HiddenRAM_MatAryClear();
						break;
					case SETUP_DisableDebugMode: // DisableDebugMode
						DisableDebugMode = 0 ; // on
						break;
					case SETUP_ExitDebugModeCheck: // ExitDebugModeCheck
						ExitDebugModeCheck = 1 ; // on
						break;
					case SETUP_BreakStop: // Break
						BreakCheck = 1 ; // on
						BreakCheckDefault = 1 ; // on
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						TimeDsp = 1 ; // on
						break;
					case SETUP_IfEndCheck: // IfEnd Check
						CheckIfEnd = 1 ; // on
						break;
					case SETUP_ACBreak: // ACBreak
						ACBreak = 1 ; // on
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 0;
						break;
					case SETUP_Key1sttime: // Key Repeat First Count *ms
						KeyRepeatFirstCount += 1 ;
						if ( KeyRepeatFirstCount > 40 ) KeyRepeatFirstCount=40;
						break;
					case SETUP_KeyReptime: // Key Repeat Next Count *ms
						KeyRepeatNextCount += 1 ;
						if ( KeyRepeatNextCount > 20 ) KeyRepeatNextCount=20;
						break;
					case SETUP_SkipUpDown: // Skipup/down count +
						PageUpDownNum++; if ( PageUpDownNum > 9999 ) PageUpDownNum = 9999;
						break;
					case SETUP_MatDspmode: // Matrix Display mode
						MatXYmode = 0 ; // m,n
						break;
					case SETUP_Matrixbase: // Matrix base
						MatBaseDefault = 0 ; //
						MatBase = MatBaseDefault;
						break;
					case SETUP_DATE: // DATE year
						year = (DateStr[0]-'0')*1000+(DateStr[1]-'0')*100+(DateStr[2]-'0')*10+(DateStr[3]-'0');
						year = SelectNum2("Year",year,0,9999);
						DateStr[0]=(year/1000)+'0';
						DateStr[1]=(year/100)%10+'0';
						DateStr[2]=(year/10)%10+'0';
						DateStr[3]=(year)%10+'0';
						StorDATE( DateStr );
						break;
					case SETUP_TIME: // Time hour
						hour = (TimeStr[0]-'0')*10+(TimeStr[1]-'0');
						hour = SelectNum2("Hour",hour,0,23);
						TimeStr[0]=(hour/10)%10+'0';
						TimeStr[1]=(hour)%10+'0';
						StorTIME( TimeStr );
						break;
					case SETUP_RootFolder:
						if ( StorageMode == 2 ) break;
						root2[0]='\0';	// default root folder
						folder[0]='\0';
						FileListUpdate= 1;
						break;
					case SETUP_AutoSaveMode: // Auto save
						AutoSaveMode = 1;
						break;
					case SETUP_Forceg1msave: // Force g1m save
						ForceG1Msave = 1 ; // g1m and text
						break;
					case SETUP_DirectGBconvert: // Direct GB convert
						DirectGBconvert = 1 ; // on
						break;
					case SETUP_Pictmode: // Pict mode
						PictMode = 0 ; // strage Memory mode
						break;
					case SETUP_Storagemode: // Storage mode
						ChangeStorageMode( 0 ) ; // Storage Memory mode (default)
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						RefreshCtrl = 0 ; // off
						break;
					case SETUP_DefaultWaitcount: // Wait count +
						DefaultWaitcount+=10; if ( DefaultWaitcount > 9999 ) DefaultWaitcount = 9999;
						Waitcount=DefaultWaitcount;
						break;
					case SETUP_Executemode: // CB mode
						CB_INTDefault = 0 ; // normal
						CB_INT = CB_INTDefault;
						ComplexMode = 0;	// real
						break;
					case SETUP_G1MorG3M: // g1m/g3m mode
						CB_G1MorG3MDefault = 0 ; // auto
						CB_G3M_TEXT = 0;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F2:
//				Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
				switch (select) {
					case SETUP_Angle: // Angle
						Angle = 1 ; // Rad
						break;
					case SETUP_ComplexMode: // complex mode
						if ( CB_INTDefault == 2 ) ComplexMode = 1;	// a+bi
						break;
					case SETUP_Variable:
						i = VarListRange; if ( i==0 ) i=1;
						i = SelectNum1sub("Select List No.","List",i,0,ExtListMax,&key);
						if ( key==KEY_CTRL_EXIT ) break;
						VarListRange = i;
						break;
					case SETUP_FuncType: // function Type
						switch ( subselect ) {
							case 0:
								FuncType = 1;
								break;
							case 1:
								FuncType = 5;
								break;
							case 2:
								FuncType = 9;
								break;
						}
						break;
					case SETUP_DrawType: // Draw Type Plot
						DrawType = 1 ;
						break;
					case SETUP_Coord: // Coord	off
						Coord = 0 ;
						break;
					case SETUP_Grid: // Grid		off
						Grid = 0 ;
						break;
					case SETUP_Axes: // Axes		off
						Axes = 0 ;
						break;
					case SETUP_Label: // Label	off
						Label = 0 ;
						break;
					case SETUP_Derivative: // 	Derivative off
						Derivative = 0 ;
						break;
					case SETUP_Background: // BG pict
						i = SelectNum2("Pict",BG_Pict_No,1,20);
						Change_BG_Pict( i );
						break;
					case SETUP_Sketch: // Sketch Line	Thick
						S_L_Style = S_L_Thick ;
						break;
					case SETUP_Display: // Display
						i = SelectNum1("Sci",CB_Round.DIGIT,0,15,&key);
						if ( key==KEY_CTRL_EXIT ) break;
						CB_Round.DIGIT=i;
						CB_Round.MODE =Sci;
						break;
					case SETUP_ExpDisplay: // Exponent display type
						CB_Round.ExpTYPE = 1;	// x10 Standard font only
						break;
					case SETUP_Help:	// Help
						CB_HelpOn = 0;	// help off
						break;
					case SETUP_RecoverSetup: // Setup Mode
						CB_RecoverSetup = 0 ; // recover off
						break;
					case SETUP_EditGBFONT: // Disable GB Font
						EditGBFont = 0;
						GBcode = 0;
						break;
					case SETUP_EnableExtFONT: // Disable External Font
						EnableExtFont = 0;
						EditExtFont   = 0;
						FileListUpdate= 1;
						break;
					case SETUP_PlotLineColor: //
						us=CB_ColorIndexPlot;
						SetRGBColor(&us);
						CB_ColorIndexPlot=us;
						break;
					case SETUP_EditColorBack: // Edit back color
						SetRGBColor(&CB_ColorIndexEditBack);
						break;
					case SETUP_EditColorBase: // Edit base color
						SetRGBColor(&CB_ColorIndexEditBase);
						break;
					case SETUP_EditColorNum:  // Edit Numeric color
						SetRGBColor(&CB_ColorIndexEditNum);
						break;
					case SETUP_EditColorCMD:  // Edit command color
						SetRGBColor(&CB_ColorIndexEditCMD);
						break;
					case SETUP_EditColorQuot: // Edit quot color
						SetRGBColor(&CB_ColorIndexEditQuot);
						break;
					case SETUP_EditColorComt: // Edit comment color
						SetRGBColor(&CB_ColorIndexEditComt);
						break;
					case SETUP_EditColorLine: // Edit Line number color
						SetRGBColor(&CB_ColorIndexEditLine);
						break;
					case SETUP_FavoriteColor: // Favorite color
						SetRGBColor(&CB_FavoriteColorIndex);
						break;
					case SETUP_CMDINPUT: // Command input method
						CommandInputMethod=1;
						break;
					case SETUP_EditIndent: //
						i = CB_EditIndent&0x8;
						CB_EditIndent = 1 | i;
						break;
					case SETUP_XINPUT: 		// X input method
						XInputMethod=1;
						break;
					case SETUP_EditExtFont: // Use Ext Font (edit)
						EditExtFont=0;
						break;
					case SETUP_EditFontSize: // Edit Font Size
						EditFontSize &= 0xF8;	// Standard
						EditFontSize |= 0x01;	// mini
						UpdateLineNum=1;
						break;
					case SETUP_EditLineNum: // Line number (edit)
						EditFontSize &= 0x0F;	// off (default)
						UpdateLineNum=1;
						break;
					case SETUP_EditListChar:
						EditListChar=1;		//
						break;
					case SETUP_ExtendPict:		// Max Pict
						if ( limit ) break;
						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						if (ExtendPict>70) ExtendPict=70; else ExtendPict-=10;
						if (ExtendPict<0) ExtendPict=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_ExtendList:		// Max List
						if ( limit ) break;
//						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						ExtendList--;
						if (ExtendList<0) ExtendList=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_EnableOC: // enable auto over clock
						CB_disableOC = 1;	// off
						break;
					case SETUP_MaxHeapRam:	// Max Heap Ram size
						if ( limit ) break;
						HeapRAM_malloc( MAXHEAP1 );	// 117KB
						break;
					case SETUP_UseHidnRam: // Hidden RAM
						if ( limit ) break;
						if ( YesNo("Initialize Ok?")==0 ) break;
						UseHiddenRAM = 0 ; // off
						ExtendPict=0;
						ExtendList=(6-1);
						HiddenRAM_MatAryInit();
						break;
					case SETUP_HidnRamInit: // HiddenRAMInit
						if ( limit ) break;
//						if ( UseHiddenRAM ) UseHiddenRAM |= 0x10;	// off
//						HiddenRAM_MatAryClear();
						break;
					case SETUP_DisableDebugMode: // DisableDebugMode
						DisableDebugMode = 1 ; // disable
						break;
					case SETUP_ExitDebugModeCheck: // ExitDebugModeCheck
						ExitDebugModeCheck = 0 ; // off
						break;
					case SETUP_BreakStop: // Break
						BreakCheck = 0 ; // off
						BreakCheckDefault = 0 ; // off
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 1;
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						TimeDsp = 0 ; // off
						break;
					case SETUP_IfEndCheck: // IfEnd Check
						CheckIfEnd = 0 ; // off
						break;
					case SETUP_ACBreak: // ACBreak
						ACBreak = 0 ; // off
						break;
					case SETUP_Key1sttime: // Key Repeat First Count *ms
						KeyRepeatFirstCount -= 1 ;
						if ( KeyRepeatFirstCount < 1 ) KeyRepeatFirstCount=1;
						break;
					case SETUP_KeyReptime: // Key Repeat Next Count *ms
						KeyRepeatNextCount -= 1 ;
						if ( KeyRepeatNextCount < 1 ) KeyRepeatNextCount=1;
						break;
					case SETUP_SkipUpDown: // Skipup/down count -
						PageUpDownNum--; if ( PageUpDownNum < PageUpDownNumDefault ) PageUpDownNum = PageUpDownNumDefault;
						break;
					case SETUP_MatDspmode: // Matrix display mode
						MatXYmode = 1 ; // x,y
						break;
					case SETUP_Matrixbase: // Matrix base
						MatBaseDefault = 1 ; // base
						MatBase = MatBaseDefault;
						break;
					case SETUP_Pictmode: // Pict mode
						PictMode = 1 ; // heap mode
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						RefreshCtrl = 1 ; // graphics
						break;
					case SETUP_DATE: // DATE month
						month = (DateStr[5]-'0')*10+(DateStr[6]-'0');
						month = SelectNum2("Month",month,1,12);
						DateStr[5]=(month/10)%10+'0';
						DateStr[6]=(month)%10+'0';
						StorDATE( DateStr );
						break;
					case SETUP_TIME: // Time min
						min = (TimeStr[3]-'0')*10+(TimeStr[4]-'0');
						min = SelectNum2("Min",min,0,59);
						TimeStr[3]=(min/10)%10+'0';
						TimeStr[4]=(min)%10+'0';
						StorTIME( TimeStr );
						break;
					case SETUP_RootFolder:
						if ( StorageMode == 2 ) break;
						if ( strlen( folderbuf ) ) {
							root2[0]='\\';
							strcpy( root2+1, folderbuf );	// current folder
						} else root2[0]='\0';	// root folder
						folder[0]='\0';
						FileListUpdate= 1;
						break;
					case SETUP_AutoSaveMode: // Auto save
						AutoSaveMode = 0;
						break;
					case SETUP_Forceg1msave: // Force g1m save
						ForceG1Msave = 0 ; // text only
						break;
					case SETUP_DirectGBconvert: // Direct GB convert
						DirectGBconvert = 0 ; // off
						break;
					case SETUP_Storagemode: // Storage mode
						ChangeStorageMode( 2 );	// main memory
//						StorageMode = CheckSD() ; // SD mode
						break;
					case SETUP_DefaultWaitcount: // Wait count -
						DefaultWaitcount-=10; if ( DefaultWaitcount < 0 ) DefaultWaitcount = 0;
						Waitcount=DefaultWaitcount;
						break;
					case SETUP_Executemode: // CB mode
						CB_INTDefault = 1 ; // int
						CB_INT = CB_INTDefault;
						ComplexMode = 0;	// real
						break;
					case SETUP_G1MorG3M: // g1m/g3m mode
						CB_G1MorG3MDefault = 1 ; // g1m  fx mode
						CB_G1MorG3M = CB_G1MorG3MDefault ;
						SetG1MorG3M( CB_G1MorG3M );
						CB_G3M_TEXT = 0;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F3:
//				Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
				switch (select) {
					case SETUP_FuncType: // function Type
						switch ( subselect ) {
							case 0:
								FuncType = 2;
								break;
							case 1:
								FuncType = 6;
								break;
							case 2:
								FuncType =10;
								break;
						}
						break;
					case SETUP_Angle: // Angle
						Angle = 2 ; // Grad
						break;
					case SETUP_ComplexMode: // complex mode
						if ( CB_INTDefault == 2 ) ComplexMode = 2;	// r_theta
						break;
					case SETUP_Grid: // Grid		Line
						Grid = 2 ;
						break;
					case SETUP_Axes: // Axes		Scale
						Axes = 2 ;
						break;
					case SETUP_Background: // BG pict
						i = BG_OpenFileDialog( BG_filename );
						if ( i ) Change_BG_Pict( 21 );
						break;
					case SETUP_Sketch: // Sketch Line	Broken
						S_L_Style = S_L_Broken ;
						break;
					case SETUP_Display: // Display
						i = SelectNum1("Norm",CB_Round.DIGIT,0,15,&key);
						if ( key==KEY_CTRL_EXIT ) break;
						CB_Round.DIGIT=i;
						CB_Round.MODE =Norm;
						break;
					case SETUP_Help:	// Help
						if ( CB_HelpOn ) CB_HelpFrameColorIndex=GetColorIndex(CB_HelpFrameColorIndex);
						break;
					case SETUP_EditIndent: //
						i = CB_EditIndent&0x8;
						CB_EditIndent = 2 | i;
						break;
					case SETUP_ExpDisplay: // Exponent display type
						CB_Round.ExpTYPE = 2;	// x10 All font
						break;
					case SETUP_EditGBFONT: // Enable Full GB Font
						EditGBFont = 2;
						GBcode = 2;
						break;
					case SETUP_EditFontSize: // Edit Font Size minimini
						mini=EditFontSize & 0x7;
						switch ( mini ) {
							case 3:	// minimini
							case 4:	// minimini fix
								EditFontSize+=2;	// -> Bold
								break;
							case 5:	// minimini Bold
							case 6: // minimini Bold fix
								EditFontSize-=2;	// ->  not Bold
								break;
							default:
								EditFontSize &= 0xF8;	// Standard
								EditFontSize |= 0x03;	// minimini
								break;
						}
						UpdateLineNum=1;
						break;
					case SETUP_EditListChar:
						EditListChar=2;		// reverse L
						break;
					case SETUP_MaxHeapRam:	// Max Heap Ram size
						if ( limit ) break;
						HeapRAM_malloc( MAXHEAP );	// 127KB
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						if ( TimeDsp == 0 ) break;
						if ( TimeDsp & 0x4 )    TimeDsp &= ~0x4 ; // hires timer off
						else					TimeDsp |=  0x4 ; // hires timer on
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 2;
						break;
					case SETUP_SkipUpDown: // Skipup/down count init
						PageUpDownNum = SelectNum3( PageUpDownNum );
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						RefreshCtrl = 2 ; // graphics+text
						break;
					case SETUP_Pictmode: // Pict mode
//						PictMode = 2 ; // Memory(read) & Smem(save) mode
						break;
					case SETUP_DATE: // DATE day
						day = (DateStr[8]-'0')*10+(DateStr[9]-'0');
						day = SelectNum2("Day",day,1,31);
						DateStr[8]=(day/10)%10+'0';
						DateStr[9]=(day)%10+'0';
						StorDATE( DateStr );
						break;
					case SETUP_TIME: // Time sec
						sec = (TimeStr[6]-'0')*10+(TimeStr[7]-'0');
						sec = SelectNum2("Sec",sec,0,59);
						TimeStr[6]=(sec/10)%10+'0';
						TimeStr[7]=(sec)%10+'0';
						StorTIME( TimeStr );
						break;
					case SETUP_DefaultWaitcount: // Wait count set
						DefaultWaitcount =  SelectNum2( "Wait", DefaultWaitcount, 0, 9999);
						Waitcount=DefaultWaitcount;
						break;
					case SETUP_G1MorG3M: // g1m/g3m mode
						CB_G1MorG3MDefault = 3 ; // g3m  CG mode
						CB_G1MorG3M = CB_G1MorG3MDefault ;
						SetG1MorG3M( CB_G1MorG3M );
						break;
					case SETUP_Executemode: // CB mode
						CB_INTDefault = 2 ; // complex
						CB_INT = CB_INTDefault;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F4:
//				Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
				switch (select) {
					case SETUP_FuncType: // function Type
						switch ( subselect ) {
							case 0:
								FuncType = 3;
								break;
							case 1:
								FuncType = 7;
								break;
							case 2:
								FuncType =11;
								break;
						}
						break;
					case SETUP_Sketch: // Sketch  Line	Dot
						S_L_Style = S_L_Dot ;
						break;
					case SETUP_Display: // Display
						ENG++;
						if ( ENG>3 ) ENG=0;
						if ( ENG>1 ) ENG=3;
						break;
					case SETUP_CMDINPUT: // Command input method (fx5800P mode change)
						CB_fx5800P = 1-CB_fx5800P;
						break;
					case SETUP_EditIndent: //
						i = CB_EditIndent&0x8;
						CB_EditIndent = 4 | i;
						break;
					case SETUP_Help:	// Help
						if ( CB_HelpOn ) SetRGBColor(&CB_HelpFrameColorIndex);
						break;
					case SETUP_EditFontSize: // Edit Font Size minimini fx 6x8
						EditFontSize &= 0xF8;	// Standard
						EditFontSize |= 0x07;	// mini mini fx
						break;
					case SETUP_ExtendPict:		// Max Pict
						if ( limit ) break;
						ExtendPict=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_ExtendList:		// Max List
						if ( limit ) break;
						ExtendList=0;
						ExtListMax=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_MaxHeapRam:	// Max Heap Ram size
						if ( limit ) break;
						HeapRAM_malloc( MAXHEAPDEFAULT );	// 86KB
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						if ( TimeDsp == 0 ) break;
						if ( TimeDsp & 0x2 )    TimeDsp &= ~0x2 ; // off reset GetKey1/2
						else					TimeDsp |=  0x2 ; // on  reset GetKey1/2
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 3;
						break;
					case SETUP_Key1sttime: // Key Repeat First Count *ms
						KeyRepeatFirstCount = 20 ;
						break;
					case SETUP_KeyReptime: // Key Repeat Next Count *ms
						KeyRepeatNextCount  = 5 ;
						break;
					case SETUP_SkipUpDown: // Skipup/down count init
						PageUpDownNum = PageUpDownNumDefault ;
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						if ( RefreshCtrl ) Refreshtime = SelectNum4( Refreshtime+1 )-1;
						break;
					case SETUP_Pictmode: // Pict mode
						CB_ClrPict = 1-CB_ClrPict;
						break;
					case SETUP_DefaultWaitcount: // Wait count init
						DefaultWaitcount = 0;
						Waitcount=DefaultWaitcount;
						break;
					case SETUP_G1MorG3M: // g1m/g3m mode
						if ( CB_G1MorG3M ==3 ) {
							if ( CB_G3M_TEXT )	CB_G3M_TEXT = 0;
							else				CB_G3M_TEXT = 1;
						}
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F5:
//				Bdisp_AreaReverseVRAMx3(0, y*8, 125, y*8+7);	// reverse select line
				switch (select) {
					case SETUP_EditIndent: //
						CB_EditIndent^=0x08;
						break;
					case SETUP_Sketch: // Sketch  Line	Thin
						S_L_Style = S_L_Thin ;
						break;
					case SETUP_EditFontSize: // Edit Font Size
						mini=EditFontSize & 0x07;
						switch ( mini ) {
							case 1:	// mini
							case 3:	// minimini
							case 5:	// minimini Bold
								EditFontSize++;	// -> Fix
								break;
							case 2:	// mini fix
							case 4:	// minimini fix
							case 6: // minimini Bold fix
								EditFontSize--;	// ->  not Fix
								break;
						}
						UpdateLineNum=1;
						break;
					case SETUP_MaxHeapRam:	// Max Heap Ram size
						if ( limit ) break;
						MaxHeapRam =  SelectNum2( "Free", MaxHeapRam, MINHEAP, MAXHEAP );
						HeapRAM_malloc( MaxHeapRam );
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode init
						RefreshCtrl = 1 ; // graphics only
						Refreshtime = 3-1 ; // 3/128
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F6:
				switch (select) {
					case SETUP_FuncType: // function Type
						switch ( subselect ) {
							case 0:
								subselect = 1;
								break;
							case 1:
								subselect = 2;
								break;
							case 2:
								subselect = 0;
								break;
						}
						break;
					case SETUP_EditFontSize: // Edit Font Size +Gap
						if ( EditFontSize & 0x08 )	EditFontSize -= 0x08;	// ->normal
						else 						EditFontSize += 0x08;	// ->+Gap
						break;
					default:
						VerDisp();
						break;
				}
				break;
			default:
				break;
		}
		Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep
	}
	CB_ColorIndex=coloetmp;
	CB_BackColorIndex=backcoloetmp;
	SaveConfig();
	RestoreDisp(SAVEDISP_PAGE1);
	return select;
}