#include "CB.h"

char ReEnterSEE=0;

const char TypeTitle[][10]={"Strings  ","Function ","Graphics "};
const char TypeStr[][2]={"S","f","Y"};


int SelectMemoryNum( int type, int max, int store, unsigned int *key ) {		//
	char buffer[32];
	int n=0,n0=1;
	char *scrbuf[896];
	memcpy( scrbuf, PictAry[0], 896);	// store VRAM

	PopUpWin(4);
	FkeyClearAll();
	locate( 3,2); if ( store ) Print((unsigned char *)"Store In"); else Print((unsigned char *)"Recall From");
	locate( 5,3); Print((unsigned char *)TypeTitle[type]);
	locate(14,3); Print((unsigned char *)"Memory");
	locate( 3,5); sprintf(buffer,"%s[%d~%d]:",TypeStr[type],1,max); Print((unsigned char *)buffer);
	buffer[0]='\0';
	while (1) {
		n=InputNumD(13+strlen(buffer), 5, log10(max)+1, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &(*key));		// 0123456789
		if ( (*key) == KEY_CTRL_EXIT ) break;
		if ( n == n0 ) break;
		if ( (1<=n)&&(n<=max) ) break;
		n=n0;
	}
	memcpy( PictAry[0], scrbuf, 896);	// restore VRAM
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
	unsigned int key;
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
	unsigned int key;
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
	unsigned int key;
	int	cont=1;
	int seltop=(*select);
	int i,j,k,x,y,len,oplen;
	int opNum,opNumMax;
	int reg,dimA,dimB,ElementSize;
	int base;
	char *string=0,*str=0;
	char opcodeFN[]="\xF9\x1B";
	
	char *scrbuf[896];
	memcpy( scrbuf, PictAry[0], 896);	// store VRAM

	Cursor_SetFlashMode(0); 		// cursor flashing off

	if ( (*select)<1 ) (*select)=1;
	
	LoadStringType( type, &reg, &opNum );

//	opNum=MatAry[reg].SizeA;

	while (cont) {
		Bdisp_AllClr_VRAM();
		locate( 1, 1); Print((unsigned char*)"== Function Memory ==");
		locate( 4, 1); Print((unsigned char*)TypeTitle[type]);

		if (  (*select)<seltop ) seltop = (*select);
		if ( ((*select)-seltop) > 5 ) seltop = (*select)-5;
		if ( seltop < 1 ) seltop = 1;
		if ( (opNum -seltop) < 5 ) seltop = opNum -5; 

		for ( i=0; i<6; i++ ) {
			k=seltop+i;
			j=2; if ( k>=10 ) j=1;
			locate( j, 2+i); Print((unsigned char*)TypeStr[type]);
			sprintf( buffer,"%d", k );
			if ( k<100 ) {
				locate( j+1, 2+i); Print((unsigned char*)buffer);
			} else {
				PrintMini( 6+1, (1+i)*8+2,(unsigned char*)buffer,MINI_OVER);
			}
			locate( 4, 2+i); Print((unsigned char*)":");
			locate( 5, 2+i);
			dimA = MatAry[reg].SizeA ;
			if ( dimA == 0 ) {
//				Print((unsigned char*)"");
			} else { 
				if ( dimA >= k ) {
					string = MatrixPtr(reg, k,  1);
					OpcodeStringToAsciiString( buffer, string, 64-1 );
					Print((unsigned char*)buffer);
				}
			}
		}

		y = ((*select)-seltop)+1 ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse (*select) line 

		Fkey_Icon( FKeyNo1,  69 );	//	Fkey_dispR( FKeyNo1, "STORE");
		Fkey_Icon( FKeyNo2,  70 );	//	Fkey_dispR( FKeyNo2, "RECALL");
		if ( type==1 ) Fkey_Icon( FKeyNo3, 240 );	//	Fkey_dispR( FKeyNo3, " fn ");
//		Fkey_Icon( FKeyNo4, 241 );	//	Fkey_dispN( FKeyNo4, " SEE");
//		if ( ReEnterSEE == 0 ) Fkey_Icon( FKeyNo5, 389 );	//	Fkey_dispR( FKeyNo5,"EDIT");
		if ( ReEnterSEE == 0 ) Fkey_Icon( FKeyNo4,  42 );	//	Fkey_dispN( FKeyNo4, "Edit");

//		Bdisp_PutDisp_DD();

		k=(*select);
		
//		ElementSize=MatAry[reg].ElementSize;

		GetKey( &key );
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
					Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse (*select) line 
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

//			case KEY_CHAR_STORE:
//				if ( ReEnterSEE == 0 ) {
//					FkeyClearAll();
//					Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse (*select) line 
//					string=GetStringPtr(type, (*select) );
//					ReEnterSEE=1;
//					key=InputStr( 5, y+1, 17,  string, MatAry[reg].SizeB-1, " ", REV_OFF);
//					ReEnterSEE=0;
//				}
//				break;

			default:
				break;
		}
	}
	memcpy( PictAry[0], scrbuf, 896);	// restore VRAM
	return str;
}

