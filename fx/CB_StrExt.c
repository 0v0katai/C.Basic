#include "CB.h"

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
	locate( 3,3); Print((unsigned char *)TypeTitle[type]);
	locate(14,3); Print((unsigned char *)"Memory");
	locate( 3,5); sprintf(buffer,"%s[%d~%d]:",TypeStr[type],1,max); Print((unsigned char *)buffer);
	buffer[0]='\0';
	while (1) {
		n=InputNumD(13+strlen(buffer), 5, log10(max)+1, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &(*key));		// 0123456789
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

char* CB_RecallString(int type ){			// ----------- Recall String  type: 0:string  1:fn   2:GraphY
	unsigned int key;
	int reg,dimA;
	int opNum,n;
	char *string;
	LoadStringType( type, &reg, &opNum );
	dimA = MatAry[reg].SizeA ;
	if ( dimA == 0 ) {
		string = NULL;
	} else { 
		n = SelectMemoryNum( type, opNum, 1, &key );
		if ( key == KEY_CTRL_EXIT ) return 0;
		if ( dimA >= n ) {
			string = MatrixPtr(reg, n,  1);
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
		Fkey_Icon( FKeyNo3, 240 );	//	Fkey_dispR( FKeyNo3, " fn ");
		Fkey_Icon( FKeyNo4, 241 );	//	Fkey_dispN( FKeyNo4, " SEE");

//		Bdisp_PutDisp_DD();

		k=(*select);
		
//		ElementSize=MatAry[reg].ElementSize;

		GetKey( &key );
		if ( KEY_CTRL_XTT   == key ) (*select)=23;	// X
		if ( KEY_CHAR_ANS   == key ) (*select)=28;	// Ans
		if ( KEY_CHAR_THETA == key ) (*select)=27;	// Theta
		if ( KEY_CHAR_VALR  == key ) (*select)=26;	// <r>
		if ( ( 'A' <= key ) && ( key <= 'z' ) ) {
			(*select)=key-'A';
		}
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				break;
				
			case KEY_CTRL_UP:
				(*select) -= 1;
				if ( (*select)<1 ) (*select)=opNum;
//				if ( abs(i-(*select))>1 ) seltop=(*select);
				break;
			case KEY_CTRL_DOWN:
				(*select) += 1;
				if ( (*select)>opNum ) (*select)=1;
//				if ( abs(i-(*select))>1 ) seltop=(*select);
				break;
			case KEY_CTRL_LEFT:
				(*select) -= 6;
				if ( (*select)<1 ) (*select)=opNum;
//				if ( abs(i-(*select))>1 ) seltop=(*select);
				break;
			case KEY_CTRL_RIGHT:
				(*select) -= 6;
				if ( (*select)>opNum ) (*select)=1;
//				if ( abs(i-(*select))>1 ) seltop=(*select);
				break;

			case KEY_CTRL_F1:	// store
				CB_StoreStringSub( type, (*select), clipbuffer );
				break;
			case KEY_CTRL_F2:	// recall
				str = MatrixPtr( reg, (*select), 1 ) ;
				cont=0;
				break;
			case KEY_CTRL_F3:	// fn
				str='\0';
				cont=0;
				break;
//			case KEY_CTRL_F4:
//				break;
				

			default:
				break;
		}
	}
	memcpy( PictAry[0], scrbuf, 896);	// restore VRAM
	return str;
}

