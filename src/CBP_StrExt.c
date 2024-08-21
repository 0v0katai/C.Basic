

#include "prizm.h"
#include "CBP.h"

char ReEnterSEE=0;

const char TypeTitle[][10]={"Strings  ","Function ","Graphics "};
const char TypeStr[][2]={"S","f","Y"};
	
int SelectMemoryNum( int type, int max, int store, int *key ) {		//
	char buffer[32];
	int n=0,n0=1;
	SaveDisp(SAVEDISP_PAGE2);
	PopUpWin(4);
	FkeyClearAll();
	locate( 3,2); if ( store ) Prints((unsigned char *)"Store In"); else Prints((unsigned char *)"Recall From");
	locate( 5,3); Prints((unsigned char *)TypeTitle[type]);
	locate(14,3); Prints((unsigned char *)"Memory");
	locate( 3,5); sprintf(buffer,"%s[%d~%d]:",TypeStr[type],1,max); Prints((unsigned char *)buffer);
	buffer[0]='\0';
	while (1) {
		n=InputNumD(13+strlen(buffer), 5, log10(max)+1, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &(*key));		// 0123456789
		if ( (*key) == KEY_CTRL_EXIT ) break;
		if ( n == n0 ) break;
		if ( (1<=n)&&(n<=max) ) break;
		n=n0;
	}
	RestoreDisp(SAVEDISP_PAGE2);
	return n ; // ok
}

void LoadStringType( int type, int *reg, int *opNum ){
	switch ( type ) {
		case 0:	// string
			*reg=defaultStrAry;
			*opNum=defaultStrAryN;
			break;
		case 1:	// fn
			*reg=defaultFnAry;
			*opNum=defaultFnAryN;
			break;
		case 2:	// GraphY
			*reg=defaultGraphAry;
			*opNum=defaultGraphAryN;
			break;
	}
}
char* GetStringPtr(int type, int n ){			// ----------- Store  String  type: 0:string  1:fn   2:GraphY
	unsigned int key;
	int reg,dimA,dimB;
	int opNum;
	char *str;

	LoadStringType( type, &reg, &opNum );
	if ( n>opNum ) return NULL;
	ErrorNo=0;
	switch ( type ) {
		case 0:	// string
			str = GetStrYFnPtrSub( reg, n, defaultStrArySize  );
			break;
		case 1:	// fn
			str = GetStrYFnPtrSub( reg, n, defaultFnArySize  );
			break;
		case 2:	// GraphY
			str = GetStrYFnPtrSub( reg, n, defaultGraphArySize  );
			break;
	}
	if ( ErrorNo ) return NULL; // error
	return str;
}

char* RecallStringSub( int reg, int select ) {
	char *str;
	if ( ( MatAry[reg].SizeA < select ) ) return NULL;
	str = MatrixPtr( reg, select, 1 ) ; 
	if ( str[0]=='\0' ) return NULL;
	return str;
}

char* CB_RecallString(int type ){			// ----------- Recall String  type: 0:string  1:fn   2:GraphY
	int key;
	int reg,dimA;
	int opNum,n;
	char *string;
	LoadStringType( type, &reg, &opNum );
	dimA = MatAry[reg].SizeA ;
	n = SelectMemoryNum( type, opNum, 0, &key );
	if ( key == KEY_CTRL_EXIT ) return 0;
	if ( dimA == 0 ) {
		string = NULL;
	} else { 
		if ( dimA >= n ) {
			string = RecallStringSub(reg, n );
		} else string = NULL;
	}
	return string;
}


int CB_StoreStringSub(int type, int n, char *clipbuffer ){			// ----------- Store  String  type: 0:string  1:fn   2:GraphY
	int reg,dimA,dimB;
	int opNum;
	char *str;

	if ( clipbuffer == NULL ) return 0;
	ErrorNo=0;
	LoadStringType( type, &reg, &opNum );
	str = GetStringPtr( type, n );
	if ( ErrorNo ) return 0; // error
	OpcodeCopy( str, clipbuffer , MatAry[reg].SizeB-1 );
	return 0;
}

int CB_StoreString(int type, char *clipbuffer ){			// ----------- Store  String  type: 0:string  1:fn   2:GraphY
	int key;
	int reg,dimA,dimB;
	int opNum,n;
	char *str;
	if ( clipbuffer == NULL ) return 0;
	LoadStringType( type, &reg, &opNum );
	n = SelectMemoryNum( type, opNum, 1, &key );
	if ( key == KEY_CTRL_EXIT ) return 0;
	return CB_StoreStringSub( type, n, clipbuffer ) ;
}


char* CB_SeeString(int type, int *select, char *clipbuffer ){	// ----------- See    String  type: 0:string  1:fn   2:GraphY
	char buffer[64];
	int key;
	int	cont=1;
	int seltop=(*select);
	int i,j,k,x,y,len,oplen;
	int opNum,opNumMax;
	int reg,dimA,dimB,ElementSize;
	int base;
	char *string=0,*str=0;
	char opcodeFN[]="\xF9\x1B";
	int bk_CB_ColorIndex=CB_ColorIndex;						// current color index
	int bk_G1MorG3M=CB_G1MorG3M;
	unsigned short bk_CB_BackColorIndex=CB_BackColorIndex;	// current backcolor index
	unsigned short bk_CB_ColorIndexPlot=CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_ColorIndex=-1;
	CB_BackColorIndex=COLOR_WHITE;	// White
	CB_ColorIndexPlot=COLOR_BLUE;	// Blue
	CB_G1MorG3M = 3;
	Cursor_SetFlashOff(); 		// cursor flashing off
	SaveDisp(SAVEDISP_PAGE2);

	if ( (*select)<1 ) (*select)=1;
	
	LoadStringType( type, &reg, &opNum );

//	opNum=MatAry[reg].SizeA;

	while (cont) {
		Bdisp_AllClr_VRAM2();
		CB_ColorIndex=COLOR_BLUE;	// Blue
		CB_Prints( 1, 1, (unsigned char*)"== Function Memory ==");
		CB_Prints( 4, 1, (unsigned char*)TypeTitle[type]);
		CB_ColorIndex=-1;

		if (  (*select)<seltop ) seltop = (*select);
		if ( ((*select)-seltop) > 5 ) seltop = (*select)-5;
		if ( seltop < 1 ) seltop = 1;
		if ( (opNum -seltop) < 5 ) seltop = opNum -5; 

		for ( i=0; i<6; i++ ) {
			k=seltop+i;
			j=2; if ( k>=10 ) j=1;
			locate( j, 2+i); Prints((unsigned char*)TypeStr[type]);
			sprintf( buffer,"%d", k );
			if ( k<100 ) {
				locate( j+1, 2+i); Prints((unsigned char*)buffer);
			} else {
				CB_ColorIndex=COLOR_BLACK;	// Black
				CB_PrintMini( 6*3-2, (1+i)*24+6,(unsigned char*)buffer,MINI_OVER);
			}
			locate( 4, 2+i); Prints((unsigned char*)":");
			locate( 5, 2+i);
			dimA = MatAry[reg].SizeA ;
			if ( dimA == 0 ) {
//				Prints((unsigned char*)"");
			} else { 
				if ( dimA >= k ) {
					string = MatrixPtr(reg, k,  1);
					OpcodeStringToAsciiString( buffer, string, 64-1 );
					Prints((unsigned char*)buffer);
				}
			}
		}

		y = ((*select)-seltop)+1 ;
		Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line

		Fkey_Icon( FKeyNo1,  69 );	//	Fkey_dispR( FKeyNo1, "STORE");
		Fkey_Icon( FKeyNo2,  70 );	//	Fkey_dispR( FKeyNo2, "RECALL");
		if ( type==1 ) Fkey_Icon( FKeyNo3, 240 );	//	Fkey_dispR( FKeyNo3, " fn ");
//		Fkey_Icon( FKeyNo4, 241 );	//	Fkey_dispN( FKeyNo4, " SEE");
//		if ( ReEnterSEE == 0 ) Fkey_Icon( FKeyNo5, 389 );	//	Fkey_dispR( FKeyNo5,"EDIT");
		if ( ReEnterSEE == 0 ) Fkey_Icon( FKeyNo4,  42 );	//	Fkey_dispN( FKeyNo4, "Edit");

		k=(*select);
		
		ElementSize=MatAry[reg].ElementSize;

		EnableDisplayStatusArea();
		GetKey_DisableCatalog( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				break;
				
			case KEY_CTRL_UP:
				(*select) -= 1;
				if ( (*select)<1 ) (*select)=opNum;
				break;
			case KEY_CTRL_DOWN:
				(*select) += 1;
				if ( (*select)>opNum ) (*select)=1;
				break;
			case KEY_CTRL_PAGEUP:
				(*select) -= 6;
				if ( (*select)<1 ) (*select)=opNum;
				break;
			case KEY_CTRL_PAGEDOWN:
				(*select) += 6;
				if ( (*select)>opNum ) (*select)=1;
				break;

			case KEY_CTRL_F1:	// store
				CB_StoreStringSub( type, (*select), clipbuffer );
				break;
			case KEY_CTRL_F2:	// recall
				str = RecallStringSub( reg, (*select) ) ;
				cont=0;
				break;
				
			case KEY_CTRL_F3:	// 
				if ( type!=1 ) break;
				str=opcodeFN;
				cont=0;
				break;
				
			case KEY_CTRL_F4:
			case KEY_CTRL_LEFT:
			case KEY_CTRL_RIGHT:
				if ( ReEnterSEE == 0 ) {
					FkeyClearAll();
					Bdisp_AreaReverseVRAMx3(0, y*8, 127, y*8+7);	// reverse select line
					string=GetStringPtr(type, (*select) );
					ReEnterSEE=1;
					if ( key==KEY_CTRL_RIGHT ) {
						key=InputStrSub( 5, y+1, 17, 0, string, MatAry[reg].SizeB-1, " ", REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF);
					} else {
						key=InputStr( 5, y+1, 17,  string, MatAry[reg].SizeB-1, " ", REV_OFF);
					}
					ReEnterSEE=0;
				}
				break;

			default:
				break;
		}
	}
	CB_ColorIndex    =bk_CB_ColorIndex;		// current color index
	CB_BackColorIndex=bk_CB_BackColorIndex;	// current backcolor index
	CB_ColorIndexPlot=bk_CB_ColorIndexPlot;	// Plot color index (default blue)
	CB_G1MorG3M      =bk_G1MorG3M;
	RestoreDisp(SAVEDISP_PAGE2);
	return str;
}