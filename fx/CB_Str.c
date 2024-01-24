#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"
#include "CB_Matrix.h"
#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "CB_Str.h"
#include "fx_syscall.h"

//----------------------------------------------------------------------------------------------
//		Interpreter inside
//----------------------------------------------------------------------------------------------
char   *CB_CurrentStr;	//

char   CB_StrBufferCNT;
char   CB_StrBuffer[CB_StrBufferCNTMax][CB_StrBufferMax];	//

char   defaultStrAry=26;	// <r>
char   defaultStrAryN=20;
short  defaultStrArySize=255+1;

char   defaultFnAry=27;		// Theta
char   defaultFnAryN=8;
short  defaultFnArySize=255+1;

char   defaultGraphAry=27;		// Theta
char   defaultGraphAryN=8;
short  defaultGraphArySize=255+1;

char	dummychar1;
char	dummychar2;
char	dummychar3;
char	dummychar4;
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int StrGetOpcode( char *SRC, int ptr ){
	int c=SRC[ptr];
	switch ( c ) {
		case 0x00:		// <EOF>
			return 0 ;
		case 0x5C:	//  Backslash
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			return ((unsigned char)c<<8)+(unsigned char)SRC[ptr+1];
	}
	return (unsigned char)c ;
}
int StrGetOpcodeInc( char *SRC, int *ptr ){
	int c=SRC[(*ptr)++];
	switch ( c ) {
		case 0x00:		// <EOF>
			(*ptr)--;
			return 0 ;
		case 0x5C:	//  Backslash
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			return ((unsigned char)c<<8)+(unsigned char)SRC[(*ptr)++];
	}
	return (unsigned char)c ;
}
void StrPutOpcodeInc( char *SRC, int *ptr, int opcode ){
	if ( opcode >= 0x100 ) {
		SRC[(*ptr)++]=opcode>>8;
		SRC[(*ptr)++]=opcode & 0xFF;
	} else {
		SRC[(*ptr)++]=opcode;
	}
}

int StrNextOpcode( char *SRC, int *offset ){
	switch ( SRC[(*offset)++] ) {
		case 0x00:		// <EOF>
			(*offset)--;
			return 0 ;
		case 0x5C:		//  Backslash
		case 0x7F:		// 
		case 0xFFFFFFF7:		// 
		case 0xFFFFFFF9:		// 
		case 0xFFFFFFE5:		// 
		case 0xFFFFFFE6:		// 
		case 0xFFFFFFE7:		// 
		case 0xFFFFFFFF:	// 
			(*offset)++;
			return 2 ;
	}
	return 1 ;
}
int StrPrevOpcode( char *SRC, int *offset ){
	int c;
	(*offset)-=2; 
	if ( *offset < 0 ) { (*offset)=0; return 0;}
		c=SRC[*offset];
		switch ( c ) {
			case 0x5C:		//  Backslash
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

int StrOpcodePtr( char *str, int strptr) {	// strptr(1- )   ->  byteptr(0- )
	int i,ptr=0;
	strptr--;
	if ( strptr ) for ( i=0; i<strptr; i++ ) StrNextOpcode( str, &ptr );
	return ptr;
}

//-----------------------------------------------------------------------------
int StrLen( char *str , int *oplen ) {
	int	slen=0;
	(*oplen)=0;
	while ( 1 ) {
		if ( StrNextOpcode( str, &(*oplen) ) == 0x00 ) break;
		slen++;
	}
	return slen;
}
void StrJoin( char *str1, char *str2, int maxlen ) {
	int i,len,len1,len2;
	len1=strlenOp( str1 );
	len2=strlenOp( str2 ); if ( len2==0 ) return ;
	len=len2;
	if ( len1+len2 > maxlen ) len=maxlen-len1;
	for ( i=0; i<=len; i++ ) str1[len1+i]=str2[i];
}
int StrCmp( char *str1, char *str2 ) {	// str1==str0 ->0   str1>str2 ->1   str1<str2 ->-1
	int i=0,j=0,c,d;
	while (1) {
		c=StrGetOpcodeInc( str1, &i );
		d=StrGetOpcodeInc( str2, &j );
		if ( c > d ) return 1;
		else
		if ( c < d ) return -1;
		if ( ( c==0 ) && ( d==0 ) ) return 0;
	}	
	return 0;
}

int StrSrc( char *SrcBase, char *searchstr, int *strptr, int size){
	int opbkup,csrptr;
	int sptr,cptr;
	int opcode=1,srccode;

	csrptr=StrOpcodePtr( SrcBase, (*strptr) ); 	// opptr -> byteptr
	opbkup=(*strptr);
	while ( (csrptr)<size ) {
		sptr=0;
		opcode =StrGetOpcode( SrcBase, csrptr ) ;
		srccode=StrGetOpcode( searchstr, sptr ) ;
		if ( opcode == 0 ) break;	// No search
		if ( opcode != srccode ) {
			StrNextOpcode( SrcBase, &(csrptr) ); (*strptr)++;
		} else {
			cptr = (*strptr);
			while ( (csrptr)<size ) {
				StrNextOpcode( searchstr, &sptr );
				srccode=StrGetOpcode( searchstr, sptr ) ;
				if ( srccode == 0x00 ) { (*strptr)=cptr; return 1; }	// Search Ok
				StrNextOpcode( SrcBase, &(csrptr) ); (*strptr)++;
				opcode =StrGetOpcode( SrcBase, csrptr ) ;
//				if ( (csrptr) >= size ) { (*strptr)=opbkup; return 0; }	// No search
				if ( opcode != srccode ) break ;
			}
		}
	}
	{ (*strptr)=opbkup; return 0; }	// No search
}

int StrMidCopySub( char *str1, char *str2, int oplen, int n, int m ) {	// mid$(str2,n,m) -> str1
	int i,opptr1,opptr2;
	opptr1=StrOpcodePtr( str2, n ); 		// strptr -> opptr1
	opptr2=StrOpcodePtr( str2, n+m ); 		// strptr -> opptr2
	if ( opptr2 > oplen ) opptr2=oplen;
	i=opptr2-opptr1;
	if ( i ) memcpy( str1, str2+opptr1, i );
	return i;
}
void StrMid( char *str1, char *str2, int n, int m ) {	// mid$(str2,n,m) -> str1
	int i,opptr1,opptr2,slen,oplen;
	int opcode;
	slen=StrLen( str2 ,&oplen );
	if ( n < 1 )  { i=0; goto exit; }
	if ( n > slen ) n=slen;
	if ( ( m <= 0 ) || ( m > slen-n ) ) m=slen-n+1;
	
	i=StrMidCopySub( str1, str2, oplen, n, m );
	exit:
	str1[i]='\0';
}

void StrRight( char *str1, char *str2, int n ) {	// Right$(str2,n) -> str1
	int i,opptr1,opptr2,slen,oplen,m;
	int opcode;
	slen=StrLen( str2 ,&oplen );
	if ( n < 1 ) { i=0; goto exit; }
	if ( n > slen ) n=slen;
	m=slen-n+1;
	i=StrMidCopySub( str1, str2, oplen, m, n );
	exit:
	str1[i]='\0';
}

int StrInv( char *str1, char *str2 ) {	// mirror$(str2) -> str1
	int i,slen,oplen;
	int opcode;
	int opptr1,opptr2;
	slen=StrLen( str2 ,&oplen );
	if ( slen < 2 ) return;
	opptr1=0;
	opptr2=StrOpcodePtr( str2, slen-1 ); 		// strptr -> opptr2
	for (i=0; i<slen; i++ ) {
		opcode =StrGetOpcode( str2, opptr2 ) ;
		StrPrevOpcode( str2, &opptr2 ) ;
		StrPutOpcodeInc( str1, &opptr1, opcode ) ;
	}
	str1[opptr1]='\0';
}

int StrRotate( char *str1, char *str2, int n ) {	// Rotate$("1234567",  2) -> "6712345"
	int i,j,opptr1,opptr2,slen,oplen;				// Rotate$("1234567", -2) -> "3456712"
	int opcode,m;
	if ( n == 0 ) return;
	slen=StrLen( str2 ,&oplen );
	if ( n > 0 ) {
		m=slen-n+1;
		i=StrMidCopySub( str1, str2, oplen, m, n );
		j=StrMidCopySub( str1+i, str2, oplen, 1, m-1 );
	} else { n=-n;
		m=slen-n;
		i=StrMidCopySub( str1, str2, oplen, n+1, m );
		j=StrMidCopySub( str1+i, str2, oplen, 1, n );
	}
	str1[i+j]='\0';
}

int StrShift( char *str1, char *str2, int n ) {		// Shift$("1234567",  2) -> "34567"
	int i,opptr1,opptr2,slen,oplen;					// Shift$("1234567", -2) -> "12345"
	int opcode,m;
	if ( n == 0 ) return;
	slen=StrLen( str2 ,&oplen );
	if ( n > 0 ) {
		m=slen-n+1;
		i=StrMidCopySub( str1, str2, oplen, n, m );	// =Right$
	} else { n=-n;
		m=slen-n;
		i=StrMidCopySub( str1, str2, oplen, 1, m );	// =Left$
	}
	str1[i]='\0';
}
int StrLwr( char *str1, char *str2 ) {		// Lwr$(str2, n) -> str1
	int i,slen,oplen;
	int opcode;
	int opptr1,opptr2;
	slen=StrLen( str2 ,&oplen );
	if ( slen < 1 ) return;
	opptr1=0;
	opptr2=0;
	for (i=0; i<slen; i++ ) {
		opcode =StrGetOpcodeInc( str2, &opptr2 ) ;
		if ( ( 'A' <= opcode ) && ( opcode <= 'Z' ) ) opcode+=('a'-'A');
		StrPutOpcodeInc( str1, &opptr1, opcode ) ;
	}
	str1[opptr1]='\0';
}
int StrUpr( char *str1, char *str2 ) {		// Upr$(str2, n) -> str1
	int i,slen,oplen;
	int opcode;
	int opptr1,opptr2;
	slen=StrLen( str2 ,&oplen );
	if ( slen < 1 ) return;
	opptr1=0;
	opptr2=0;
	for (i=0; i<slen; i++ ) {
		opcode =StrGetOpcodeInc( str2, &opptr2 ) ;
		if ( ( 'a' <= opcode ) && ( opcode <= 'z' ) ) opcode-=('a'-'A');
		StrPutOpcodeInc( str1, &opptr1, opcode ) ;
	}
	str1[opptr1]='\0';
}

int StrChar( char *str1, char *str2, int n ){	// StrChar(str2,n)->str1
	int i,j,slen,oplen;
	int opptr1;
	slen=StrLen( str2 ,&oplen );
	opptr1=0;
	for ( i=0; i<n; i++ ) {
		for ( j=0; j<oplen; j++ ) {
			if ( opptr1>CB_StrBufferMax-1 ) goto exit;
			str1[opptr1++]=str2[j];
		}
	}
	exit:
	str1[opptr1]='\0';
}

int StrCenter( char *str1, char *str2, int max, char *str3 ){	// StrCenter(str2,max,str3)->str1
	char buffer[CB_StrBufferMax];
	int n,slen,oplen;
	slen=StrLen( str2 ,&oplen );
	if ( CB_StrBufferMax-1 < max ) max=CB_StrBufferMax-1;
	if ( max < 1 ) max=1;
	n=(max-1)/2-slen/2;
	StrChar( buffer, str3, max ) ;
	StrJoin( str2, buffer, CB_StrBufferMax-1 ) ;
	str1[0]='\0';
	if ( n>=0 ) { 
		StrRotate( buffer, str2, n );
		StrMid( str1, buffer, 1, max );
	} else {
		StrMid( str1, str2, 1, max );
	}
}

//----------------------------------------------------------------------------------------------
int OpcodeCopy(char *buffer, char *SRC, int Maxlen) {
	int c;
	int srcPtr=0,ptr=0;
	while ( 1 ){
		c = SRC[srcPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				buffer[ptr]='\0';
				return ptr;
			case 0x5C:	//  Backslash
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				if ( ptr < Maxlen-1 ) {	// 2byte opcode
					buffer[ptr++]=c;
					buffer[ptr++]=SRC[srcPtr++];
				} else {
					buffer[ptr]='\0';
					return ptr;
				}
				break;
			default:
				if ( ptr < Maxlen ) {	// 1byte opcode
					buffer[ptr++]=c;
				} else {
					buffer[ptr]='\0';
					return ptr;
				}
				break;
		}
	}
	return ptr;
}

void OpcodeStringToAsciiString(char *buffer, char *SRC, int Maxlen ) {	// Opcode String  ->  Ascii String
	char tmpbuf[18];
	int i,j=0,len,srcPtr=0,ptr=0;
	int c=1;
	while ( c != '\0' ) {
		c = SRC[srcPtr++] ; 
		if ( c==0x5C ) // Backslash
			c = SRC[srcPtr++]&0xFF ;
		else
		if ( (c==0x7F)||(c==0xFFFFFFF7)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) ) 
			c = ( ( c & 0xFF )<< 8 ) + (SRC[srcPtr++]&0xFF);
		else c = c & 0xFF;

		CB_OpcodeToStr( c, tmpbuf ) ;	// SYSCALL
		len = strlen( (char*)tmpbuf ) ;
		i=0;
		while ( i < len ) buffer[ptr++]=tmpbuf[i++] ;
		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	buffer[ptr]='\0' ;
}
//-----------------------------------------------------------------------------
int CB_GetQuotOpcode(char *SRC, char *buffer, int Maxlen) {
	int c;
	int ptr=0;
	c=SRC[ExecPtr-2];
	if ( ( c==0x27 ) || ( c==' ' ) || ( c==0x0D ) || ( c==':' ) ) {
		while (1){
			c = SRC[ExecPtr++];
			buffer[ptr++]=c;
			switch ( c ) {
				case 0x00:	// <EOF>
				case 0x22:	// "
					buffer[--ptr]='\0' ;
					return ptr;
				case 0x5C:	//
				case 0x7F:	// 
				case 0xFFFFFFF7:	// 
				case 0xFFFFFFF9:	// 
				case 0xFFFFFFE5:	// 
				case 0xFFFFFFE6:	// 
				case 0xFFFFFFE7:	// 
				case 0xFFFFFFFF:	// 
					buffer[ptr++]=SRC[ExecPtr++];
					break;
				default:
					break;
			}
			if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
		}
	} else {
		while (1){
			c = SRC[ExecPtr++];
			buffer[ptr++]=c;
			switch ( c ) {
				case 0x00:	// <EOF>
				case 0x0D:	// <CR>
				case 0x22:	// "
					buffer[--ptr]='\0' ;
					return ptr;
				case 0x5C:	//
				case 0x7F:	// 
				case 0xFFFFFFF7:	// 
				case 0xFFFFFFF9:	// 
				case 0xFFFFFFE5:	// 
				case 0xFFFFFFE6:	// 
				case 0xFFFFFFE7:	// 
				case 0xFFFFFFFF:	// 
					buffer[ptr++]=SRC[ExecPtr++];
					break;
				default:
					break;
			}
			if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
		}
	}
	return ptr;
}

void CB_GetLocateStr(char *SRC, char *buffer, int Maxlen ) {
	int i,maxoplen;
	char *buffer2;
	buffer2 = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return ;			// error
	OpcodeStringToAsciiString( buffer, buffer2, Maxlen );
}

//----------------------------------------------------------------------------------------------
// Casio Basic
//----------------------------------------------------------------------------------------------

int CB_IsStr( char *SRC, int execptr ) {
	int c=SRC[execptr];
	if ( c == 0x22 ) {	// String
		return 1;
	} else
	if ( c=='$' ) {	// Mat String
		return 2;
	} else
	if ( c == 0xFFFFFFF9 ) {	// Str
		c=SRC[execptr+1];
		if ( c == 0x30 ) return c;	// StrJoin(
		else
		if ( ( c == 0x38 ) || ( c == 0x3E ) ) return 0;	// Exp( or ClrVct
		else
		if ( ( 0x34 <= c ) && ( c <= 0x47 ) ) return c;
		else
		if ( c == 0x1B ) return c;	// fn
	} else
	if ( c == 0x7F ) {
		c=SRC[execptr+1];
		if ( c == 0xFFFFFFF0 )  return c;	// GraphY
	}
	return 0;
}

char* NewStrBuffer(){
	if ( CB_StrBufferCNT > CB_StrBufferCNTMax-1 ) { CB_Error(MemoryERR); return NULL; }	// Memory error
	return CB_StrBuffer[CB_StrBufferCNT++];
}

char* CB_GetOpStr1( char *SRC ,int *maxlen ) {		// String -> buffer	return
	int c,d,n;
	int execptr,len,i=0;
	int reg,dimA,dimB;
	char *buffer;

	switch ( CB_IsStr( SRC, ExecPtr ) ) {
		case 1:	// """"
			ExecPtr++;
			buffer=NewStrBuffer(); if ( buffer==NULL ) return 0;
			CB_GetQuotOpcode( SRC, buffer, CB_StrBufferMax-1 );
			(*maxlen)=CB_StrBufferMax-1;
			break;
		case 2:	// $Mat
			ExecPtr++;
//			if ( ( SRC[ExecPtr] == 0x7F ) && ( SRC[ExecPtr+1] == 0x40 ) ) ExecPtr+=2;	// skip 'Mat '
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return 0 ;			// error
			buffer=MatrixPtr( reg, dimA, dimB );
			(*maxlen)=MatAry[reg].SizeB;
			break;
		case 0x3F:	// Str 1-20
			reg=defaultStrAry;
			ExecPtr+=2;
			if ( MatAry[reg].SizeA == 0 ) {
				DimMatrixSub( reg, 8, defaultStrAryN+1-MatBase, defaultStrArySize, MatBase );	// byte matrix
			}
	str1:	if ( ErrorNo ) return ; // error
			if ( CB_INT ) dimA = EvalIntsub1( SRC ); else dimA = Evalsub1( SRC );	// str no : Mat s[n,len]
			if ( ( dimA < MatAry[reg].Base ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return 0; }  // Argument error
			dimB=MatAry[reg].Base;
			buffer=MatrixPtr( reg, dimA, dimB );
			(*maxlen)=MatAry[reg].SizeB;
			break;
		case 0x1B:	// fn
			reg=defaultFnAry;
			ExecPtr+=2;
			if ( MatAry[reg].SizeA == 0 ) {
				DimMatrixSub( reg, 8, defaultFnAryN+1-MatBase, defaultFnArySize, MatBase );	// byte matrix
			}
			goto str1;
			break;
		case 0xFFFFFFF0:	// GraphY
			reg=defaultGraphAry;
			ExecPtr+=2;
			if ( MatAry[reg].SizeA == 0 ) {
				DimMatrixSub( reg, 8, defaultGraphAryN+1-MatBase, defaultGraphArySize, MatBase );	// byte matrix
			}
			goto str1;
			break;
		case 0x30:	// StrJoin(
			ExecPtr+=2;
			(*maxlen)=CB_StrJoin( SRC );
			return CB_CurrentStr;
		case 0x34:	// StrLeft(
			ExecPtr+=2;
			(*maxlen)=CB_StrLeft( SRC );
			return CB_CurrentStr;
		case 0x35:	// StrRight(
			ExecPtr+=2;
			(*maxlen)=CB_StrRight( SRC );
			return CB_CurrentStr;
		case 0x36:	// StrMid(
			ExecPtr+=2;
			(*maxlen)=CB_StrMid( SRC );
			return CB_CurrentStr;
		case 0x37:	// Exp->Str(
			ExecPtr+=2;
			(*maxlen)=CB_ExpToStr( SRC );
			return CB_CurrentStr;
		case 0x39:	// StrUpr(
			ExecPtr+=2;
			(*maxlen)=CB_StrUpr( SRC );
			return CB_CurrentStr;
		case 0x3A:	// StrLwr(
			ExecPtr+=2;
			(*maxlen)=CB_StrLwr( SRC );
			return CB_CurrentStr;
		case 0x3B:	// StrInv(
			ExecPtr+=2;
			(*maxlen)=CB_StrInv( SRC );
			return CB_CurrentStr;
		case 0x3C:	// StrShift(
			ExecPtr+=2;
			(*maxlen)=CB_StrShift( SRC );
			return CB_CurrentStr;
		case 0x3D:	// StrRotate(
			ExecPtr+=2;
			(*maxlen)=CB_StrRotate( SRC );
			return CB_CurrentStr;
		case 0x40:	// Str(
			ExecPtr+=2;
			(*maxlen)=CB_EvalToStr( SRC );
			return CB_CurrentStr;
		case 0x41:	// DATE
			ExecPtr+=2;
			(*maxlen)=CB_DateToStr();
			return CB_CurrentStr;
		case 0x42:	// TIME
			ExecPtr+=2;
			(*maxlen)=CB_TimeToStr();
			return CB_CurrentStr;
		case 0x43:	// Sprintf(
			ExecPtr+=2;
			(*maxlen)=CB_Sprintf( SRC );
			return CB_CurrentStr;
		case 0x44:	// StrChar(
			ExecPtr+=2;
			(*maxlen)=CB_StrChar( SRC );
			return CB_CurrentStr;
		case 0x45:	// StrCenter(
			ExecPtr+=2;
			(*maxlen)=CB_StrCenter( SRC );
			return CB_CurrentStr;
		case 0x46:	// Hex(
			ExecPtr+=2;
			(*maxlen)=CB_Hex( SRC );
			return CB_CurrentStr;
		case 0x47:	// Bin(
			ExecPtr+=2;
			(*maxlen)=CB_Bin( SRC );
			return CB_CurrentStr;
		default:
			{ CB_Error(SyntaxERR); return 0; }  // Syntax error
	}
	return buffer;
}

char* CB_GetOpStr( char *SRC, int *maxoplen ) {	// Get opcode String 
	int c;
	char *buffer;
	char *CB_StrAddBuffer;
	
	CB_CurrentStr=CB_GetOpStr1( SRC, &(*maxoplen) ) ;		// String -> CB_CurrentStr
	if ( ErrorNo ) return 0;	// error
	c=SRC[ExecPtr];
	if ( c != 0xFFFFFF89 ) { // non +
		if ( c == ')' ) ExecPtr++;	
		return CB_CurrentStr; //
	}
	CB_StrAddBuffer=NewStrBuffer(); if ( buffer==NULL ) return 0;
	OpcodeCopy( CB_StrAddBuffer, CB_CurrentStr, CB_StrBufferMax-1 );		//
	while (1) {
		ExecPtr++;
		CB_CurrentStr=CB_GetOpStr1( SRC, &(*maxoplen) ) ;		// String -> CB_CurrentStr
		if ( ErrorNo ) return 0;	// error
		
		StrJoin( CB_StrAddBuffer, CB_CurrentStr, CB_StrBufferMax-1 ) ;
		c=SRC[ExecPtr];
		if ( c != 0xFFFFFF89 ) break ; // +
	}
	if ( c == ')' ) ExecPtr++;	
	return CB_StrAddBuffer;
}

//----------------------------------------------------------------------------------------------
void StorStrMat( char *SRC ) {	// "String" -> &Mat A
	int reg,dimA,dimB;
	char *MatAryC;
	MatrixOprand( SRC, &reg, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	if ( MatAry[reg].ElementSize != 8 ) { CB_Error(ArgumentERR); return; }	// element size error
	MatAryC=MatrixPtr( reg, dimA, dimB );
	OpcodeCopy( MatAryC, CB_CurrentStr, MatAry[reg].SizeB-1 );
}

void StorStrStr( char *SRC ) {	// "String" -> Sto 1-20
	int reg,dimA,dimB;
	char *MatAryC;
	reg=defaultStrAry;
	if ( MatAry[reg].SizeA == 0 ) {
		DimMatrixSub( reg, 8, defaultStrAryN+1-MatBase, defaultStrArySize, MatBase );	// byte matrix
		if ( ErrorNo ) return ; // error
	}
	dimA = CB_EvalInt( SRC );	// str no : Mat s[n,len]
	if ( ( dimA < MatAry[reg].Base ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	MatAryC=MatrixPtr( reg, dimA, MatAry[reg].Base );
	OpcodeCopy( MatAryC, CB_CurrentStr, MatAry[reg].SizeB-1 );
}

void StorStrGraphY( char *SRC ) {	// "String" -> GraphY 1-5
	int reg,dimA,dimB;
	char *MatAryC;
	reg=defaultGraphAry;
	if ( MatAry[reg].SizeA == 0 ) {
		DimMatrixSub( reg, 8, defaultGraphAryN+1-MatBase, defaultGraphArySize, MatBase );	// byte matrix
		if ( ErrorNo ) return ;
	}
	dimA = CB_EvalInt( SRC );	// GraphY no : Mat y[n,len]
	if ( ( dimA < MatAry[reg].Base ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	MatAryC=MatrixPtr( reg, dimA, MatAry[reg].Base );
	OpcodeCopy( MatAryC, CB_CurrentStr, MatAry[reg].SizeB-1 );
}

void StorStrFn( char *SRC ) {	// "String" -> fn 1-9
	int reg,dimA,dimB;
	char *MatAryC;
	reg=defaultFnAry;
	if ( MatAry[reg].SizeA == 0 ) {
		DimMatrixSub( reg, 8, defaultFnAryN+1-MatBase, defaultFnArySize, MatBase );	// byte matrix
		if ( ErrorNo ) return ;
	}
	dimA = CB_EvalInt( SRC );	// fn no : Mat x[n,len]
	if ( ( dimA < MatAry[reg].Base ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	MatAryC=MatrixPtr( reg, dimA, MatAry[reg].Base );
	OpcodeCopy( MatAryC, CB_CurrentStr, MatAry[reg].SizeB-1 );
}

void StorStrList0( char *SRC ) {	// "String" -> List n[0]
	int reg,dimA;
	reg=ListRegVar( SRC );
	if ( reg<0 ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	if ( SRC[ExecPtr]=='[' ) {
		ExecPtr++;
		dimA = CB_EvalInt( SRC );
		if ( dimA != 0 ) { CB_Error(ArgumentERR); return; }  // Argument error
		if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
	}
	if ( MatAry[reg].SizeA == 0 ) { 
		DimMatrixSub( reg, CB_INT? 32:64, 10-MatBase, 1, MatBase );	// new matrix
		if ( ErrorNo ) return ; // error
	}
}

void StorDATE( char *buffer ) {	// "2017/01/17" -> DATE
	int a,hour,min,sec;
	unsigned char datestr[8];
//	if ( ( buffer[4] != '/' ) || ( buffer[7] != '/' ) ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	a = GetTime();
	hour  = ( a >> 16 ) & 0xFF ;
	min   = ( a >>  8 ) & 0xFF ;
	sec   = ( a       ) & 0xFF ;
	datestr[0]=((buffer[0]-'0')<<4) + buffer[1]-'0';	// year1
	datestr[1]=((buffer[2]-'0')<<4) + buffer[3]-'0';	// year1
	datestr[2]=((buffer[5]-'0')<<4) + buffer[6]-'0';	// min
	datestr[3]=((buffer[8]-'0')<<4) + buffer[9]-'0';	// sec
	datestr[4]=hour;
	datestr[5]=min;
	datestr[6]=sec;
	datestr[7]=0;

	RTC_SetDateTime( datestr ) ;
	CB_TicksStart=RTC_GetTicks();	// init
}

void StorTIME( char *buffer ) {	// "23:59:59" -> TIME
	int a,year1,year2,month,day;
	unsigned char timestr[8];
//	if ( ( buffer[2] != ':' ) || ( buffer[5] != ':' ) ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	a = GetDate();
	year1 = ( a >> 24 ) & 0xFF ;
	year2 = ( a >> 16 ) & 0xFF ;
	month = ( a >>  8 ) & 0xFF ;
	day   = ( a       ) & 0xFF ;
	timestr[0]=year1;
	timestr[1]=year2;
	timestr[2]=month;
	timestr[3]=day;
	timestr[4]=((buffer[0]-'0')<<4) + buffer[1]-'0';	// hour
	timestr[5]=((buffer[3]-'0')<<4) + buffer[4]-'0';	// min
	timestr[6]=((buffer[6]-'0')<<4) + buffer[7]-'0';	// sec
	timestr[7]=0;

	RTC_SetDateTime( timestr ) ;
	CB_TicksStart=RTC_GetTicks();	// init
}

void CB_StorStr( char *SRC ) {
	int c;
	c=CB_IsStr( SRC, ExecPtr );
	switch ( c ) {
		case 2:	// &Mat
			ExecPtr++;
			StorStrMat( SRC ) ;
			break;
		case 0x3F:	// Str 1-20
			ExecPtr+=2;
			StorStrStr( SRC ) ;
			break;
		case 0x41:	// DATE
			ExecPtr+=2;
			StorDATE( CB_CurrentStr ) ;
			break;
		case 0x42:	// TIME
			ExecPtr+=2;
			StorTIME( CB_CurrentStr ) ;
			break;
		case 0x1B:			// fn
			ExecPtr+=2;
			StorStrFn( SRC ) ;
			break;
		case 0xFFFFFFF0:	// GraphY
			ExecPtr+=2;
			StorStrGraphY( SRC ) ;
			break;
		default:
			c=SRC[ExecPtr];
			if ( c == 0x7F ) {
				c=SRC[ExecPtr+1];
				if ( c == 0x51 ) {	// List [0]?
					ExecPtr+=2;
					StorStrList0( SRC ) ;
					return;
				}
			} else { CB_Error(SyntaxERR); return ; }  // Syntax error
	}
}

//----------------------------------------------------------------------------------------------

void CB_StrPrint( char *SRC , int csrX ) {
	char buffer[CB_StrBufferMax];
	int c,d,i=0;
	
	c = SRC[ExecPtr] ; 
	if ( c == 0x0E ) {	// -> store str
		ExecPtr++;
		CB_StorStr( SRC );
		dspflag=0;
		if ( ErrorNo ) return ;  // error
	} else {			// display str
		OpcodeStringToAsciiString( buffer, CB_CurrentStr, CB_StrBufferMax-1 );
		CB_SelectTextVRAM();	// Select Text Screen
		if ( CursorX >1 ) Scrl_Y();
		CursorX+=csrX;
		while ( buffer[i] ) {
			CB_PrintC( CursorX, CursorY, (unsigned char*)buffer+i );
			CursorX++; if ( ( CursorY < 7 ) && ( CursorX > 21 ) ) Scrl_Y();
			c = buffer[i] ;
			if ( ( c==0x0C ) || ( c==0x0D ) ) Scrl_Y();
			if ( (c==0x7F)||(c==0xFFFFFFF7)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) ) i++;
			i++;
		}
//		if ( buffer[0]==NULL ) CursorX=22;
		if ( ( CursorY < 7 ) && ( CursorX == 1 ) ) Scrl_Y();
		dspflag=1;
		Bdisp_PutDisp_DD_DrawBusy_skip_through_text(SRC);
	}
}

void CB_Str( char *SRC ){		// "" "" or &Mat or Str1-20 or StrFunction
	int maxoplen;
	
	CB_CurrentStr = CB_GetOpStr( SRC, &maxoplen );	
	if ( ErrorNo ) return ;  // error

	CB_StrPrint(SRC, 0);
}

//----------------------------------------------------------------------------------------------
int CB_StrLen( char *SRC ) {
	int i,maxoplen;
	int	buffercnt=CB_StrBufferCNT;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_StrBufferCNT=buffercnt;
	return StrLen( buffer ,&i );
}
int CB_StrCmp( char *SRC ) {
	int maxoplen;
	int	buffercnt=CB_StrBufferCNT;
	char *buffer, *buffer2;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	buffer2 = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	CB_StrBufferCNT=buffercnt;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return StrCmp( buffer, buffer2 ) ;
}

int CB_StrSrc( char *SRC ) {
	int sptr=0,slen,maxoplen;
	int	buffercnt=CB_StrBufferCNT;
	char *buffer, *buffer2;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	slen=StrLen( CB_CurrentStr ,&maxoplen);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	buffer2  = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0 ;  // error
	CB_StrBufferCNT=buffercnt;
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		sptr = CB_EvalInt( SRC );	//
		if ( sptr < 1 ) sptr=1;
		if ( sptr > slen ) sptr=slen;
	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( StrSrc( buffer, buffer2, &sptr, CB_StrBufferMax-1 ) ==0 ) return 0 ; // no found
	return sptr+1;
}

//----------------------------------------------------------------------------------------------
double CB_EvalStrDBL( char *buffer, int calcflag ) {		// Eval str -> double
	double result;
	int execptr=ExecPtr;
//	int oplen=strlenOp( buffer );
//	if ( oplen == 0 ) return 0;
	ExecPtr = 0;
//	ErrorPtr= 0;
//	ErrorNo = 0;
    if ( calcflag == 0 ) {
		if ( CB_INT)	result = EvalIntsub14( buffer );
		else			result = Evalsub14( buffer );
	}
    else {
		if ( CB_INT)	result = ListEvalIntsubTop( buffer );	// List calc
		else			result = ListEvalsubTop( buffer );	// List calc
	}
//	if ( ExecPtr < oplen ) { ExecPtr=execptr; CB_Error(SyntaxERR) ; } // Syntax error 
    ExecPtr=execptr;
	if ( ErrorNo ) { ErrorPtr=ExecPtr; return 0; }
	return result;
}
double CB_EvalStrDBL2( char *buffer, int calcflag ) {		// Eval str -> double
	double result;
	int execptr=ExecPtr;
	ExecPtr = 0;
    if ( calcflag == 0 ) {
		if ( CB_INT)	result = EvalIntsubTop( buffer );
		else			result = EvalsubTop( buffer );
	}
    else {
		if ( CB_INT)	result = ListEvalIntsubTop( buffer );	// List calc
		else			result = ListEvalsubTop( buffer );	// List calc
	}
    ExecPtr=execptr;
	if ( ErrorNo ) { ErrorPtr=ExecPtr; return 0; }
	return result;
}

int CB_EvalStrInt( char *buffer, int calcflag ) {		// Eval str -> Int
	int result;
	int execptr=ExecPtr;
//	int oplen=strlenOp( buffer );
//	if ( oplen == 0 ) return 0;	
	ExecPtr = 0;
    if ( calcflag == 0 ) {
		result = EvalIntsub14( buffer );
	}
    else {
		result = ListEvalIntsubTop( buffer );	// List calc
	}
//	if ( ExecPtr < oplen ) { ExecPtr=execptr; CB_Error(SyntaxERR) ; } // Syntax error 
    ExecPtr=execptr;
	if ( ErrorNo ) { ErrorPtr=ExecPtr; return 0; }
	return result;
}
int CB_EvalStrInt2( char *buffer, int calcflag ) {		// Eval str -> Int
	int result;
	int execptr=ExecPtr;
	ExecPtr = 0;
    if ( calcflag == 0 ) {
		result = EvalIntsubTop( buffer );
	}
    else {
		result = ListEvalIntsubTop( buffer );	// List calc
	}
    ExecPtr=execptr;
	if ( ErrorNo ) { ErrorPtr=ExecPtr; return 0; }
	return result;
}

double CB_EvalStr( char *SRC, int calcflag ) {		// Exp(	
	double result;
	int c;
	int maxoplen;
	int	buffercnt=CB_StrBufferCNT;
	char *buffer;
	
	buffer = CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return
	if ( ErrorNo ) return 0;  // error
	CB_StrBufferCNT=buffercnt;
	result=CB_EvalStrDBL( buffer, calcflag );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return result;
}
int CBint_EvalStr( char *SRC, int calcflag ) {		// Exp(			Eval str -> int
	int result;
	int c;
	int maxoplen;
	int	buffercnt=CB_StrBufferCNT;
	char *buffer;
	
	buffer = CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return
	if ( ErrorNo ) return 0;  // error
	CB_StrBufferCNT=buffercnt;
	result=CB_EvalStrInt( buffer, calcflag );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return result;
}

char* CB_GraphYStrSub( char *SRC, int reg ) {	//  defaultGraphAry or  defaultFnAry
	double result;
	int dimA,dimB;
	int base=MatAry[reg].Base;
	dimA=Eval_atod( SRC, SRC[ExecPtr] );
	if ( ( dimA < base ) || ( dimA > MatAry[reg].SizeA-1+base ) ) { CB_Error(ArgumentERR); }  // Argument error
	return MatrixPtr( reg, dimA, base );
}
double CB_GraphYStr( char *SRC, int calcflag ) {	// defaultGraphAry
	char *ptr = CB_GraphYStrSub( SRC, defaultGraphAry ) ;
	if ( ErrorNo ) return 0;
	return CB_EvalStrDBL( ptr, calcflag );
}
double CB_FnStr( char *SRC, int calcflag ) {	// defaultFnAry
	char *ptr = CB_GraphYStrSub( SRC, defaultFnAry ) ;
	if ( ErrorNo ) return 0;
	return CB_EvalStrDBL2( ptr, calcflag );
}

int CBint_GraphYStr( char *SRC, int calcflag ) {	// defaultGraphAry
	char *ptr = CB_GraphYStrSub( SRC, defaultGraphAry ) ;
	if ( ErrorNo ) return 0;
	return CB_EvalStrInt( ptr, calcflag );
}
int CBint_FnStr( char *SRC, int calcflag ) {	// defaultFnAry
	char *ptr = CB_GraphYStrSub( SRC, defaultFnAry ) ;
	if ( ErrorNo ) return 0;
	if ( calcflag ) 
	return CB_EvalStrInt2( ptr, calcflag );
}

//----------------------------------------------------------------------------------------------
int CB_StrJoin( char *SRC ) {
	int maxoplen;
	char *CB_StrAddBuffer, *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	CB_StrAddBuffer=NewStrBuffer(); if ( CB_StrAddBuffer==NULL ) return 0;
	OpcodeCopy( CB_StrAddBuffer, buffer, CB_StrBufferMax-1 );		//

	buffer = CB_GetOpStr( SRC, &maxoplen ) ;		// String -> CB_CurrentStr
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	StrJoin( CB_StrAddBuffer, buffer, CB_StrBufferMax-1 ) ;
	CB_CurrentStr=CB_StrAddBuffer;
	return CB_StrBufferMax-1;
}

int CB_StrLeft( char *SRC ) {	// StrLeft( str1, n  )
	int n;
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	n = CB_EvalInt( SRC );	//
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	if ( n > 0 ) StrMid( CB_CurrentStr, buffer, 1, n ) ;
	else	CB_CurrentStr[0]='\0';
	return CB_StrBufferMax-1;
}
int CB_StrRight( char *SRC ) {	// StrRight( str1, n  )
	int n;
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return ;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	n = CB_EvalInt( SRC );	//
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	if ( n > 0 ) StrRight( CB_CurrentStr, buffer, n ) ;
	else	CB_CurrentStr[0]='\0';
	return CB_StrBufferMax-1;
}
int CB_StrMid( char *SRC ) {	// StrMid( str1, n [,m] )
	int n,m;
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	n = CB_EvalInt( SRC );	//
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		m = CB_EvalInt( SRC );	//
	} else m=-1;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	StrMid( CB_CurrentStr, buffer, n, m ) ;
	return CB_StrBufferMax-1;
}


int CB_StrLwr( char *SRC ) {	// StrLwr( str1 )
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	StrLwr( CB_CurrentStr, buffer ) ;
	return CB_StrBufferMax-1;
}
int CB_StrUpr( char *SRC ) {	// StrUpr( str1 )
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	StrUpr( CB_CurrentStr, buffer ) ;
	return CB_StrBufferMax-1;
}
int CB_StrInv( char *SRC ) {	// StrInv( str1 )
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	StrInv( CB_CurrentStr, buffer ) ;
	return CB_StrBufferMax-1;
}
int CB_StrShift( char *SRC ) {	// StrShift( str1 [,n] )
	int n;
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0;  // error
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		n = CB_EvalInt( SRC );	//
	} else n = 1;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	StrShift( CB_CurrentStr, buffer, n ) ;
	return CB_StrBufferMax-1;
}
int CB_StrRotate( char *SRC ) {	// StrRotate( str1 [,n] )
	int n;
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0 ;  // error
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		n = CB_EvalInt( SRC );	//
	} else n = 1;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	StrRotate( CB_CurrentStr, buffer, n ) ;
	return CB_StrBufferMax-1;
}

int CB_ExpToStr( char *SRC ) {	//  Exp->Str(
	int maxoplen;
	char *buffer;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0 ;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	CB_CurrentStr = buffer;
	CB_StorStr( SRC );
	return 1;
}

//----------------------------------------------------------------------------------------------
int CB_StrChar( char *SRC ) {	// StrChar("*"[,n])
	int n;
	int maxoplen,maxoplen2;
	char *buffer;
	char *buffer2;
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return ;  // error
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		n = CB_EvalInt( SRC );	//
	} else n = 1;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	if ( n > 0 ) StrChar( CB_CurrentStr, buffer, n) ;
	else	CB_CurrentStr[0]='\0';
	return CB_StrBufferMax-1;

}

int CB_StrCenter( char *SRC ) {	// StrCenter( Str1,max[,"SpaceChar"])
	int min,max;
	int maxoplen,maxoplen2;
	char *buffer;
	char *buffer2;
	int  charflag=0;
	char spc[]=" ";
	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return ;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	max = CB_EvalInt( SRC );	//
	if ( SRC[ExecPtr] == ',' ) { ExecPtr++;
		buffer2 = CB_GetOpStr( SRC, &maxoplen2 );
		charflag=1;
	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	if ( max > 0 ) StrCenter( CB_CurrentStr, buffer, max, charflag? buffer2:spc) ;
	else	CB_CurrentStr[0]='\0';
	return CB_StrBufferMax-1;
}

int CB_EvalToStr( char *SRC ){		// Str(
	double value = CB_EvalDbl( SRC );
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	sprintGR(CB_CurrentStr, value, CB_StrBufferMax-1, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return CB_StrBufferMax-1;
}
int CB_Hex( char *SRC ){		// Hex(
	int n,tmp;
	int value = CB_EvalInt( SRC );
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
//	sprintf(CB_CurrentStr, "%X",value);
	n=8;
	tmp=value;
	if (value) { while ( (tmp&0xF0000000)==0 ) { tmp=tmp<<4; n--; } } else n=1;
	if ( n<1 ) n=1;
	NumToHex( CB_CurrentStr, (unsigned int)value, n);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return CB_StrBufferMax-1;
}
int CB_Bin( char *SRC ){		// Bin(
	int n,tmp;
	int value = CB_EvalInt( SRC );
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	n=32;
	tmp=value;
	if (value) { while ( (tmp&0x80000000)==0 ) { tmp=tmp<<1; n--; } } else n=1;
	if ( n<1 ) n=1;
	NumToBin( CB_CurrentStr, (unsigned int)value, n);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return CB_StrBufferMax-1;
}

int CB_Sprintf( char *SRC ) {	// Ssprintf( "%4.4f %d %d", -1.2345,%123,%A)
	int maxoplen;
	int i,c;
	int type[3]={-1,-1,-1};	// 0:dbl  1:int  2:str
	double dblval[3]={0,0,0};
	int    intval[3]={0,0,0};
	char  *strval[3]={0,0,0};
	char *buffer;

	buffer = CB_GetOpStr( SRC, &maxoplen );
	if ( ErrorNo ) return 0 ;  // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;

	i=0;
	do {
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			strval[i]=CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return 
			type[i]=2;
		} else {	// expression
			c=SRC[ExecPtr];
			if ( CB_INT ) { 
				if ( c=='#' ) { type[i]=0; dblval[i]=CB_EvalDbl( SRC ); }
				else {
				if ( c=='%' ) ExecPtr++;
				type[i]=1; intval[i]=CB_EvalInt( SRC );
				}
			} else	{
				if ( c=='%' ) { ExecPtr++; type[i]=1; intval[i]=CB_EvalInt( SRC ); }
				else {
				if ( c=='#' ) ExecPtr++;
				type[i]=0; dblval[i]=CB_EvalDbl( SRC );
				}
			}
		}
		c=SRC[ExecPtr];
		if ( c != ',' ) break;
		 ExecPtr++;
		i++;
	} while ( i<3 );

	if ( c == ')' ) ExecPtr++;	
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	
	switch ( type[0] ) {
		case 0:			// dbl
			switch ( type[1] ) {
				case 0:		// dbl
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, dblval[0],dblval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, dblval[0],dblval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, dblval[0],dblval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, dblval[0],dblval[1]); break;
					} break;
				case 1:		// int
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1]); break;
					} break;
				case 2:		// str
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, dblval[0],intval[1]); break;
					} break;
				default:         i=sprintf( CB_CurrentStr, buffer, dblval[0]); break;
			} break;
		case 1:			// int
			switch ( type[1] ) {
				case 0:		// dbl
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, intval[0],dblval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, intval[0],dblval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, intval[0],dblval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, intval[0],dblval[1]); break;
					} break;
				case 1:		// int
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, intval[0],intval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, intval[0],intval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, intval[0],intval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, intval[0],intval[1]); break;
					} break;
				case 2:		// str
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, intval[0],strval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, intval[0],strval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, intval[0],strval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, intval[0],strval[1]); break;
					} break;
				default:         i=sprintf( CB_CurrentStr, buffer, intval[0]); break;
			} break;
		case 2:			// str
			switch ( type[1] ) {
				case 0:		// dbl
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, strval[0],dblval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, strval[0],dblval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, strval[0],dblval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, strval[0],dblval[1]); break;
					} break;
				case 1:		// int
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, strval[0],intval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, strval[0],intval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, strval[0],intval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, strval[0],intval[1]); break;
					} break;
				case 2:		// str
					switch ( type[2] ) {
						case 0:  i=sprintf( CB_CurrentStr, buffer, strval[0],strval[1],dblval[2]); break;
						case 1:  i=sprintf( CB_CurrentStr, buffer, strval[0],strval[1],intval[2]); break;
						case 2:  i=sprintf( CB_CurrentStr, buffer, strval[0],strval[1],strval[2]); break;
						default: i=sprintf( CB_CurrentStr, buffer, strval[0],strval[1]); break;
					} break;
				default:         i=sprintf( CB_CurrentStr, buffer, strval[0]); break;
			} break;
		default: i=0; break;
	}
	if ( i=0 ) { CB_Error(ArgumentERR); return 0; }	// Argument error

	i=-1;
	while ( i < CB_StrBufferMax ) {
		c=CB_CurrentStr[++i];
		if ( c == 0 ) break;
		switch ( c ) {
			case '-':
				CB_CurrentStr[i]=0x87;	// (-)
				break;
//			case '+':
//				CB_CurrentStr[i]=0x89;	// (+)
//				break;
//			case 'E':
//			case 'e':
//				CB_CurrentStr[i]=0x0F;	// (exp)
//				break;
		}
	}

	return CB_StrBufferMax-1;
}


//----------------------------------------------------------------------------------------------
int CB_StrDMS( char *SRC ) {
	double a,b,c,d;
	int i=0,j=3,f=1;
	
	if (CB_INT)	a = CBint_CurrentValue ;
	else		a = CB_CurrentValue    ;

	if ( a<0 ) { f=-1; a=-a; }
	b=floor(a);
	b=(a-b)*60.;
	c=floor(b);
	d=(b-c)*60.;
	
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error

	sprintf(CB_CurrentStr, "%d %02d  %05.2f", (int)a*f, (int)c, d);

	i=floor(log10(a));
	if ( i<0 ) i=0;
	if ( f<0 ) i++;

	if ( CB_CurrentStr[0] == '-' ) CB_CurrentStr[0]=0x87;	// (-)

	CB_CurrentStr[i+1]=0x9C;
	
	CB_CurrentStr[i+4]=0xE5;
	CB_CurrentStr[i+5]=0x96;

	if ( CB_CurrentStr[i+10] == '0' ) {
		j--;
		if ( CB_CurrentStr[i+9] == '0' ) j-=2;
	}

	CB_CurrentStr[ 8+i+j]=0xE5;
	CB_CurrentStr[ 9+i+j]=0x98;
	CB_CurrentStr[10+i+j]='\0';

	CB_StrPrint(SRC, 23-(10+i+j) );
}

int DateToStr( char *buffer) {	// "2017/01/17 TUE"
	int a, y1,y2,y3,y4, m1,m2, d1,d2;
	char weekStr[7][4]={"SAT","SUN","MON","TUE","WED","THU","FRI"};
//	char weekStr[7][4]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
	int	y,m,d,w,C,Y,r;

	a = GetDate();
	y1 = ( a >> 28 ) & 0xF ;
	y2 = ( a >> 24 ) & 0xF ;
	y3 = ( a >> 20 ) & 0xF ;
	y4 = ( a >> 16 ) & 0xF ;
	m1 = ( a >> 12 ) & 0xF ;
	m2 = ( a >>  8 ) & 0xF ;
	d1 = ( a >>  4 ) & 0xF ;
	d2 =   a         & 0xF ;

	y = y1*1000+y2*100+y3*10+y4;	// days of the week calculation (Zeller's congruence)
	m = m1*10+m2;
	d = d1*10+d2;
	if ( m <= 2 ) { m+=12; y--; }
	
	C = y / 100;
	Y = y % 100 ;
	if ( y >= 1582 ) r=5*C+(C/4); else r=6*C+5;
	w = ( d + (26*(m+1))/10 + Y + (Y/4) + r ) % 7;
//	w = ( y + y/4 - y/100 + y/400 + (13*m+8)/5 + d ) % 7;
	
	buffer[0]=y1+'0';
	buffer[1]=y2+'0';
	buffer[2]=y3+'0';
	buffer[3]=y4+'0';
	buffer[4]='/';
	buffer[5]=m1+'0';
	buffer[6]=m2+'0';
	buffer[7]='/';
	buffer[8]=d1+'0';
	buffer[9]=d2+'0';
	buffer[10]=' ';
	buffer[11]=weekStr[w][0];
	buffer[12]=weekStr[w][1];
	buffer[13]=weekStr[w][2];
	buffer[14]='\0';

	return 14;
}

int TimeToStr( char *buffer ) {	// "23:59:59"
	int a, h1,h2,  m1,m2, s1,s2;
	
	a = GetTime();
	h1 = ( a >> 20 ) & 0xF ;
	h2 = ( a >> 16 ) & 0xF ;
	m1 = ( a >> 12 ) & 0xF ;
	m2 = ( a >>  8 ) & 0xF ;
	s1 = ( a >>  4 ) & 0xF ;
	s2 =   a         & 0xF ;

	buffer[0]=h1+'0';
	buffer[1]=h2+'0';
	buffer[2]=':';
	buffer[3]=m1+'0';
	buffer[4]=m2+'0';
	buffer[5]=':';
	buffer[6]=s1+'0';
	buffer[7]=s2+'0';
	buffer[8]='\0';

	return 8 ;
}

int CB_DateToStr() {	// "2017/01/17 TUE"
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	DateToStr( CB_CurrentStr );
	return CB_StrBufferMax-1;
}

int CB_TimeToStr() {	// "23:59:59"
	CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;  // error
	TimeToStr( CB_CurrentStr );
	return CB_StrBufferMax-1;
}


//----------------------------------------------------------------------------------------------
//int StrObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int StrObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int StrObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int StrObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

