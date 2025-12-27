#include "CB.h"

//-----------------------------------------------------------------------------
//	Graph Function	( originaled by Colon )
//-----------------------------------------------------------------------------
//	Strings -> Graph function memory 
//-----------------------------------------------------------------------------
void StoreGraphSub( char *MatAryC, int FuncNo ){
	MatAryC[4-1] = 0x02;
	switch ( FuncNo % 5 ) {
		case 1:
			MatAryC[5-1] = 0x00;
			MatAryC[6-1] = 0x1F;
			break;
		case 2:
			MatAryC[5-1] = 0xF8;
			MatAryC[6-1] = 0x00;
			break;
		case 3:
			MatAryC[5-1] = 0x07;
			MatAryC[6-1] = 0xE0;
			break;
		case 4:
			MatAryC[5-1] = 0xF8;
			MatAryC[6-1] = 0x1F;
			break;
		case 5:
			MatAryC[5-1] = 0x00;
			MatAryC[6-1] = 0x00;
			break;
	}
}

//-----------------------------------------------------------------------------
void StoreGraphY( char *fstring, int FuncNo ){
	int i,j,k;
	int reg=defaultGraphAry;
	int Counter;
	int oplen;
	char *MatAryC ;
	
	MatAryC = GetStrYFnPtrSub( reg, FuncNo, defaultGraphArySize ) ;
	if ( StrLen( fstring, &oplen ) == 0 ) {
		for ( Counter=1-1; Counter<=7-1; Counter++ ) MatAryC[Counter] = 0;
	} else {
		MatAryC[1-1] = 0x01;
		switch ( FuncType ) {
			case 4:
				MatAryC[2-1] = 0x98; break;
			case 5:
				MatAryC[2-1] = 0x88; break;
			case 6:
				MatAryC[2-1] = 0xB8; break;
			case 7:
				MatAryC[2-1] = 0xA8; break;
			default:
				MatAryC[2-1] = 0x08; break;
		}
		MatAryC[3-1] = 0xBC;
		if ( ( StrLen( MatAryC+6, &oplen ) == 0 ) ) StoreGraphSub( MatAryC, FuncNo );
		OpcodeCopy( MatAryC+6, fstring, MatAry[reg].SizeB-1-6 );
	}
}

void StoreGraphr( char *fstring, int FuncNo ){
	int i,j,k;
	int reg=defaultGraphAry;
	int Counter;
	int oplen;
	char *MatAryC ;
	
	MatAryC = GetStrYFnPtrSub( reg, FuncNo, defaultGraphArySize ) ;
	if ( StrLen( fstring, &oplen ) == 0 ) {
		for ( Counter=1-1; Counter<=7-1; Counter++ ) MatAryC[Counter] = 0;
	} else {
		MatAryC[1-1] = 0x04;
		MatAryC[2-1] = 0x0A;
		MatAryC[3-1] = 0xBC;
		if ( ( StrLen( MatAryC+6, &oplen ) == 0 ) ) StoreGraphSub( MatAryC, FuncNo );
		OpcodeCopy( MatAryC+6, fstring, MatAry[reg].SizeB-1-6 );
	}
}

void StoreGraphXt( char *fstring, int FuncNo ){
	int i,j,k;
	int reg=defaultGraphAry;
	int Counter;
	int oplen;
	char *MatAryC ;
	char separator[]="\xF6\x00";
	char *StrXt,*StrYt;
	char buffer[256];
	
	MatAryC = GetStrYFnPtrSub( reg, FuncNo, defaultGraphArySize ) ;
	StrSplit( MatAryC+6, separator, 1, 256-1 );
	StrXt = MatrixPtr( CB_MatListAnsreg, 1, 1 );
	if ( MatAry[CB_MatListAnsreg].SizeA == 1 ) MatElementPlus( CB_MatListAnsreg, 2, MatAry[CB_MatListAnsreg].SizeB );	// matrix +
	StrYt = MatrixPtr( CB_MatListAnsreg, 2, 1 );
	
	if ( StrLen( fstring, &oplen ) == 0 ) {
		if ( ( ( ReadMatrixInt( reg, FuncNo, 1)*256 + ReadMatrixInt( reg, FuncNo, 2) ) == 0x0409 ) && StrYt[0] ) {
			goto opcpy;
		} else {
			for ( Counter=1-1; Counter<=7-1; Counter++ ) MatAryC[Counter] = 0;
		}
	} else {
		MatAryC[1-1] = 0x04 ;
		MatAryC[2-1] = 0x09 ;
		MatAryC[3-1] = 0xBC ;
		if ( ( StrLen( MatAryC+6, &oplen ) == 0 ) ) StoreGraphSub( MatAryC, FuncNo );
	  opcpy:
		buffer[0]='\0';
		StrJoin( buffer, fstring, 256-1 );
		StrJoin( buffer, separator, 256-1 );
		StrJoin( buffer, StrYt, 256-1 );
		OpcodeCopy( MatAryC+6, buffer, 256-1 );
	}
}

void StoreGraphYt( char *fstring, int FuncNo ){
	int i,j,k;
	int reg=defaultGraphAry;
	int Counter;
	int oplen;
	char *MatAryC ;
	char separator[]="\xF6\x00";
	char *StrXt,*StrYt;
	char buffer[256];
	
	MatAryC = GetStrYFnPtrSub( reg, FuncNo, defaultGraphArySize ) ;
	StrSplit( MatAryC+6, separator, 1, 256-1 );
	StrXt = MatrixPtr( CB_MatListAnsreg, 1, 1 );
	if ( MatAry[CB_MatListAnsreg].SizeA == 1 ) MatElementPlus( CB_MatListAnsreg, 2, MatAry[CB_MatListAnsreg].SizeB );	// matrix +
	StrYt = MatrixPtr( CB_MatListAnsreg, 2, 1 );
	
	if ( StrLen( fstring, &oplen ) == 0 ) {
		if ( ( ( ReadMatrixInt( reg, FuncNo, 1)*256 + ReadMatrixInt( reg, FuncNo, 2) ) == 0x0409 ) && StrXt[0] ) {
			goto opcpy;
		} else {
			for ( Counter=1-1; Counter<=7-1; Counter++ ) MatAryC[Counter] = 0;
		}
	} else {
		MatAryC[1-1] = 0x04 ;
		MatAryC[2-1] = 0x09 ;
		MatAryC[3-1] = 0xBC ;
		if ( ( StrLen( MatAryC+6, &oplen ) == 0 ) ) StoreGraphSub( MatAryC, FuncNo );
	  	if ( StrYt[0]==0 ) StrXt[0]=0;
	  opcpy:
		buffer[0]='\0';
		StrJoin( buffer, StrXt, 256-1 );
		StrJoin( buffer, separator, 256-1 );
		StrJoin( buffer, fstring, 256-1 );
		OpcodeCopy( MatAryC+6, buffer, 256-1 );
	}
}

void StoreGraphX( char *fstring, int FuncNo ){
	int i,j,k;
	int reg=defaultGraphAry;
	int Counter;
	int oplen;
	char *MatAryC ;
	
	MatAryC = GetStrYFnPtrSub( reg, FuncNo, defaultGraphArySize ) ;
	if ( StrLen( fstring, &oplen ) == 0 ) {
		for ( Counter=1-1; Counter<=7-1; Counter++ ) MatAryC[Counter] = 0;
	} else {
		MatAryC[1-1] = 0x02;
		switch ( FuncType ) {
			case 8:
				MatAryC[2-1] = 0x98; break;
			case 9:
				MatAryC[2-1] = 0x88; break;
			case 10:
				MatAryC[2-1] = 0xB8; break;
			case 11:
				MatAryC[2-1] = 0xA8; break;
			default:
				MatAryC[2-1] = 0x08; break;
		}
		MatAryC[3-1] = 0xBC;
		if ( ( StrLen( MatAryC+6, &oplen ) == 0 ) ) StoreGraphSub( MatAryC, FuncNo );
		OpcodeCopy( MatAryC+6, fstring, MatAry[reg].SizeB-1-6 );
	}
}

//-----------------------------------------------------------------------------
//	Graph function memory  -> Strings
//-----------------------------------------------------------------------------
char* ReadGraphY( int FuncNo ) {
	int reg=defaultGraphAry;
	char *MatAryC ;
	MatAryC = MatrixPtr( reg, FuncNo, 1 );
	if ( MatAryC[1-1]!=0x01 ) return NULL;
	return MatAryC +6;
}
char* ReadGraphX( int FuncNo ) {
	int reg=defaultGraphAry;
	char *MatAryC ;
	MatAryC = MatrixPtr( reg, FuncNo, 1 );
	if ( MatAryC[1-1]!=0x02 ) return NULL;
	return MatAryC +6;
}

char* ReadGraphr( int FuncNo ) {
	int reg=defaultGraphAry;
	char *MatAryC ;
	MatAryC = MatrixPtr( reg, FuncNo, 1 );
	if ( ( MatAryC[1-1]!=0x04 ) || ( MatAryC[2-1]!=0x09 ) ) return NULL;
	return MatAryC +6;
}

char* ReadGraphXt( int FuncNo ) {
	int reg=defaultGraphAry;
	char *MatAryC ;
	char separator[]="\xF6\x00";
	char *StrXt,*StrYt;
	int maxsize=MatAry[reg].SizeB-1-6;
	char *buffer;
	
	MatAryC = MatrixPtr( reg, FuncNo, 1 );
	if ( ( MatAryC[1-1]!=0x04 ) || ( MatAryC[2-1]!=0x09 ) ) return NULL;
	return MatAryC +=6;
}


char * StrSearch_0xF6( char *str, int maxlen ) {	// search 0xF6	-> return ptr
	int i;
	for (i=0; i<maxlen; i++) {
		if ( str[i] == 0xFFFFFFF6 ) return str+i;
	}
	return str;
}

char* ReadGraphYt( int FuncNo ) {
	int reg=defaultGraphAry;
	char *MatAryC ;
	char separator[]="\xF6\x00";
	char *StrXt,*StrYt;
	int maxsize=MatAry[reg].SizeB-1-6;
	
	MatAryC = MatrixPtr( reg, FuncNo, 1 );
	if ( ( MatAryC[1-1]!=0x04 ) || ( MatAryC[2-1]!=0x09 ) ) return NULL;
	MatAryC +=6;
	if ( MatAryC[0] == 0x00 ) return NULL;
	return StrSearch_0xF6( MatAryC, maxsize )+1;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_GraphFunc( char *SRC, int c ) {
	int n,d;
	int reg=defaultGraphAry;
	char *buffer;
	if ( ( 0x63<=c ) && ( c<=0x6F) ) {
		switch ( c ) {
			case 0x63:	d= 0; break;		// Y=Type
			case 0x64:	d= 1; break;		// r=Type
			case 0x65:	d= 2; break;		// ParamType
//			case 0x66:			// 
			case 0x67:	d= 3; break;		// X=Type
			case 0x68:	d= 8; break;		// X>Type
			case 0x69:	d= 9; break;		// X<Type
			case 0x6A:	d= 4; break;		// Y>Type
			case 0x6B:	d= 5; break;		// Y<Type
			case 0x6C:	d= 6; break;		// Y>=Type
			case 0x6D:	d= 7; break;		// Y<=Type
			case 0x6E:	d=10; break;		// X>=Type
			case 0x6F:	d=11; break;		// X<=Type
		}
		FuncType = d;
		return;
	}
	if ( c == 0xFFFFFF98 ) {	// SetG-Color
		c = CB_GetColor( SRC );
		if ( SRC[ExecPtr]!=',' ) { CB_Error(SyntaxERR); goto exit; }	// Syntax error
		ExecPtr++;
		buffer = GetStrYFnPtr( SRC, reg, defaultGraphAryN, defaultGraphArySize ) ;
		if ( buffer[7-1] ) {
			buffer[5-1] = c/256;
			buffer[6-1] = c%256;
		}
		goto exit; 
	}

	d=SRC[ExecPtr];
	if ( ( d==':' ) || ( d==0x0D ) || ( d==0x0C ) || ( d==0x00 ) ) {
		n = MatAry[reg].SizeA;
		if ( n ) { 
			while ( n ) {
				buffer = MatrixPtr( reg, n, 1 ) ;
				switch ( c ) {
					case 0xFFFFFFC8:	// G SelOn
						buffer[3-1] |= 0x80;
						break;
					case 0xFFFFFFC9:	// T SelOn
						buffer[3-1] |= 0x20;
						break;
					case 0xFFFFFFD8:	// G SelOff
						buffer[3-1] &= 0x7F;
						break;
					case 0xFFFFFFD9:	// T SelOff
						buffer[3-1] &= 0xDF;
						break;
				}
				n--;
			}
		}
	} else {
		buffer = GetStrYFnPtr( SRC, reg, defaultGraphAryN, defaultGraphArySize ) ;
		if ( buffer[7-1] == 0 ) goto exit; 
		switch ( c ) {
				case 0x2B:			// NormalG
					buffer[4-1] = 0x02;
					break;
				case 0x2C:			// ThickG
					buffer[4-1] = 0x03;
					break;
				case 0x2D:			// BrokenThickG
					buffer[4-1] = 0x05;
					break;
				case 0x3F:			// DotG
					buffer[4-1] = 0x08;
					break;
				case 0xFFFFFFF5:	// ThinG
					buffer[4-1] = 0x12;
					break;
				case 0xFFFFFFC8:	// G SelOn
					buffer[3-1] |= 0x80;
					break;
				case 0xFFFFFFC9:	// T SelOn
					buffer[3-1] |= 0x20;
					break;
				case 0xFFFFFFD8:	// G SelOff
					buffer[3-1] &= 0x7F;
					break;
				case 0xFFFFFFD9:	// T SelOff
					buffer[3-1] &= 0xDF;
					break;
		}
	}
  exit:
	dspflag=0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
short VarListRange=0;

void CB_VarList( char *SRC ) {
	VarListRange = CB_EvalInt( SRC );
}
void CB_VarRange( char *SRC ) {
	VarListRange=0;
}

void CB_F_Result( char *SRC ) {
	dspflag=4;		// 2:value		3:mat    4:list
}

void CB_DispF_Tbl( char *SRC ) {
	
	if ( VarListRange ) {
	} else {
	}

}
