/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v1.10

 copyright(c)2015/2016/2017 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
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
#include "CB_Eval.h"
#include "CB_interpreter.h"
#include "CBI_Eval.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "CB_Matrix.h"
#include "CB_Str.h"
#include "CB_MonochromeLib.h"

//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> double
//----------------------------------------------------------------------------------------------
char ExpBuffer[ExpMax+1];

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

double ReadMatrix( int reg, int dimA, int dimB){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int		base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case 64:
			return MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array doubl
		case  2:		// Vram
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;			// Matrix array 1 bit
			return ( MatAryC[dimB*(((MatAry[reg].SizeA-1)>>3)+1)+(dimA>>3)] & ( 128>>(dimA&7) ) ) != 0 ;
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			return MatAryC[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array char
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			return MatAryW[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array word
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			return MatAryI[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array int
	}
}
//-----------------------------------------------------------------------------
//int EvalObjectAlignE4a( unsigned int n ){ return n; }	// align +4byte
//int EvalObjectAlignE4b( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

void WriteMatrix( int reg, int dimA, int dimB, double value){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int tmp;
	int mptr;
	int		base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case 64:
			MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB]  = (double)value ;	// Matrix array double
			break;
		case  2:		// Vram
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;					// Matrix array 1 bit
			tmp=( 128>>(dimA&7) );
			mptr=dimB*(((MatAry[reg].SizeA-1)>>3)+1)+((dimA)>>3);
			if ( value ) 	MatAryC[mptr] |= tmp ;
			else	 		MatAryC[mptr] &= ~tmp ;
			break;
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			MatAryC[dimA*MatAry[reg].SizeB+dimB] = (char)value ;	// Matrix array char
			break;
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			MatAryW[dimA*MatAry[reg].SizeB+dimB] = (short)value ;	// Matrix array word
			break;
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			MatAryI[dimA*MatAry[reg].SizeB+dimB] = (int)value ;		// Matrix array int
			break;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatrixOprandreg( char *SRC, int *reg) {	// 0-
	int c,d;
	if ( SRC[ExecPtr] == 0x7F ) { d=SRC[ExecPtr+1];
		switch ( d ) {
			case 0x40:
				ExecPtr+=2;	// skip 'Mat '
				break;
			case 0x51:		// 'List '
				ExecPtr+=2;
				*reg=ListRegVar( SRC );
				return 1;
			case 0x6A :		// List1
			case 0x6B :		// List2
			case 0x6C :		// List3
			case 0x6D :		// List4
			case 0x6E :		// List5
			case 0x6F :		// List6
				*reg=d+(58-0x6A); return 1;
			default:
				break;
		}
	}
	c =SRC[ExecPtr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		ExecPtr++;
		*reg=c-'A';
		if ( MatAry[*reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return 0; }	// No Matrix Array error
	} else {
		*reg=MatRegVar(SRC); 
		if ( *reg>=0 ) {
			if ( MatAry[*reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return 0; }	// No Matrix Array error
		} else {
			*reg=ListRegVar(SRC);
			if ( *reg>=0 ) {
				if ( MatAry[*reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return 0; }	// No Matrix Array error
			} else { CB_Error(SyntaxERR); return 0; }	// Syntax error
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------
int MatrixOprand( char *SRC, int *reg, int *dimA, int *dimB  ) {	// base:0  0-    base:1 1-
	int c;
	if ( MatrixOprandreg( SRC, &(*reg)) == 0 ) return 0 ;
	c =SRC[ExecPtr];
	if ( SRC[ExecPtr] != '[' ) { 
		(*dimA)=MatAry[(*reg)].Base;
		(*dimB)=(*dimA);	// Mat A  (no element)
		if  ( ( '0'<=c )&&( c<='9' ) ) {
			ExecPtr++;
			(*dimA)= c-'0';		// A0 A5 etc
			MatOprand1num( SRC, (*reg), &(*dimA), &(*dimB) );
		}
		return -1 ;
	}
	ExecPtr++;
	if ( CB_INT ) MatOprandInt2( SRC, *reg, &(*dimA), &(*dimB));else MatOprand2( SRC, *reg, &(*dimA), &(*dimB));
	return 1;
}
//-----------------------------------------------------------------------------
void MatOprand1num( char *SRC, int reg, int *dimA, int *dimB ){	// A0,A1,b3,c9 etc. error check
	int base,ElementSize=0;
	if ( MatAry[reg].SizeA == 0 ) {
		ElementSize=ElementSizeSelect( SRC, &base, ElementSize) & 0xFF;
		DimMatrixSub( reg, ElementSize, 10-MatBase, 1, MatBase );	// new matrix
		if ( ErrorNo ) return ; // error
	}
	base=MatAry[reg].Base;
	(*dimB)=base;
	if ( ( (*dimA) < (*dimB) ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) { 
		if ( MatAry[reg].SizeA < 10-base ) MatElementPlus( reg, 10-base, 1 );	// Mat element +
		else
		{ CB_Error(DimensionERR); return ; }	// Dimension error
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int RegVar( int c ) {
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) )  return c-'A' ;
	if ( c == 0xFFFFFFC0 ) return 28;		// Ans
	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) )	return c-0xFFFFFFCD+26 ;	// <r> or Theta
	return -1;
}

int GetVarName( char *SRC, int *ptr, char *name, int *len ){
	int i=0;
	int c=SRC[(*ptr)];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {		// 1st character
		name[i]=c;
		(*ptr)++;
		for ( i=1; i<(*len); i++) {
			c=SRC[(*ptr)];
			if ( ( ( '0'<=c )&&( c<='9' ) ) || ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) || ( c=='_' ) ) {
				name[i]=c;
				(*ptr)++;
			} else {
				name[i]='\0';
				break;
			}
		}
	} else return 0;	// not var name
	name[(*len)]='\0';
	(*len)=i;
	return 1;	// ok
}

int RegVarAliasEx( char *SRC ) {	// 
	int i,j,reg,len=32;
	int alias_code;
	int exptr=ExecPtr;
	int c=SRC[ExecPtr];
	char name[32+1];
	reg=RegVar(c); if ( reg>=0 ) { ExecPtr++; return reg; }
	if ( c=='_' ) { //	_ABCDE   var name
		ExecPtr++;
		if ( GetVarName( SRC, &ExecPtr, name, &len) ) {
			for ( i=0; i<=AliasVarMAX; i++ ) {
				if ( ( len == AliasVarCode[i].len ) && ( AliasVarCode[i].alias == 0x4040 ) ) {	// @@
					for (j=0; j<len; j++) {
						if ( AliasVarCode[i].name[j] != name[j] ) break;
					}
					if ( j==len ) return AliasVarCode[i].org;	// Macth!!
				}
			}
		}
			for ( i=0; i<=AliasVarMAXMat; i++ ) {	// check already Mat ?
				if ( ( len == AliasVarCodeMat[i].len ) && ( AliasVarCodeMat[i].alias == 0x4040 ) ) {	// @@
					for (j=0; j<len; j++) {
						if ( AliasVarCodeMat[i].name[j] != name[j] ) break;
					}
					if ( j==len ) return AliasVarCodeMat[i].org;	// Macth!!
				}
			}
		AliasVarMAX++; 	// New Var 
		IsExtVar++;
		if ( ( IsExtVar > VARMAXSIZE ) || ( AliasVarMAX > ALIASVARMAX ) ) { CB_Error(TooManyVarERR); return -1 ; } // Too Many Var ERR
		AliasVarCode[AliasVarMAX].org  =IsExtVar;
		AliasVarCode[AliasVarMAX].alias=0x4040;	// @@
		if ( len > 8 ) len=8;
		AliasVarCode[AliasVarMAX].len=len;
		memcpy( &AliasVarCode[AliasVarMAX].name[0], name, len );
		return IsExtVar;	// New Var !
//		{ CB_Error(UndefinedVarERR); return -1 ; } // Undefined Var ERR
	}
	ExecPtr += GetOpcodeLen( SRC, ExecPtr ,&alias_code );
	for ( i=0; i<=AliasVarMAX; i++ ) {
		if ( AliasVarCode[i].alias==(short)alias_code ) return AliasVarCode[i].org;
	}
	ExecPtr = exptr;
	return -1;
//	AliasVarMAX++;  	// New Var 
//	IsExtVar++;
//	if ( ( alias_code < 0xFF ) || ( AliasVarMAX > ALIASVARMAX ) ) { ExecPtr = exptr; return -1 ; } //  no var
//	if ( IsExtVar > VARMAXSIZE ) { CB_Error(TooManyVarERR); return -1 ; } // Too Many Var ERR
//	AliasVarCode[AliasVarMAX].org  =IsExtVar;
//	AliasVarCode[AliasVarMAX].alias=alias_code;
//	return IsExtVar;	// New Var !
}

int MatRegVar( char *SRC ) {	// 
	int i,j,reg,len=32;
	int alias_code, org_reg;
	int exptr=ExecPtr;
	int c=SRC[ExecPtr];
	char name[32+1];
	reg=RegVar(c); if ( reg>=0 ) { ExecPtr++; return reg; }
	if ( c=='@' ) {
		c=SRC[++ExecPtr];
		if ( c=='(' ) { 
			ExecPtr++; 
			reg=CB_EvalInt( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++ ;	// 
			goto jp1;
		}
		i=RegVar( c );
		if ( i>=0 ) { ExecPtr++;
			if ( CB_INT ) reg=LocalInt[i][0] ; else reg=LocalDbl[i][0] ;
			goto jp1;
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) { 
			goto jp0;
		}
	}
	if ( ( '0'<=c )&&( c<='9' ) ) { 
		  jp0:
			reg=Eval_atoi( SRC, c );
		  jp1:
			if ( ( reg<1 ) || ( ExtListMax<reg ) ) { CB_Error(ArgumentERR); return -1 ; } // Argument error
			if ( 26<reg ) reg+=6;
			if ( 58<reg ) reg+=26;
			return reg-1 ;
	}
	if ( c=='_' ) { //	_ABCDE   var name
		ExecPtr++;
		if ( GetVarName( SRC, &ExecPtr, name, &len) ) {
			for ( i=0; i<=AliasVarMAXMat; i++ ) {
				if ( ( len == AliasVarCode[i].len ) && ( AliasVarCode[i].alias == 0x4040 ) ) {	// @@
					for (j=0; j<len; j++) {
						if ( AliasVarCodeMat[i].name[j] != name[j] ) break;
					}
					if ( j==len ) return AliasVarCodeMat[i].org;	// Macth!!
				}
			}
		}
		{ CB_Error(UndefinedVarERR); return -1 ; } // Undefined Var ERR
	}
	ExecPtr += GetOpcodeLen( SRC, ExecPtr ,&alias_code );
	for ( i=0; i<=AliasVarMAXMat; i++ ) {
		if ( AliasVarCodeMat[i].alias==(short)alias_code ) return AliasVarCodeMat[i].org;
		if ( AliasVarCodeMat[i].org<0 ) break;
	}
	ExecPtr = exptr;
	return -1;
}
int ListRegVar( char *SRC ) {	// return reg no
	int c;
	int	reg;
	c=SRC[ExecPtr++];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		if ( CB_INT ) reg=LocalInt[c-'A'][0] ; else reg=LocalDbl[c-'A'][0] ;
		goto jp1;
	}
	if ( c == 0xFFFFFFC0 ) return 28;		// Ans
	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) )	return c-0xFFFFFFCD+26 ;	// <r> or Theta
	if ( c=='(' ) { 
		reg=CB_EvalInt( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++ ;	// 
		goto jp0;
	}
	ExecPtr--;
	reg=Eval_atoi( SRC, c );
  jp0:
	if ( ( reg<1 ) || ( ExtListMax<reg ) ) { CB_Error(ArgumentERR); return -1 ; } // Argument error
  jp1:
  	if ( reg<=52 ) {
		if ( 27<=reg ) return reg+5;
		return reg+57;
	} else return reg+31;
}

//-----------------------------------------------------------------------------
int MatOperandSub( int c ) {
	if  ( ( '0'<=c )&&( c<='9' ) ) return c-'0';
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) )  return LocalDbl[c-'A'][0] ;
	if ( c == 0xFFFFFFC0 ) return 28;		// Ans
	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) )	return LocalDbl[c-0xFFFFFFCD+26][0] ;	// <r> or Theta
	CB_Error(SyntaxERR);
	return -1 ; 	// Syntax error
}
void MatOprand1sub( char *SRC, int reg, int *dimA ){	// base:0  0-    base:1 1-
	int c,d,e;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( (d==',')||(d==']')||(d==0x0E)||(d==0x13)||(d==0x0D)||(d==0) )  {		// [a,
		ExecPtr++ ;
		(*dimA) = MatOperandSub( c );
	} else
	if ( d == 0xFFFFFF89 ) { 										// [a+1,
		e=SRC[ExecPtr+3];
		if ( (e==',')||(e==']')||(e==0x0E)||(e==0x13)||(e==0x0D)||(e==0) ) {
			ExecPtr+=2 ;
			(*dimA) = MatOperandSub( c );
			c=SRC[ExecPtr++];
			(*dimA) += MatOperandSub( c );
		} else goto L1;
	} else
	if ( d == 0xFFFFFF99 ) { 										// [a-1,
		e=SRC[ExecPtr+3];
		if ( (e==',')||(e==']')||(e==0x0E)||(e==0x13)||(e==0x0D)||(e==0) ) {
			ExecPtr+=2 ;
			(*dimA) = MatOperandSub( c );
			c=SRC[ExecPtr++];
			(*dimA) -= MatOperandSub( c );
		} else goto L1;
	} else {
  L1:	(*dimA)=(EvalsubTop( SRC ));
	}
}
void MatOprand2( char *SRC, int reg, int *dimA, int *dimB ){	// base:0  0-    base:1 1-
	int c,d;
	int base=MatAry[reg].Base;
	MatOprand1sub( SRC, reg, &(*dimA) );
	if ( ( (*dimA) < base ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	if ( SRC[ExecPtr] == ',' ) {
		ExecPtr++ ;
		MatOprand1sub( SRC, reg, &(*dimB) );
		if ( ( (*dimB) < base ) || ( MatAry[reg].SizeB-1+base < (*dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	} else {
		(*dimB)=base;
	}
	if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
}
void MatOprand1( char *SRC, int reg, int *dimA, int *dimB ){	// base:0  0-    base:1 1-
	int c,d;
	int base;
	MatOprand1sub( SRC, reg, &(*dimA) );
	if ( MatAry[reg].SizeA == 0 ) { 
		DimMatrixSub( reg, CB_INT? 32:64, 10-MatBase, 1, MatBase );	// new matrix
		if ( ErrorNo ) return ; // error
	}
	base=MatAry[reg].Base;
	(*dimB)=base;
	if ( ( (*dimA) < base ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) {
//		if ( MatAry[reg].SizeA < 10-base ) MatElementPlus( reg, 10-base, 1 );	// List element +
//		else 
		if ( (MatAry[reg].SizeA+1)==((*dimA)-1+base) ) MatElementPlus( reg, (*dimA)-1+base, 1 );	// List element +
		else
		{ CB_Error(DimensionERR); return ; }	// Dimension error
	}
	if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
}

//----------------------------------------------------------------------------------------------
int EvalObjectAlignE4d( unsigned int n ){ return n+n; }	// align +6byte
int EvalObjectAlignE4e( unsigned int n ){ return n; }	// align +4byte
int EvalObjectAlignE4f( unsigned int n ){ return n; }	// align +4byte
int EvalObjectAlignE4g( unsigned int n ){ return n; }	// align +4byte
//int EvalObjectAlignE4h( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------
double CB_EvalDbl( char *SRC ) {
	double value;
	if (CB_INT) {
		if ( SRC[ExecPtr]=='#' ) { ExecPtr++;
			value=EvalsubTop( SRC ); 
		}
		else value=EvalIntsubTop( SRC ); 
	} else	 value=EvalsubTop( SRC ); 	
	return value;
}

int EvalEndCheck( int c ) {
	if ( c==':' ) return c;
	if ( c==0x0E ) return c;	// ->
	if ( c==0x13 ) return c;	// =>
	if ( c==',' ) return c;
	if ( c==')' ) return c;
	if ( c==']' ) return c;
	if ( c=='}' ) return c;
	if ( c==0x0D ) return c;	// <CR>
	if ( c==0x0C ) return c;	// <Dsps>
	return ( c==0 );
}

double EvalsubTop( char *SRC ) {	// eval 1
	double  result,dst;
	int c;
	int excptr=ExecPtr;
	int ansreg=CB_MatListAnsreg;

//	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	result=Evalsub1(SRC);
	c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result;
	else 
	if ( c==0xFFFFFF89 ) { // +
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result+dst;
	} else
	if ( c==0xFFFFFF99 ) { // -
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result-dst;
	} else
	if ( c=='=') { // ==
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result == dst;
	} else
	if ( c=='>') { // >
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result > dst;
	} else
	if ( c=='<') { // <
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result < dst;
	} else
	if ( c==0x11) { // !=
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result != dst;
	} else
	if ( c==0x12) { // >=
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result >= dst;
	} else
	if ( c==0x10) { // <=
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result <= dst;
	} else
	if ( c==0xFFFFFFA9 ) { // *
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result*dst;
	} else
	if ( c==0xFFFFFFB9 ) { // /
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return fDIV(result,dst);
	} else
	if ( c==0xFFFFFF9A ) { // xor
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result ^ (int)dst;
	} else
	if ( ( c=='|' ) || ( c==0xFFFFFFAA ) ) { // or
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result | (int)dst;
	} else
	if ( ( c=='&' ) || ( c==0xFFFFFFBA ) ) { // and
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result & (int)dst;
	} else
	if ( c==0xFFFFFF8B ) { // ^2
		ExecPtr++;
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result*result;
	} else
	if ( c==0xFFFFFF9B ) { // ^(-1) RECIP
		ExecPtr++;
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return fDIV(result,1);
	} else
	if ( c==0x7F ) { // 
		c=SRC[++ExecPtr];
		if ( c==0xFFFFFFB0 ) { // And
			ExecPtr++; dst=Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result && (int)dst;
		} else
		if ( c==0xFFFFFFB1 ) { // Or
			ExecPtr++; dst=Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result || (int)dst;
		} else
		if ( c==0xFFFFFFB4 ) { // Xor
			ExecPtr++; dst=Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return ((int)result!=0) ^ ((int)dst!=0);
		} else
		if ( c==0xFFFFFFBC ) { // Int/
			ExecPtr++; dst=Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return fIDIV(result,dst);
		} else
		if ( c==0xFFFFFFBD ) { // Rmdr
			ExecPtr++; dst=Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return fMOD(result,dst);
		}
	}
	
	ExecPtr=excptr;
	CB_MatListAnsreg=ansreg;
	return Evalsub14( SRC );
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

double frac( double x ) {
	double sign=1,tmp,d;
	if ( x <0 ) { sign=-1; x=-x; }
	if ( x<1e1 ) d= 1e15;
	else if ( x>=1e15 ) d= 1;
	else if ( x>=1e14 ) d= 1e1;
	else if ( x>=1e13 ) d= 1e2;
	else if ( x>=1e12 ) d= 1e3;
	else if ( x>=1e11 ) d= 1e4;
	else if ( x>=1e10 ) d= 1e5;
	else if ( x>=1e9 ) d= 1e6;
	else if ( x>=1e8 ) d= 1e7;
	else if ( x>=1e7 ) d= 1e8;
	else if ( x>=1e6 ) d= 1e9;
	else if ( x>=1e5 ) d= 1e10;
	else if ( x>=1e4 ) d= 1e11;
	else if ( x>=1e3 ) d= 1e12;
	else if ( x>=1e2 ) d= 1e13;
	else if ( x>=1e1 ) d= 1e14;
	tmp=x-floor(x);
	return (floor(tmp*d+.5)/d*sign) ;
}

double fdegree( double x ) {	//	Rad,Grad ->Deg
	switch ( Angle ) { 
		case 0:	// Deg
			return x;
		case 1:	// Rad
			return x*(180./PI);
		case 2:	// Grad
			return x*(180/200);
	}
}
double finvdegree( double x ) {	//	Deg -> Rad,Grad
	switch ( Angle ) { 
		case 0:	// Deg
			return x;
		case 1:	// Rad
			return x*(PI/180.);
		case 2:	// Grad
			return x*(200./180.);
	}
}
double fgrad( double x ) {		//	Deg,Rad -> Grad
	switch ( Angle ) { 
		case 0:	// Deg
			return x*(180./200.);
		case 1:	// Rad
			return x*(PI/200.);
		case 2:	// Grad
			return x;
	}
}
double finvgrad( double x ) {		//	Grad -> Deg,Rad
	switch ( Angle ) { 
		case 0:	// Deg
			return x*(180./200.);
		case 1:	// Rad
			return x*(PI/200.);
		case 2:	// Grad
			return x;
	}
}

double fradian( double x ) {	//	Deg,Grad ->Rad
	switch ( Angle ) { 
		case 0:	// Deg
			return x*(PI/180.);
		case 1:	// Rad
			return x;
		case 2:	// Grad
			return x*(PI/200.);
	}
}
double finvradian( double x ) {	//	Rad -> Deg,Grad
	switch ( Angle ) { 
		case 0:	// Deg
			return x*(180./PI);
		case 1:	// Rad
			return x;
		case 2:	// Grad
			return x*(200./PI);
	}
}
double fsin( double x ) {
	x = sin( fradian(x) );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double fcos( double x ) {
	x = cos( fradian(x) );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double ftan( double x ) {
	x = tan( fradian(x) );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double fasin( double x ) {
	x = finvradian( asin(x) );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double facos( double x ) {
	x = finvradian( acos(x) );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double fatan( double x ) {
	x = finvradian( atan(x) );
	CheckMathERR(&x); // Math error ?
	return x ;
}

double fpolr( double x, double y ) {	// Pol(x,y) -> r
	x = sqrt(x*x+y*y);
	CheckMathERR(&x); // Math error ?
	return x ;
}
double fpolt( double x, double y ) {	// Pol(x,y) -> Theta
	if ( ( x==0 ) && ( y==0 ) ) return 0;
	if ( x != 0 ) x = fatan(y/x);
	else x = 2*fatan(1);
	CheckMathERR(&x); // Math error ?
	return x ;
}
double frecx( double r, double t ) {	// Rec(r,Theta) -> x
	r = r*fcos(t);
	CheckMathERR(&r); // Math error ?
	return r ;
}
double frecy( double r, double t ) {	// Rec(r,Theta) -> y
	r = r*fsin(t);
	CheckMathERR(&r); // Math error ?
	return r ;
}

void CheckMathERR( double *result ) {
	char * pt;
	pt=(char *)(result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
}

double asinh( double x ) {
	x = ( (exp(x)+exp(-x))/2. );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double acosh( double x ) {
	x = ( (exp(x)-exp(-x))/2. );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double atanh( double x ) {
	double ep=exp(x);
	double em=exp(-x);
	x = ( (ep-em)/(ep+em) );
	CheckMathERR(&x); // Math error ?
	return x ;
}
double fint( double x ) {
	if ( x >= 0 ) return floor(x);
	return -floor(-x);
}
double fnot( double x ) {
	return ! (int) ( x );
}
double fsqrt( double x ) {
	x = sqrt( x );
	CheckMathERR(&x); // Math error ?
	return x;
}
double flog10( double x ) {
	x = log10( x );
	CheckMathERR(&x); // Math error ?
	return x;
}
double fpow10( double x ) {
	x = pow( 10, x );
	CheckMathERR(&x); // Math error ?
	return x;
}
double fln( double x ) {
	x = log( x );
	CheckMathERR(&x); // Math error ?
	return x;
}
double fexp( double x ) {
	x = exp( x );
	CheckMathERR(&x); // Math error ?
	return x;
}
double fcuberoot( double x ) {
	x = pow( x, 1.0/3.0 );
	CheckMathERR(&x); // Math error ?
	return x;
}
double fsqu( double x ) {
	return x*x;
}
double ffact( double x ) {
	double tmp;
	tmp = floor( x );
	if ( ( tmp < 0 ) || ( 170 < tmp ) ) { CB_Error(MathERR) ; return 0; } // Math error
	x = 1;
	while ( tmp > 0 ) { x *= tmp; tmp--; }
	CheckMathERR(&x); // Math error ?
	return x;
}
double frecip( double x ) {	// ^(-1) RECIP
	if ( x == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
	return 1 / x ;
}
double ffemto( double x ) {	// femto
	return x * 1e-15 ;
}
double fpico( double x ) {	// pico
	return x * 1e-12 ;
}
double fnano( double x ) {	// nano
	return x * 1e-9 ;
}
double fmicro( double x ) {	// micro
	return x * 1e-6 ;
}
double fmilli( double x ) {	// milli
	return x * 1e-3 ;
}
double fKiro( double x ) {	// Kiro
	return x * 1e3 ;
}
double fMega( double x ) {	// Mega
	return x * 1e6 ;
}
double fGiga( double x ) {	// Giga
	return x * 1e9 ;
}
double fTera( double x ) {	// Tera
	return x * 1e12 ;
}
double fPeta( double x ) {	// Peta
	return x * 1e15 ;
}
double fExa( double x ) {	// Exa
	return x * 1e18 ;
}

double fsign( double x ) {	// -x
	return -x;
}
double fADD( double x, double y ) {	// x + y
	return x+y;
}
double fSUB( double x, double y ) {	// x - y
	return x-y;
}
double fMUL( double x, double y ) {	// x * y
	return x*y;
}
double fDIV( double x, double y ) {	// x / y
	if ( y == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
	return x/y;
}
double fAND( double x, double y ) {	// x and y
	return (int)x & (int)y;
}
double fOR( double x, double y ) {	// x or y
	return (int)x | (int)y;
}
double fXOR( double x, double y ) {	// x xor y
	return (int)x ^ (int)y;
}
double fNot( double x ) {	// Not x
	return ~(int)x;
}
double fAND_logic( double x, double y ) {	// x && y
	return x && y;
}
double fOR_logic( double x, double y ) {	// x || y
	return x || y;
}
double fXOR_logic( double x, double y ) {	// x xor y
	return (x!=0) ^ (y!=0);
}
double fNot_logic( double x ) {	// Not x
	return x==0;
}
double fcmpEQ( double x, double y ) {	//  x = y
	return x == y;
}
double fcmpGT( double x, double y ) {	//  x > y
	return x > y;
}
double fcmpLT( double x, double y ) {	//  x < y
	return x < y;
}
double fcmpNE( double x, double y ) {	//  x != y
	return x != y;
}
double fcmpGE( double x, double y ) {	//  x >= y
	return x >= y;
}
double fcmpLE( double x, double y ) {	//  x <= y
	return x <= y;
}

double fpow( double x, double y ) {	// pow(x,y)
	x = pow( x, y );
	CheckMathERR(&x); // Math error ?
	return x;
}
double fpowroot( double x, double y ) {	// powroot(x,y)
	if ( y == 0 ) { CB_Error(MathERR) ; return 0; } // Math error
	x = pow( x, 1/y );
	CheckMathERR(&x); // Math error ?
	return x;
}

void fDIVcheck( double *x, double *y ) {	//
	double tmp,tmp2,result;
	(*x)  = floor( (*x) +.5);
	(*y) = floor( (*y) +.5);
	if ( (*y) == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
}

double fMOD( double x, double y ) {	// fMOD(x,y)
	double result;
	fDIVcheck( &x, &y );
	result= floor(fabs(fmod( x, y ))+.5);
	if ( result == y  ) result--;
	if ( x < 0 ) {
		result=fabs(y)-result;
		if ( result == y  ) result=0;
	}
	return result ;
}
double fIDIV( double x, double y ) {	// (int)x / (int)y
	double result;
	fDIVcheck( &x, &y );
	return floor((x/y)+.5);
}
double flogab( double x, double y ) {	// flogab(x,y)
	double base,tmp,result;
	if ( x <= 0 ) { CB_Error(MathERR) ; return 0; } // Math error
	base  = log(x);
	result = log(y)/base;
	CheckMathERR(&result); // Math error ?
	return result ;
}
double frand() {
	return (double)rand()/(double)(RAND_MAX+1.0);
}

double fGCD( double x, double y ) {	// GCD(x,y)
	double tmp;
	if ( x<y ) { tmp=x; x=y; y=tmp; }
	tmp=fMOD(x,y);
	while( tmp != 0 ) {
		x=y;
		y=tmp;
		tmp=fMOD(x,y);
	}
	return y;
}
double fLCM( double x, double y ) {	// LCM(x,y)
	if ( ( x < 0 ) || ( x < 0 ) ) { CB_Error(ArgumentERR) ; return 0; } // Argumenterror
	return x/fGCD(x,y)*y;
}

double fRanNorm( double sd, double mean) {	// RanNorm#
	double a1,a2,b,g1;
	do {
		a1 = 2.0*(double)rand()/(double)RAND_MAX-1.0;
		a2 = 2.0*(double)rand()/(double)RAND_MAX-1.0;
		b = a1*a1+a2*a2;
	} while ( b>=1.0 );
	b = sqrt( ( -2.0*log(b) ) / b );
	g1 = a1*b;
	return g1*sd+mean;
}
double fRanBin( double n, double p) {	// RanBin#
	double a;
	int i,r,m=0;
	if ( ( n > 1000000 ) || ( p < 0 ) || ( p > 1 ) ) { CB_Error(MathERR) ; return 0; } // Math error
	r=(RAND_MAX+1.0)*p;
	for ( i=0; i<n; i++ ) if ( rand() <= r ) m++;
	return m;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int EvalObjectAlignE4ee( unsigned int n ){ return n ; }	// align +4byte
int EvalObjectAlignE4ff( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

unsigned int Eval_atofNumDiv(char *SRC, int c, double *num ){
	double a=.1;
	while ( ( '0'<=c )&&( c<='9' ) ) {
		(*num) = (*num) + (double)(c-'0')*a;
		a*=.1;
		c=SRC[++ExecPtr];
	}
	return c;
}
unsigned int Eval_atofNumMult(char *SRC, int c, double *num ){
	while ( ( '0'<=c )&&( c<='9' ) ) {
		(*num) = (*num)*10 +(c-'0');
		c=SRC[++ExecPtr];
	}
	return c;
}
double Eval_atof(char *SRC, int c) {
	double mantissa=0,exponent=0;
	double sign=1;
	int d;
		if ( c == '.'  )   {	// .123456
				c = SRC[++ExecPtr];
				c=Eval_atofNumDiv(SRC, c, &mantissa);
		} else if ( c == 0x0F  ) {  // exp
				mantissa = 1.0;
				goto lblexp;
		} else { 	// 123456
			if ( c == '0' ) {
				d = SRC[ExecPtr+1];
				if ( (  d=='x' ) || ( d=='X' ) || (  d=='b' ) || ( d=='B' ) ) return Eval_atoi( SRC, c );
			}
			c=Eval_atofNumMult(SRC, c, &mantissa);	// 123456
			if ( c == '.'  ) {
				c = SRC[++ExecPtr];
				c=Eval_atofNumDiv(SRC, c, &mantissa);	// 123456.789
			}
		}
		lblexp:
		if ( ( c == 0x0F  ) || ( c == 'e'  ) ){ // exp
				c=SRC[++ExecPtr];
				if ( ( c == 0xFFFFFF89 ) || ( c == '+'  ) ) c=SRC[++ExecPtr];
				if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) { sign=-1; c=SRC[++ExecPtr]; }	// (-) & -
				if ( ( '0'<=c )&&( c<='9' ) ) c=Eval_atofNumMult(SRC, c, &exponent);
				else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; } //  error 
			return mantissa * pow(10,exponent*sign) ;
		} else
			return mantissa ;
}

//-----------------------------------------------------------------------------

int Get2Eval( char *SRC, double *tmp, double *tmp2){
	int c;
	(*tmp) = EvalsubTop( SRC );
	if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
	ExecPtr++;
	(*tmp2) = EvalsubTop( SRC );
	c=SRC[ExecPtr];
	if ( c == ')' ) ExecPtr++;
	return c;
}

double Evalsub1(char *SRC) {	// 1st Priority
	double result=0,tmp,tmp2;
	double sign=1;
	int c,d;
	char *pt;
	int dimA,dimB,reg,x,y;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryF;

	c = SRC[ExecPtr++];
	if ( c == '(') {
		result = EvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		result = - Evalsub1( SRC );
		return result;
	}
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) { ExecPtr++; return LocalInt[reg][0] ; }
		else
		if ( c=='[' ) { goto Matrix; }
		else
		if ( ( '0'<=c )&&( c<='9' ) ) {
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
		} else
		if ( c=='#' ) { ExecPtr++; return LocalDbl[reg][0] ; }
		if ( CB_INT) return LocalInt[reg][0] ; else return LocalDbl[reg][0] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		ExecPtr--; return Eval_atof( SRC , c );
	}
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0x40 :		// Mat A[a,b]
				  Matjmp:
					c=SRC[ExecPtr];
					if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) { reg=c-'A'; ExecPtr++; } 
					else { reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
					Matrix1:	
					if ( SRC[ExecPtr] == '[' ) {
					Matrix:	
						ExecPtr++;
						MatOprand2( SRC, reg, &dimA, &dimB );
					Matrix2:
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=3;	// Mat A
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrix( reg, dimA, dimB);
						
				case 0x51 :		// List 1~26
					reg=ListRegVar( SRC );
				  Listj:
					if ( SRC[ExecPtr] == '[' ) {
						ExecPtr++;
						MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=4;	// List 1
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrix( reg, dimA, dimB);
					
				case 0x6A :		// List1
				case 0x6B :		// List2
				case 0x6C :		// List3
				case 0x6D :		// List4
				case 0x6E :		// List5
				case 0x6F :		// List6
					reg=c+(58-0x6A); goto Listj;
						
				case 0x3A :				// MOD(a,b)
					Get2Eval( SRC, &tmp, &tmp2);
					return fMOD(tmp,tmp2);
					
				case 0x3C :				// GCD(a,b)
					Get2Eval( SRC, &tmp, &tmp2);
					return fGCD(tmp,tmp2);

				case 0x3D :				// LCM(a,b)
					Get2Eval( SRC, &tmp, &tmp2);
					return fLCM(tmp,tmp2);
					
				case 0xFFFFFF85 :		// logab(a,b)
					Get2Eval( SRC, &tmp, &tmp2);
					return flogab(tmp,tmp2);
					
				case 0xFFFFFFB3 :		// Not
					return ( Evalsub5( SRC ) == 0 ) ;
						
				case 0xFFFFFF9F :		// KeyRow(
					return CB_KeyRow( SRC ) ; 
				case 0xFFFFFF8F :		// Getkey
					return CB_GetkeyEntry( SRC );
					
				case 0xFFFFFF87 :		// RanInt#(st,en[,n])
					if ( Get2Eval( SRC, &tmp, &tmp2) == ',' ) {
						ExecPtr++;
						CB_RanInt( SRC, tmp, tmp2 );
					}
					return frandIntint( tmp, tmp2 ) ;
					
				case 0xFFFFFF88 :		// RanList#(n) ->ListAns
					CB_RanList( SRC ) ;
					return 4 ;
				case 0xFFFFFF89 :		// RanBin#(n,p[,m]) ->ListAns
					return CB_RanBin( SRC ) ;
				case 0xFFFFFF8A :		// RanNorm#(sd,mean[,n]) ->ListAns
					return CB_RanNorm( SRC ) ;
					
				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cellsum( reg, x, y );
	
				case 0x5F :				// 1/128 Ticks
					return CB_Ticks( SRC );	// 
					
				case 0xFFFFFF86 :		// RndFix(n,digit)
					tmp=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					i = EvalsubTop( SRC ) +.5;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result=Round( tmp, Fix, i) ;
					return result ;
						
				case 0xFFFFFFF0 :		// GraphY str
					return CB_GraphYStr( SRC, 0 );
						
				case 0xFFFFFFF5 :		// IsExist(
					return  CB_IsExist( SRC ,0 );
				case 0xFFFFFFF6 :		// Peek(
					return  CB_Peek( SRC, EvalsubTop( SRC ) );
				case 0xFFFFFFF8 :		// VarPtr(
					return  CB_VarPtr( SRC );
				case 0xFFFFFFFA :		// ProgPtr(
					return  CB_ProgPtr( SRC );
				case 0x00 :				// Xmin
					return Xmin;
				case 0x01 :				// Xmax
					return Xmax;
				case 0x02 :				// Xscl
					return Xscl;
				case 0x04 :				// Ymin
					return Ymin;
				case 0x05 :				// Ymax
					return Ymax;
				case 0x06 :				// Yscl
					return Yscl;
				case 0x08 :				// Thetamin
					return TThetamin;
				case 0x09 :				// Thetamax
					return TThetamax;
				case 0x0A :				// Thetaptch
					return TThetaptch;
				case 0x0B :				// Xfct
					return Xfct;
				case 0x0C :				// Yfct
					return Yfct;
	
				case 0x29 :				// Sigma( X, X, 1, 1000)
					return CB_Sigma( SRC );
				case 0x20 :				// Max( List 1 )	Max( { 1,2,3,4,5 } )
					return CB_MinMax( SRC, 1 );
				case 0x2D :				// Min( List 1 )	Min( { 1,2,3,4,5 } )
					return CB_MinMax( SRC, 0 );
				case 0x2E :				// Mean( List 1 )	Mean( { 1,2,3,4,5 } )
					return CB_Mean( SRC );
				case 0x4C :				// Sum List 1
					return CB_Sum( SRC );
				case 0x4D :				// Prod List 1
					return CB_Prod( SRC );
				case 0x47:	// Fill(
					CB_MatFill(SRC);
					return 3;
				case 0x49:	// Argument(
					CB_Argument(SRC);
					return 3;
				case 0x2C:	// Seq
					CB_Seq(SRC);
					return 4;
				case 0x41:	// Trn
					CB_MatTrn(SRC);
					return 3;

				case 0x46 :				// Dim
					result=CB_Dim( SRC );
					if ( result >= 0 ) return result;
					ExecPtr--;	// error
					break;
				case 0x58 :				// ElemSize( Mat A )
					return CB_ElemSize( SRC );
				case 0x59 :				// RowSize( Mat A )
					return CB_RowSize( SRC );
				case 0x5A :				// ColSize( Mat A )
					return CB_ColSize( SRC );
				case 0x5B :				// MatBase( Mat A )
					return CB_MatBase( SRC );
					
				case 0x5C :				// ListCmp( List 1, List 2)
					return CB_ListCmp( SRC );
					
				case 0x4A :				// List>Mat( List 1, List 2,..) -> List 5
					CB_List2Mat( SRC );
					return 0;
				case 0x4B :				// Mat>List( Mat A, m) -> List n
					CB_Mat2List( SRC );;
					return 0;
				default:
					ExecPtr--;	// error
					break;
			}
			break;
//		case 0xFFFFFFC0 :	// Ans
//			return CB_CurrentValue ;
		case '&' :	// & VarPtr
			return CB_VarPtr( SRC ) ;
		case 0xFFFFFFD0 :	// PI
			return PI ;
		case 0xFFFFFFC1 :	// Ran#
			return CB_frand( SRC );
		case 0xFFFFFF97 :	// abs
			result = fabs( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
			result = Evalsub5( SRC );
			if ( result >= 0 ) goto intg;
			return -floor(-result);
		case 0xFFFFFFDE :	// intg
			result = Evalsub5( SRC );
			intg:
			return floor(result) ;
		case 0xFFFFFFB6 :	// frac
			result = frac( Evalsub5( SRC ) );
			return result ;
//		case 0xFFFFFFA7 :	// Not
//			result = ! (int) ( Evalsub5( SRC ) );
//			return result ;
		case  0xFFFFFFAB  :	//  ! Not
			result = ! (int) ( Evalsub5( SRC ) );
			return result ;

		case '%' :	// 1/128 Ticks
			return CB_Ticks( SRC );	// 
		case '*' :	// peek
			return CB_Peek( SRC, Evalsub1( SRC ) );	// 
//		case '@' :	// Mat @A
//			ExecPtr--;
//			goto Matjmp;
			
		case '{':	// { 1,2,3,4,5... }->List Ans
			CB_List(SRC);
			return 0;
		case '[':	// [ [0,1,2][2,3,4] ]->Mat Ans
			CB_Matrix(SRC);
			return 0;

		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					y=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result = PxlTest(y, x) ;			// 
					return result ;
				case 0xFFFFFFB0 :				// SortA( List 1)
					return CB_EvalSortAD( SRC, 1 );
				case 0xFFFFFFB1 :				// SortD( List 1)
					return CB_EvalSortAD( SRC, 0 );
				case 0xFFFFFFF4:	// SysCall(
					return  CB_SysCall( SRC );
				case 0xFFFFFFF5:	// Call(
					return  CB_Call( SRC );
				case 0xFFFFFFF8:	// RefreshCtrl
					return  RefreshCtrl;
				case 0xFFFFFFFA:	// RefreshTime
					return  Refreshtime+1;
				case 0xFFFFFFFB:	// Screen
					return  ScreenMode;
				case 0xFFFFFFFE:	// BackLight
					return	BackLight(-1);
				case 0xFFFFFFF2:	// PopUpWin(
					return CB_PopUpWin(SRC);
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFF86 :	// sqr
			return fsqrt( Evalsub5( SRC ) );
		case 0xFFFFFF95 :	// log10
			return flog10( Evalsub5( SRC ) );
		case 0xFFFFFFB5 :	// 10^
			return fpow10( Evalsub5( SRC ) );
		case 0xFFFFFF85 :	// ln
			return fln( Evalsub5( SRC ) );
		case 0xFFFFFFA5 :	// expn
			return fexp( Evalsub5( SRC ) );
		case 0xFFFFFF96 :	// cuberoot
			return fcuberoot( Evalsub5( SRC ) );

		case 0xFFFFFF81 :	// sin
			return fsin( Evalsub5( SRC ) );
		case 0xFFFFFF82 :	// cos
			return fcos( Evalsub5( SRC ) );
		case 0xFFFFFF83 :	// tan
			return ftan( Evalsub5( SRC ) );
		case 0xFFFFFF91 :	// asin
			return fasin( Evalsub5( SRC ) );
		case 0xFFFFFF92 :	// acos
			return facos( Evalsub5( SRC ) );
		case 0xFFFFFF93 :	// atan
			return fatan( Evalsub5( SRC ) );
		case 0xFFFFFFA1 :	// sinh
			return  sinh( Evalsub5( SRC ) );
		case 0xFFFFFFA2 :	// cosh
			return  cosh( Evalsub5( SRC ) );
		case 0xFFFFFFA3 :	// tanh
			return  tanh( Evalsub5( SRC ) );
		case 0xFFFFFFB1 :	// asinh
			return  asinh( Evalsub5( SRC ) );
		case 0xFFFFFFB2 :	// acosh
			return  acosh( Evalsub5( SRC ) );
		case 0xFFFFFFB3 :	// atanh
			return  atanh( Evalsub5( SRC ) );
			
		case 0xFFFFFF80 :	// Pol( x, y ) -> r=List Ans[1] , Theta=List Ans[2]
			tmp=EvalsubTop( SRC );
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return 0; }	// Syntax error
			ExecPtr++;
			tmp2=EvalsubTop( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			WriteListAns2( fpolr(tmp,tmp2), fpolt(tmp,tmp2) );
			return 0;
		case 0xFFFFFFA0 :	// Rec( r, Theta ) -> X,Y
			tmp=EvalsubTop( SRC );
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return 0; }	// Syntax error
			ExecPtr++;
			tmp2=EvalsubTop( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			WriteListAns2( frecx(tmp,tmp2), frecy(tmp,tmp2) );
			return 0;
			
		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFC6:	// M_PixelTest(
					return CB_ML_PixelTest( SRC );
				case 0xFFFFFFD8:	// M_Test
					return CB_MLTest( SRC );
//				case 0x53:	// M_Contrast(
//					return CB_ML_GetContrast( SRC );
				case 0x31:	// StrLen(
					return CB_StrLen( SRC );
				case 0x32:	// StrCmp(
					return CB_StrCmp( SRC );
				case 0x33:	// StrSrc(
					return CB_StrSrc( SRC );
				case 0x38:	// Exp(
					return CB_EvalStr(SRC, 0 );
				case 0x21:	// Xdot
					return Xdot;
				case 0x1B :		// fn str
					return CB_FnStr( SRC, 0 );
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFFDD :	// Eng
			return ENG ;
		default:
			break;
	}
	if ( c == '#') {
		result = EvalsubTop( SRC );
//		result = Evalsub1( SRC );
		return result;
	}
	ExecPtr--;
	reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}

double CB_frand( char *SRC ) {
	int c = SRC[ExecPtr];
	if ( ( '0'<=c )&&( c<='9' ) ) {
		jp:
		srand(CB_EvalInt(SRC));
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	} else
	if ( c=='#' ) { ExecPtr++; goto jp; }
	return frand() ;
}

double DmsToDec( char *SRC, double h ) {	// 12"34"56 -> 12.5822222
	double m,s,f=1;
	int c=SRC[ExecPtr];
	if ( h<0 ) { f=-1; h=-h; }
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		m=Evalsub1(SRC);
		if ( SRC[ExecPtr] == 0xFFFFFF8C ) ExecPtr++;
		if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
			s=Evalsub1(SRC);
		} else return (h + m/60)*f;
	} else return h*f;
	return (h + m/60 + s/3600)*f ;
}
//-----------------------------------------------------------------------------
//int EvalObjectAlignE4gg( unsigned int n ){ return n ; }	// align +4byte
int EvalObjectAlignE4hh( unsigned int n ){ return n+n; }	// align +6byte
//int EvalObjectAlignE4ii( unsigned int n ){ return n ; }	// align +4byte
//int EvalObjectAlignE4jj( unsigned int n ){ return n ; }	// align +4byte
//-----------------------------------------------------------------------------

double Evalsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	double result,tmp;
	int c;
	result = Evalsub1( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				result *= result ;
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				if ( result == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result = 1 / result ;
				break;
			case  0xFFFFFFAB  :	//  !
				result = ffact( result );
				break;
				
			case  0x01  :	//  femto
				result *= 1e-15 ;
				break;
			case  0x02  :	//  pico
				result *= 1e-12 ;
				break;
			case  0x03  :	//  nano
				result *= 1e-9 ;
				break;
			case  0x04  :	//  micro
				result *= 1e-6 ;
				break;
			case  0x05  :	//  milli
				result *= 1e-3 ;
				break;
			case  0x06  :	//  Kiro
				result *= 1e3 ;
				break;
			case  0x07  :	//  Mega
				result *= 1e6 ;
				break;
			case  0x08  :	//  Giga
				result *= 1e9 ;
				break;
			case  0x09  :	//  Tera
				result *= 1e12 ;
				break;
			case  0x0A  :	//  Peta
				result *= 1e15 ;
				break;
			case  0x1B  :	//  Exa
				result *= 1e18 ;
				break;
				
			case  0xFFFFFF8C  :	//  dms
				result = DmsToDec( SRC, result );
				break;
				
			case  0xFFFFFF9C  :	//  Deg
				result=finvdegree( result );
				break;
			case  0xFFFFFFAC  :	//  Rad
				result=finvradian( result );
				break;
			case  0xFFFFFFBC  :	//  Grad
				result=finvgrad( result );
				break;
				
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub3(char *SRC) {	//  3rd Priority  ( ^ ...)
	double result,tmp;
	int c;
	char *pt;
	result = Evalsub2( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = pow( result, Evalsub2( SRC ) );
				CheckMathERR(&result); // Math error ?
				break;
			case  0xFFFFFFB8  :	// powroot
				result = pow( Evalsub2( SRC ), 1/result );
				CheckMathERR(&result); // Math error ?
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub4(char *SRC) {	//  4th Priority  (Fraction) a/b/c
	double result,frac1,frac2,frac3;
	unsigned int c;
	result = Evalsub3( SRC );
	c = SRC[ExecPtr];
	if ( c == 0xFFFFFFBB ) {
		ExecPtr++;
		frac1 = result ;
		frac2 = Evalsub3( SRC );
		c = SRC[ExecPtr];
		if ( c == 0xFFFFFFBB ) {
			ExecPtr++;
			frac3 = Evalsub3( SRC );
			if ( frac3 == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = frac1 + ( frac2 / frac3 ) ;
		} else {
			if ( frac2 == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = ( frac1 / frac2 ) ;
		}
	}
	return result;
}
double Evalsub5(char *SRC) {	//  5th Priority abbreviated multiplication
	double result,tmp;
	int c,execptr;
	int dimA,dimB,reg,x,y;
	result = Evalsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A'<=c )&&( c<='Z' )) ||
			(( 'a'<=c )&&( c<='z' )) ||
			 ( c == 0xFFFFFFCD ) || // <r>
			 ( c == 0xFFFFFFCE ) || // Theta
			 ( c == 0xFFFFFFD0 ) || // PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result *= Evalsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x51:	// List 1[a]
				case 0x3A:	// MOD(a,b)
				case 0x3C:	// GCD(a,b)
				case 0x3D:	// LCM(a,b)
				case 0xFFFFFF8F:	// Getkey
				case 0xFFFFFF85:	// logab(a,b)
				case 0xFFFFFF86:	// RndFix(n,digit)
				case 0xFFFFFF87:	// RanInt#(st,en)
				case 0xFFFFFFB3 :	// Not
				case 0xFFFFFFF0:	// GraphY
				case 0x00:	// Xmin
				case 0x01:	// Xmax
				case 0x02:	// Xscl
				case 0x04:	// Ymin
				case 0x05:	// Ymax
				case 0x06:	// Yscl
				case 0x08:	// Thetamin
				case 0x09:	// Thetamax
				case 0x0A:	// Thetaptch
				case 0x0B:	// Xfct
				case 0x0C:	// Yfct
				case 0x20 :			// Max( List 1 )	Max( { 1,2,3,4,5 } )
				case 0x29 :			// Sigma( X, X, 1, 1000)
				case 0x2D :			// Min( List 1 )	Min( { 1,2,3,4,5 } )
				case 0x2E :			// Mean( List 1 )	Mean( { 1,2,3,4,5 } )
				case 0x47 :			// Fill(
				case 0x4A :			// List>Mat( List 1, List 2,..) -> List 5
				case 0x4B :			// Mat>List( Mat A, m) -> List n
				case 0x4C :			// Sum List 1
				case 0x4D :			// Prod List 1
				case 0x58 :			// ElemSize( Mat A )
				case 0x59 :			// RowSize( Mat A )
				case 0x5A :			// ColSize( Mat A )
				case 0x5B :			// MatBase( Mat A )
					result *= Evalsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					result *= Evalsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= Evalsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
//		} else if ( c == 0xFFFFFFE7 ) { // E7..
//			c = SRC[ExecPtr+1];
//			switch ( c ) {
//				default:
//					return result;
//					break;
//			}
		} else {
		  exitj:
			execptr=ExecPtr;
			c=RegVarAliasEx(SRC);
			if (c>0) { ExecPtr=execptr; result *= Evalsub4( SRC ) ; }
			else return result;
		}
	 }
	return result;
}
double Evalsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	double result,tmp;
	int c;
	result = Evalsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
			case 0xFFFFFF97 :	// abs
			case 0xFFFFFFA6 :	// int
			case 0xFFFFFFDE :	// intg
			case 0xFFFFFFB6 :	// frac
			case 0xFFFFFF85 :	// ln
			case 0xFFFFFF86 :	// sqr
			case 0xFFFFFF95 :	// log10
			case 0xFFFFFF96 :	// cuberoot
			case 0xFFFFFFA5 :	// expn
			case 0xFFFFFFA7 :	// Not
			case 0xFFFFFFB5 :	// 10^
			case 0xFFFFFFB7 :	// Neg
			case 0xFFFFFF81 :	// sin
			case 0xFFFFFF82 :	// cos
			case 0xFFFFFF83 :	// tan
			case 0xFFFFFF91 :	// asin
			case 0xFFFFFF92 :	// acos
			case 0xFFFFFF93 :	// atan
			case 0xFFFFFFA1 :	// sinh
			case 0xFFFFFFA2 :	// cosh
			case 0xFFFFFFA3 :	// tanh
			case 0xFFFFFFB1 :	// asinh
			case 0xFFFFFFB2 :	// acosh
			case 0xFFFFFFB3 :	// atanh
				result *= Evalsub5( SRC );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}

double Evalsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	double result,tmp;
	int c;
	result = Evalsub7( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ~
				result *= Evalsub7( SRC );
				break;
			case 0xFFFFFFB9 :		// €
				tmp = Evalsub7( SRC );
				if ( tmp == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result /= tmp ;
				break;
			case 0x7F:
				c = SRC[ExecPtr++];
				switch ( c ) {
					case 0xFFFFFFBC:	// Int€
						result = fIDIV( result, Evalsub7( SRC ) );
						break;
					case 0xFFFFFFBD:	// Rmdr
						result = fMOD( result, Evalsub7( SRC ) );
						break;
					default:
						ExecPtr-=2;
						return result;
						break;
				}
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub11(char *SRC) {	//  11th Priority  ( +,- )
	double result;
	int c;
	result = Evalsub10( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result += Evalsub10( SRC );
				break;
			case 0xFFFFFF99 :		// -
				result -= Evalsub10( SRC );
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}

double Evalsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	double result;
	int c;
	result = Evalsub11( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = ( result == Evalsub11( SRC ) );
				break;
			case '>' :	// >
				result = ( result >  Evalsub11( SRC ) );
				break;
			case '<' :	// <
				result = ( result <  Evalsub11( SRC ) );
				break;
			case 0x11 :	// !=
				result = ( result != Evalsub11( SRC ) );
				break;
			case 0x12 :	// >=
				result = ( result >= Evalsub11( SRC ) );
				break;
			case 0x10 :	// <=
				result = ( result <= Evalsub11( SRC ) );
				break;
			case 0xFFFFFF9A :	// xor
				result = ( (int)result ^ (int)Evalsub11( SRC ) );
				break;
			case '|' :	// or
			case 0xFFFFFFAA :	// or
				result = ( (int)result | (int)Evalsub11( SRC ) );
				break;
			case '&' :	// and
			case 0xFFFFFFBA :	// and
				result = ( (int)result & (int)Evalsub11( SRC ) );
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub13(char *SRC) {	//  13th Priority  ( And,and)
	double result;
	int c;
	result = Evalsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = ( ( (int)result != 0 ) & ( (int)Evalsub12( SRC ) != 0 ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}
double Evalsub14(char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	double result;
	int c;
	result = Evalsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = ( ( (int)result != 0 ) | ( (int)Evalsub13( SRC ) != 0 ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( ( (int)result != 0 ) ^ ( (int)Evalsub13( SRC ) != 0 ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

double Eval2(char *SRC, int *ptr) {		// Eval temp mat
	double result;
	int execptr=ExecPtr;
	int oplen=strlenOp((char*)SRC);
	ErrorPtr= 0;
	ErrorNo = 0;
	if ( oplen == 0 ) return 0;
	ExecPtr= *ptr;
	CB_StrBufferCNT=0;			// Quot String buffer clear
	result = EvalsubTop( SRC );
//	if ( ExecPtr < oplen ) CB_Error(SyntaxERR) ; // Syntax error 
	if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	*ptr=ExecPtr;
	ExecPtr=execptr;
	return result;
}
double Eval(char *SRC) {		// Eval temp
	int ptr=0;
	return Eval2( SRC, &ptr);
}

