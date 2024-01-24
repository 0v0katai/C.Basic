#include "CB.h"

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

complex Cplx_ReadMatrix( int reg, int dimA, int dimB){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	complex* MatAryCPLX;
	int		base=MatAry[reg].Base;
	int 	r;
	complex result={0,0};
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case 64:
			result.real = MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array double
			break;
		case 128:
			MatAryCPLX=(complex*)MatAry[reg].Adrs;			// Matrix array 1 bit
			result = MatAryCPLX[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array complex
			break;
		case  2:		// Vram
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;			// Matrix array 1 bit
			result.real = ( MatAryC[dimB*(((MatAry[reg].SizeA-1)>>3)+1)+(dimA>>3)] & ( 128>>(dimA&7) ) ) != 0 ;
			break;
		case  4:
			MatAryC=(char*)MatAry[reg].Adrs;			// Matrix array 4 bit
			r=MatAryC[dimB*(((MatAry[reg].SizeA-1)>>1)+1)+(dimA>>1)];
			if ( (dimA&1)==0 )  r = r>>4;
			r = r&0xF;
//			if ( r>=8 ) r -= 16;	// minus
			result.real = r ;
			break;
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			result.real = MatAryC[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array char
			break;
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			result.real = MatAryW[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array word
			break;
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			result.real = MatAryI[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array int
			break;
	}
	return result;
}
//-----------------------------------------------------------------------------
int EvalObjectAlignEC4a0( unsigned int n ){ return n; }	// align +4byte
int EvalObjectAlignEC4a1( unsigned int n ){ return n; }	// align +4byte
int EvalObjectAlignEC4a2( unsigned int n ){ return n; }	// align +4byte
int EvalObjectAlignEC4a3( unsigned int n ){ return n; }	// align +4byte
int EvalObjectAlignEC4b( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

void Cplx_WriteMatrix( int reg, int dimA, int dimB, complex value){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	complex*	MatAryCPLX;
	int tmp,vtmp,mask;
	int mptr;
	int		base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case 64:
			MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB]  = (double)value.real ;	// Matrix array double
			break;
		case 128:
			MatAryCPLX=(complex*)MatAry[reg].Adrs;			// Matrix array 1 bit
			MatAryCPLX[dimA*MatAry[reg].SizeB+dimB] = value;				// Matrix array complex
			break;
		case  2:		// Vram
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;					// Matrix array 1 bit
			tmp=( 128>>(dimA&7) );
			mptr=dimB*(((MatAry[reg].SizeA-1)>>3)+1)+((dimA)>>3);
			if ( value.real ) 	MatAryC[mptr] |= tmp ;
			else	 			MatAryC[mptr] &= ~tmp ;
			break;
		case  4:
			MatAryC=(char*)MatAry[reg].Adrs;					// Matrix array 4 bit
			mptr=dimB*(((MatAry[reg].SizeA-1)>>1)+1)+((dimA)>>1);
			vtmp=((int)value.real)&0x0F; mask=0xF0;
			if ( (dimA&1)==0 ) { vtmp<<=4; mask>>=4; }
			MatAryC[mptr] &= mask ;
			MatAryC[mptr] |= vtmp ;
			break;
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			MatAryC[dimA*MatAry[reg].SizeB+dimB] = (char)value.real ;	// Matrix array char
			break;
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			MatAryW[dimA*MatAry[reg].SizeB+dimB] = (short)value.real ;	// Matrix array word
			break;
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			MatAryI[dimA*MatAry[reg].SizeB+dimB] = (int)value.real ;		// Matrix array int
			break;
	}
}

//----------------------------------------------------------------------------------------------
complex CB_Cplx_EvalDbl( char *SRC ) {
	complex value={0,0};
	if (CB_INT==1) {
		if ( SRC[ExecPtr]=='#' ) { ExecPtr++;
			goto cplx;
		}
		else value.real=EvalIntsubTop( SRC ); 
	} else
	if (CB_INT==0)  value.real=EvalsubTop( SRC ); 
	else  {
	  cplx:
		value=Cplx_EvalsubTop( SRC );
//		if ( ( ComplexMode==0 ) && ( value.imag != 0 ) ) CB_Error(NonRealERR) ; // Non Real error 
	}
	return value;
}

int CB_Cplx_EvalDblCheckZero( char *SRC ) {
	complex z = CB_Cplx_EvalDbl( SRC );
	return ( ( z.real != 0 ) || ( z.imag != 0 ) );
}

complex Cplx_EvalsubTop( char *SRC ) {	// eval 1
	complex  result,dst;
	int c;
	int excptr=ExecPtr;
	int ansreg=CB_MatListAnsreg;

//	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	result= Cplx_Evalsub1(SRC);
	c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result;
	else 
	if ( c==0xFFFFFF89 ) { // +
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fADD(result,dst);
	} else
	if ( c==0xFFFFFF99 ) { // -
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fSUB(result,dst);
	} else
	if ( c=='=') { // ==
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fcmpEQ(result,dst);
	} else
	if ( c=='>') { // >
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fcmpGT(result,dst);
	} else
	if ( c=='<') { // <
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fcmpLT(result,dst);
	} else
	if ( c==0x11) { // !=
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fcmpNE(result,dst);
	} else
	if ( c==0x12) { // >=
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fcmpGE(result,dst);
	} else
	if ( c==0x10) { // <=
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fcmpLE(result,dst);
	} else
	if ( c==0xFFFFFFA9 ) { // *
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fMUL(result,dst);
	} else
	if ( c==0xFFFFFFB9 ) { // /
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fDIV(result,dst);
	} else
	if ( c==0xFFFFFF9A ) { // xor
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fXOR(result,dst);
	} else
	if ( ( c=='|' ) || ( c==0xFFFFFFAA ) ) { // or
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fOR(result,dst);
	} else
	if ( ( c=='&' ) || ( c==0xFFFFFFBA ) ) { // and
		ExecPtr++; dst=Cplx_Evalsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fAND(result,dst);
	} else
	if ( c==0xFFFFFF8B ) { // ^2
		ExecPtr++;
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fMUL(result,result);
	} else
	if ( c==0xFFFFFF9B ) { // ^(-1) RECIP
		ExecPtr++;
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_frecip(result);
	} else
	if ( c==0x7F ) { // 
		c=SRC[++ExecPtr];
		if ( c==0xFFFFFFB0 ) { // And
			ExecPtr++; dst=Cplx_Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fAND_logic(result,dst);
		} else
		if ( c==0xFFFFFFB1 ) { // Or
			ExecPtr++; dst=Cplx_Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fOR_logic(result,dst);
		} else
		if ( c==0xFFFFFFB4 ) { // Xor
			ExecPtr++; dst=Cplx_Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fXOR_logic(result,dst);
		} else
		if ( c==0xFFFFFFBC ) { // Int/
			ExecPtr++; dst=Cplx_Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fIDIV(result,dst);
		} else
		if ( c==0xFFFFFFBD ) { // Rmdr
			ExecPtr++; dst=Cplx_Evalsub1(SRC);
			c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return Cplx_fMOD(result,dst);
		}
	}
	
	ExecPtr=excptr;
	CB_MatListAnsreg=ansreg;
	return Cplx_Evalsub14( SRC );
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	complex
//-----------------------------------------------------------------------------
double fReP( complex z ) {
	return z.real;
}
double fImP( complex z ) {
	return z.imag;
}
double farg( complex z ) {
	double x = finvradian( atan2( z.imag, z.real ) );
	CheckMathERR(&x); // Math error ?
	return x;
}
complex Cplx_fReP( complex z ) {
	return Dbl2Cplx(z.real);
}
complex Cplx_fImP( complex z ) {
	return Dbl2Cplx(z.imag);
}
complex Cplx_farg( complex z ) {
	return Dbl2Cplx( farg(z) );
}
complex Cplx_fconjg( complex z ) {
	z.imag = -z.imag;
	return z;
}

complex Int2Cplx2( int a, int b )
{	// a,b->a+bi;
	complex result;
	result.real = a;
	result.imag = b;
	return result ;
}
complex Dbl2Cplx2( double a, double b ){	// a,b->a+bi
	complex result;
	result.real = a;
	result.imag = b;
	return result ;
}
complex Int2Cplx( int a )
{	// a->a+0i;
	complex result;
	result.real = a;
	result.imag = 0;
	return result ;
}
complex Dbl2Cplx( double a ){	// a->a+0i
	complex result;
	result.real = a;
	result.imag = 0;
	return result ;
}

complex Cplx_frac( complex z ) {
	return Dbl2Cplx2( frac( z.real), frac( z.imag) );
}
complex Cplx_fabs( complex z ) {
	if ( z.imag==0 ) return Dbl2Cplx( fabs(z.real) );
	if ( z.real==0 ) return Dbl2Cplx( fabs(z.imag) );
	return Dbl2Cplx( sqrt( z.real*z.real + z.imag*z.imag ) );
}
complex Cplx_intg( complex z ) {	// intg
	return Dbl2Cplx2( floor(z.real), floor( z.imag) );
}
complex Cplx_fint( complex z ) {	// int
	return Dbl2Cplx2( fint(z.real), fint( z.imag) );
}

complex Cplx_fdegree( complex z ) {	//	Rad,Grad ->Deg
	return Dbl2Cplx( fdegree( z.real) );
}
complex Cplx_finvdegree( complex z ) {	//	Deg -> Rad,Grad
	return Dbl2Cplx( finvdegree( z.real) );
}
complex Cplx_fgrad( complex z ) {		//	Deg,Rad -> Grad
	return Dbl2Cplx( fgrad( z.real) );
}
complex Cplx_finvgrad( complex z ) {		//	Grad -> Deg,Rad
	return Dbl2Cplx( finvgrad( z.real) );
}
complex Cplx_fradian( complex z ) {	//	Deg,Grad ->Rad
	return Dbl2Cplx( fradian( z.real) );
}
complex Cplx_finvradian( complex z ) {	//	Rad -> Deg,Grad
	return Dbl2Cplx( finvradian( z.real) );
}

complex Cplx_fsin( complex z ) {	// sin(z) = sin(z.real)*cosh(z.imag)+ cos(z.real)*sinh(z.imag)*i 
	if ( z.imag==0 ) return Dbl2Cplx( fsin(z.real) );
	return Dbl2Cplx2( ffsin(fradian(z.real))*cosh(z.imag), ffcos(fradian(z.real))*sinh(z.imag) ) ;
}
complex Cplx_fcos( complex z ) {	// cos(z) = cos(z.real)*cosh(z.imag) - sin(z.real)*sinh(z.imag)*i 
	if ( z.imag==0 ) return Dbl2Cplx( fcos(z.real) );
	return Dbl2Cplx2( ffcos(fradian(z.real))*cosh(z.imag), -ffsin(fradian(z.real))*sinh(z.imag) ) ;
}
complex Cplx_ftan( complex z ) {		// tan(z) = sin(2*z.real)/(cos(2*z.real)+sinh(2*z.imag)) + sinh(2*z.imag)/(cos(2*z.real)+sinh(2*z.imag))*i
	double tmp;
	if ( z.imag==0 ) return Dbl2Cplx( ftan(z.real) );
	tmp = ffcos(2*z.real)+cosh(2*z.imag);
	if ( tmp == 0 ) { CB_Error(MathERR) ; return Int2Cplx(0); } // Math error
	return Dbl2Cplx2( ffsin(2*fradian(z.real))/tmp,  sinh(2*z.imag)/tmp ) ;
}
complex Cplx_fasin( complex z ) {	// asin(z) = -i*log(i*z+sqrt(1-z*z))
	complex K1={1, 0};
	complex mi={0,-1};
	complex  i={0, 1};
	if ( z.imag==0 ) return Dbl2Cplx( fasin(z.real) );
	return Cplx_fMUL( mi, Cplx_fln( Cplx_fADD( Cplx_fMUL(i,z), Cplx_fsqrt(Cplx_fSUB(K1, Cplx_fMUL(z,z))) ) ) ) ;
}
complex Cplx_facos( complex z ) {		// acos(z) = i*log(i*z-sqrt(1-z*z))
	complex K1={1, 0};
	complex mi={0,-1};
	complex  i={0, 1};
	if ( z.imag==0 ) return Dbl2Cplx( facos(z.real) );
	return Cplx_fMUL( i, Cplx_fln( Cplx_fSUB( Cplx_fMUL(i,z), Cplx_fsqrt(Cplx_fSUB(K1, Cplx_fMUL(z,z))) ) ) ) ;
}
complex Cplx_fatan( complex z ) {		// atan(z) = -(i/2)*log((1+i*z)/(1-i*z))
	complex K1={1, 0};
	complex mi={0,-1};
	complex mhi={0,-.5};
	complex  i={0, 1};
	complex iz=Cplx_fMUL(i,z);
	if ( z.imag==0 ) return Dbl2Cplx( fatan(z.real) );
	return Cplx_fMUL( mhi, Cplx_fln( Cplx_fDIV( Cplx_fADD(K1,iz), Cplx_fSUB(K1,iz) ) ) );
}

complex Cplx_fpolr( complex x, complex y ) {	// Pol(x,y) -> r
	return Dbl2Cplx( fpolr(x.real, y.real) ) ;
}
complex Cplx_fpolt( complex x, complex y ) {	// Pol(x,y) -> Theta
	return Dbl2Cplx( fpolt(x.real, y.real) ) ;
}
complex Cplx_frecx( complex r, complex t ) {	// Rec(r,Theta) -> x
	return Dbl2Cplx( frecx(r.real, t.real) ) ;
}
complex Cplx_frecy( complex r, complex t ) {	// Rec(r,Theta) -> y
	return Dbl2Cplx( frecy(r.real, t.real) ) ;
}
complex Cplx_fAngle( complex r, complex t ) {	// (r,Theta) -> a+bi
	if ( (r.imag!=0)||(t.imag!=0) ) { CB_Error(NonRealERR); return Int2Cplx(0); }	// Input value must be a real number
	return Dbl2Cplx2( frecx(r.real, t.real), frecy(r.real, t.real) ) ;
}

complex Cplx_fsinh( complex z ) 
{	// sinh(x+yi) = sinh(x)*cos(y)+ cosh(x)*sin(y)*i;
	if ( z.imag==0 ) return Dbl2Cplx( sinh(z.real) );
	return  Dbl2Cplx2( sinh(z.real)*ffcos(z.imag), cosh(z.real)*ffsin(z.imag) );
}
complex Cplx_fcosh( complex z ) 
{	// cosh(x+yi) = cosh(x)*cos(y)+ sinh(x)*sin(y)*i;
	if ( z.imag==0 ) return Dbl2Cplx( cosh(z.real) );
	return  Dbl2Cplx2( cosh(z.real)*ffcos(z.imag), sinh(z.real)*ffsin(z.imag) );
}
complex Cplx_ftanh( complex z ) 
{	// tanh(x+yi) = sinh(2x)/(cosh(2x)+cos(2y)) + sin(2y)//(cosh(2x)+cos(2y))*i;
	double tmp;
	if ( z.imag==0 ) return Dbl2Cplx( tanh(z.real) );
	tmp = cosh(2*z.real)+ffcos(2*z.imag);
	if ( tmp == 0 ) { CB_Error(MathERR) ; return Int2Cplx(0); } // Math error
	return  Dbl2Cplx2( sinh(2*z.real)/tmp, ffsin(z.imag)/tmp );
}

complex Cplx_fasinh( complex z ) {		// asinh(z) = log(z+sqrt(z*z+1))
	complex c1={1,0};
	if ( z.imag==0 ) return Dbl2Cplx( asinh(z.real) );
	return Cplx_fln( Cplx_fADD( z, Cplx_fsqrt( Cplx_fADD( Cplx_fMUL( z, z ), c1 ) ) ) );
}
complex Cplx_facosh( complex z ) {			// acosh(z) = log(z+sqrt(z*z-1))
	complex c1={1,0};
	if ( z.imag==0 ) return Dbl2Cplx( acosh(z.real) );
	return Cplx_fln( Cplx_fADD( z, Cplx_fsqrt( Cplx_fSUB( Cplx_fMUL( z, z ), c1 ) ) ) );
}
complex Cplx_fatanh( complex z ) {			// atanh(z) = .5*log((1+z)/(1-z))
	complex c1={1,0};
	complex c2={.5,0};
	complex tmp1,tmp2;
	if ( z.imag==0 ) return Dbl2Cplx( atanh(z.real) );
	tmp1 = Cplx_fADD( c1, z );
	tmp2 = Cplx_fSUB( c1, z );
	return Cplx_fMUL( c2, Cplx_fln( Cplx_fDIV( tmp1, tmp2 ) ) );
}

double fabsz( complex z ) {
	return sqrt(z.real*z.real+z.imag*z.imag);
}
complex Cplx_fsqu( complex z ) {
	return Cplx_fMUL( z, z );
}
complex Cplx_fsqrt( complex z ) {
	double c;
	if ( (z.real>=0)&&(z.imag==0) ) return Dbl2Cplx( sqrt(z.real) );
	else {
		z.real *=2;
		z.imag *=2;
		c = fabsz(z);
		if ( z.imag >=  0 ) return Dbl2Cplx2( sqrt(z.real+c)/2,  sqrt(-z.real+c)/2 );
		else				return Dbl2Cplx2( sqrt(z.real+c)/2, -sqrt(-z.real+c)/2 );
	}
}
complex Cplx_fcuberoot( complex z ) {
	complex K3={1/3.0,0};
	if ( (z.real>0)&&(z.imag==0) ) return Dbl2Cplx( fcuberoot(z.real) );
	return Cplx_fpow( z, K3 );
}
complex Cplx_flog10( complex z ) {
	complex Klog10e={0.43429448190325183,0};
	if ( (z.real>0)&&(z.imag==0) ) return Dbl2Cplx( flog10(z.real) );
	return Cplx_fMUL( Klog10e, Cplx_fln(z) );
}
complex Cplx_fpow10( complex z ) {
	complex K10={10,0};
	if ( z.imag==0 ) return Dbl2Cplx( fpow10(z.real) );
	return Cplx_fpow( K10, z );
}
complex Cplx_fln( complex z ) {
	if ( (z.real>0)&&(z.imag==0) ) return Dbl2Cplx( fln(z.real) );
	return Dbl2Cplx2( log(fabsz(z)), atan2( z.imag, z.real ) );
}
complex Cplx_fexp( complex z ) {	// e^z = e^(z.real)*(cos(z.imag) + e^(z.real)*sin(z.imag)*i
	double x = exp( z.real );
	CheckMathERR(&x); // Math error ?
	if ( z.imag==0 ) return Dbl2Cplx(x);
	return Dbl2Cplx2( x*ffcos(z.imag), x*ffsin(z.imag) );
}
complex Cplx_flogab( complex x, complex y ) {	// flogab(x,y)
	if ( (x.real>0)&&(x.imag==0) && (y.real>0)&&(y.imag==0) ) return Dbl2Cplx( flogab(x.real, y.real) );
	return Cplx_fDIV( Cplx_fln(y), Cplx_fln(x) ) ;
}
complex Cplx_fpow( complex x, complex y ) {	// pow(x,y)
	if ( (x.real>=0)&&(x.imag==0)&&(y.imag==0) ) return Dbl2Cplx( fpow(x.real, y.real) );
//	if ( (x.real==0)&&(x.imag==0) && (y.real>0)&&(y.imag==0) )return Int2Cplx(0);
	return Cplx_fexp( Cplx_fMUL( Cplx_fln(x), y) );
}
complex Cplx_fpowroot( complex x, complex y ) {	// powroot(x,y)
	if ( (x.real>=0)&&(x.imag==0)&&(y.imag==0) ) return Dbl2Cplx( fpowroot(x.real, y.real) );
//	if ( (x.real==0)&&(x.imag==0) ) return Int2Cplx(0);
	return Cplx_fexp( Cplx_fMUL( Cplx_fln(x), Cplx_frecip(y) ) );
}

complex Cplx_frecip( complex z ) {	// ^(-1) RECIP
	double c;
	if ( z.imag==0 ) {
		if ( z.real==0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
		z.real = 1 / z.real ;
	} else {
		c = z.real*z.real + z.imag*z.imag;
		if ( c == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
		z.real =  z.real / c;
		z.imag = -z.imag / c;
	}
	return z;
}
complex Cplx_fsign( complex z ) {	// -z
	z.real = -z.real;
	z.imag = -z.imag;
	return z;
}
complex Cplx_fADD( complex x, complex y ) {	// x + y
	complex z;
	z.real = x.real + y.real;
	z.imag = x.imag + y.imag;
	return z;
}
complex Cplx_fSUB( complex x, complex y ) {	// x - y
	complex z;
	z.real = x.real - y.real;
	z.imag = x.imag - y.imag;
	return z;
}
complex Cplx_fMUL( complex x, complex y ) {	// x * y
	complex z;
	if ( (x.imag==0)&&(y.imag==0) ) {
		z.real = x.real * y.real;
		z.imag = 0;
	} else {
		z.real = x.real * y.real - x.imag * y.imag;
		z.imag = x.imag * y.real + x.real * y.imag;
	}
	return z;
}
complex Cplx_fDIV( complex x, complex y ) {	// x / y
	double tmp;
	complex z;
	if ( (x.imag==0)&&(y.imag==0) ) {
		if ( y.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
		z.real = x.real / y.real;
		z.imag = 0;
	} else {
		tmp = y.real*y.real + y.imag*y.imag;
		if ( tmp == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
		z.real = (x.real * y.real + x.imag * y.imag) /tmp;
		z.imag = (x.imag * y.real - x.real * y.imag) /tmp;
	}
	return z;
}
complex Cplx_fIDIV( complex x, complex y ) {	// floor( (int)x / (int)y )
	if ( (x.imag==0)&&(y.imag==0) ) return Dbl2Cplx( fIDIV(x.real, y.real) );
	return Cplx_intg( Cplx_fDIV( Cplx_intg(x), Cplx_intg(y) ) );
}
complex Cplx_fMOD( complex x, complex y ) {	// fMOD(x,y)
	if ( (x.imag!=0)||(y.imag!=0) ) { CB_Error(NonRealERR); return Int2Cplx(0); }	// Input value must be a real number
	return Dbl2Cplx( fMOD(x.real, y.real) );
}

complex Cplx_ffact( complex z ) {
	return  Dbl2Cplx( ffact( z.real ) );
}
complex Cplx_f_nPr( complex n, complex r ) {
	return  Dbl2Cplx( f_nPr( n.real, r.real ) );
}
complex Cplx_f_nCr( complex n, complex r ) {
	return  Dbl2Cplx( f_nCr( n.real, r.real ) );
}
complex Cplx_frand() {
	return Dbl2Cplx( frand() );
}
complex Cplx_fRanNorm( complex sd, complex mean) {	// RanNorm#
	return Dbl2Cplx( fRanNorm( sd.real, mean.real));
}
complex Cplx_fRanBin( complex n, complex p) {	// RanBin#
	return Dbl2Cplx( fRanBin( n.real, p.real));
}
complex Cplx_fGCD( complex x, complex y ) {	// GCD(x,y)
	if ( (x.imag!=0)||(y.imag!=0) ) { CB_Error(NonRealERR); return Int2Cplx(0); }	// Input value must be a real number
	return Dbl2Cplx( fGCD(x.real, y.real) );
}
complex Cplx_fLCM( complex x, complex y ) {	// LCM(x,y)
	if ( (x.imag!=0)||(y.imag!=0) ) { CB_Error(NonRealERR); return Int2Cplx(0); }	// Input value must be a real number
	return Dbl2Cplx( fLCM( x.real, y.real));
}

complex Cplx_fnot( complex z ) {
	z.real = ! (int) ( z.real );
	z.imag = 0;
	return z;
}
complex Cplx_fAND( complex x, complex y ) {	// x and y
	return Int2Cplx( (int)x.real & (int)y.real );
}
complex Cplx_fOR( complex x, complex y ) {	// x or y
	return Int2Cplx( (int)x.real | (int)y.real );
}
complex Cplx_fXOR( complex x, complex y ) {	// x xor y
	return Int2Cplx( (int)x.real ^ (int)y.real );
}
complex Cplx_fNot( complex x ) {	// Not x
	return Int2Cplx( ~(int)x.real );
}
complex Cplx_fAND_logic( complex x, complex y ) {	// x && y
	return Int2Cplx( x.real && y.real );
}
complex Cplx_fOR_logic( complex x, complex y ) {	// x || y
	return Int2Cplx( x.real || y.real );
}
complex Cplx_fXOR_logic( complex x, complex y ) {	// x xor y
	return Int2Cplx( ( x.real!=0) ^ (y.real!=0) );
}
complex Cplx_fNot_logic( complex x ) {	// Not x
	return Int2Cplx( ( x.real==0 ) && ( x.imag==0 ) );
}
complex Cplx_fcmpEQ( complex x, complex y ) {	//  x == y
	return Int2Cplx( ( x.real == y.real ) && ( x.imag == y.imag ) );
}
complex Cplx_fcmpEQ_0( complex z ) {	//  z == 0 
	return Int2Cplx( ( z.real == 0 ) && ( z.imag == 0 ) );
}
complex Cplx_fcmpGT( complex x, complex y ) {	//  x > y
	return Int2Cplx( x.real > y.real );
}
complex Cplx_fcmpLT( complex x, complex y ) {	//  x < y
	return Int2Cplx( x.real < y.real );
}
complex Cplx_fcmpNE( complex x, complex y ) {	//  x != y
	return Int2Cplx( ( x.real != y.real ) || ( x.imag != y.imag ) );
}
complex Cplx_fcmpGE( complex x, complex y ) {	//  x >= y
	return Int2Cplx( x.real >= y.real );
}
complex Cplx_fcmpLE( complex x, complex y ) {	//  x <= y
	return Int2Cplx( x.real <= y.real );
}

complex Cplx_ffemto( complex z ) {	// femto
	complex k={1e-15,0};
	return Cplx_fMUL( z, k );
}

complex Cplx_fpico( complex z ) {	// pico
	complex k={1e-12,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fnano( complex z ) {	// nano
	complex k={1e-9,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fmicro( complex z ) {	// micro
	complex k={1e-6,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fmilli( complex z ) {	// milli
	complex k={1e-3,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fKiro( complex z ) {	// Kiro
	complex k={1e3,0};
	return Dbl2Cplx( z.real * 1e3 );
}
complex Cplx_fMega( complex z ) {	// Mega
	complex k={1e6,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fGiga( complex z ) {	// Giga
	complex k={1e9,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fTera( complex z ) {	// Tera
	complex k={1e12,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fPeta( complex z ) {	// Peta
	complex k={1e15,0};
	return Cplx_fMUL( z, k );
}
complex Cplx_fExa( complex z ) {	// Exa
	complex k={1e18,0};
	return Cplx_fMUL( z, k );
}

complex Cplx_RoundFix( complex num, complex digit){
	return Dbl2Cplx2( Round( num.real, Fix, digit.real+.5 ),Round( num.imag, Fix, digit.real+.5 ) );
}
complex Cplx_RoundSci( complex num, complex digit){
	return Dbl2Cplx2( Round( num.real, Sci, digit.real+.5 ),Round( num.imag, Sci, digit.real+.5 ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int EvalObjectAlignE4cee( unsigned int n ){ return n ; }	// align +4byte
//int EvalObjectAlignE4cff( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

int Cplx_Get2Eval( char *SRC, complex *tmp, complex *tmp2){
	int c;
	(*tmp) = Cplx_EvalsubTop( SRC );
	if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
	ExecPtr++;
	(*tmp2) = Cplx_EvalsubTop( SRC );
	c=SRC[ExecPtr];
	if ( c == ')' ) ExecPtr++;
	return c;
}

complex Cplx_Evalsub1(char *SRC) {	// 1st Priority
	complex result={0,0},tmp,tmp2;
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
  topj:
	if ( c == '(') {
		result = Cplx_EvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		result = Cplx_fsign( Cplx_Evalsub5( SRC ) );
		return result;
	}
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) { ExecPtr++; return Int2Cplx( LocalInt[reg][0] ) ; }
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
		if (CB_INT==1) return Int2Cplx( LocalInt[reg][0] ); else return LocalDbl[reg][0] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		ExecPtr--;
		result.real = Eval_atof( SRC , c );
		return result;
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
						if ( ErrorNo ) return Int2Cplx( 1 ) ; // error
					} else { dspflag=3;	// Mat A
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return Cplx_ReadMatrix( reg, dimA, dimB);
						
				case 0xFFFFFF84 :	// Vct A[a,b]
					reg=VctRegVar( SRC ); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
					goto Matrix1;

				case 0x51 :		// List 1~26
				case 0x6A :		// List1
				case 0x6B :		// List2
				case 0x6C :		// List3
				case 0x6D :		// List4
				case 0x6E :		// List5
				case 0x6F :		// List6
					reg=ListRegVar( SRC );
				  Listj:
					if ( SRC[ExecPtr] == '[' ) {
						ExecPtr++;
						MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( ErrorNo ) return Int2Cplx( 1 ); // error
					} else { dspflag=4;	// List 1
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return Cplx_ReadMatrix( reg, dimA, dimB);

				case 0x50 :		// i
					result.imag = 1;
					return result;
						
				case 0x3A :				// MOD(a,b)
					Cplx_Get2Eval( SRC, &tmp, &tmp2);
					return Cplx_fMOD(tmp,tmp2);
					
				case 0x3C :				// GCD(a,b)
					Cplx_Get2Eval( SRC, &tmp, &tmp2);
					return Cplx_fGCD(tmp,tmp2);

				case 0x3D :				// LCM(a,b)
					Cplx_Get2Eval( SRC, &tmp, &tmp2);
					return Cplx_fLCM(tmp,tmp2);
					
				case 0xFFFFFF85 :		// logab(a,b)
					Cplx_Get2Eval( SRC, &tmp, &tmp2);
					return Cplx_flogab(tmp,tmp2);
					
				case 0xFFFFFFB3 :		// Not
					return Cplx_fNot_logic( Cplx_Evalsub5( SRC ) ) ;
						
				case 0xFFFFFF9F :		// KeyRow(
					return Int2Cplx( CB_KeyRow( SRC ) ) ; 
				case 0xFFFFFF8F :		// Getkey
					return Int2Cplx( CB_GetkeyEntry( SRC ) );
					
				case 0xFFFFFF87 :		// RanInt#(st,en[,n])
					if ( Cplx_Get2Eval( SRC, &tmp, &tmp2) == ',' ) {
						ExecPtr++;
						CB_RanInt( SRC, tmp.real, tmp2.real );
					}
					return Dbl2Cplx( frandIntint( tmp.real, tmp2.real ) );
					
				case 0xFFFFFF88 :		// RanList#(n) ->ListAns
					CB_RanList( SRC ) ;
					return Dbl2Cplx( 4 );
				case 0xFFFFFF89 :		// RanBin#(n,p[,m]) ->ListAns
					return Dbl2Cplx( CB_RanBin( SRC ) );
				case 0xFFFFFF8A :		// RanNorm#(sd,mean[,n]) ->ListAns
					return Dbl2Cplx( CB_RanNorm( SRC ) );
					
				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return Int2Cplx( 1 ); // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Int2Cplx( Cellsum( reg, x, y ) );
	
				case 0x5F :				// 1/128 Ticks
					return Int2Cplx( CB_Ticks( SRC ) );	// 
					
				case 0xFFFFFF86 :		// RndFix(n,digit)
					tmp = Cplx_EvalsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					if ( SRC[++ExecPtr] == 0xFFFFFFE4 ) { ExecPtr++; i=Sci; } else i=Fix;
					tmp2 = Cplx_EvalsubTop( SRC );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cplx_Round( tmp, i, tmp2.real);
						
				case 0xFFFFFFF0 :		// GraphY str
				case 0xFFFFFFF1:		// Graphr
				case 0xFFFFFFF2:		// GraphXt
				case 0xFFFFFFF3:		// GraphYt
				case 0xFFFFFFF4:		// GraphX
					return CB_Cplx_GraphYStr( SRC, 0 );
						
				case 0xFFFFFFF5 :		// IsExist(
					return Int2Cplx( CB_IsExist( SRC ,0 ) );
				case 0xFFFFFFF6 :		// Peek(
					return CB_Peek( SRC, Cplx_EvalsubTop( SRC ).real );
				case 0xFFFFFFF8 :		// VarPtr(
					return Int2Cplx( CB_VarPtr( SRC ) );
				case 0xFFFFFFFA :		// ProgPtr(
					return Int2Cplx( CB_ProgPtr( SRC ) );
				case 0x00 :				// Xmin
					return Dbl2Cplx( Xmin );
				case 0x01 :				// Xmax
					return Dbl2Cplx( Xmax );
				case 0x02 :				// Xscl
					return Dbl2Cplx( Xscl );
				case 0x04 :				// Ymin
					return Dbl2Cplx( Ymin );
				case 0x05 :				// Ymax
					return Dbl2Cplx( Ymax );
				case 0x06 :				// Yscl
					return Dbl2Cplx( Yscl );
				case 0x08 :				// Thetamin
					return Dbl2Cplx( TThetamin );
				case 0x09 :				// Thetamax
					return Dbl2Cplx( TThetamax );
				case 0x0A :				// Thetaptch
					return Dbl2Cplx( TThetaptch );
				case 0x0B :				// Xfct
					return Dbl2Cplx( Xfct );
				case 0x0C :				// Yfct
					return Dbl2Cplx( Yfct );
	
				case 0x22 :				// Arg
					return Cplx_farg( Cplx_Evalsub5( SRC ) );
				case 0x23 :				// Conjg
					return Cplx_fconjg( Cplx_Evalsub5( SRC ) );
				case 0x24 :				// ReP
					return Cplx_fReP( Cplx_Evalsub5( SRC ) );
				case 0x25 :				// ImP
					return Cplx_fImP( Cplx_Evalsub5( SRC ) );
				
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
					return Int2Cplx( 3 );
				case 0x48:	// Identity 
					CB_Identity(SRC);
					return Int2Cplx( 3 );
				case 0x49:	// Augment(
					CB_Augment(SRC);
					return Int2Cplx( 3 );
				case 0x2C:	// Seq
					CB_Seq(SRC);
					return Int2Cplx( 3 );
				case 0x41:	// Trn
					CB_MatTrn(SRC);
					return Int2Cplx( 3 );
				case 0x21:	// Det
					return Cplx_CB_MatDet(SRC);
					
				case 0x55 :				// Ref Mat A
					return Cplx_CB_MatRefRref( SRC, 0 );
				case 0x56 :				// Rref Mat A
					return Cplx_CB_MatRefRref( SRC, 1 );

				case 0x46 :				// Dim
					result.real = CB_Dim( SRC );
					result.imag = 0;
					if ( (result.real) >= 0 ) return result;
					ExecPtr--;	// error
					break;
				case 0x58 :				// ElemSize( Mat A )
					return Int2Cplx( CB_ElemSize( SRC ) );
				case 0x59 :				// RowSize( Mat A )
					return Int2Cplx( CB_RowSize( SRC ) );
				case 0x5A :				// ColSize( Mat A )
					return Int2Cplx( CB_ColSize( SRC ) );
				case 0x5B :				// MatBase( Mat A )
					return Int2Cplx( CB_MatBase( SRC ) );
					
				case 0x5D :				// GetRGB() ->ListAns
					return Int2Cplx(CB_GetRGB( SRC , 0 ) );
				case 0x5E :				// RGB(
					return Int2Cplx(CB_RGB( SRC , 0 ) );
				case 0x70 :				// GetHSV() ->ListAns
					return Int2Cplx(CB_GetRGB( SRC , 0x11 ) );
				case 0x71 :				// HSV(
					return Int2Cplx(CB_RGB( SRC , 1) );
				case 0x72 :				// GetHSL() ->ListAns
					return Int2Cplx(CB_GetRGB( SRC , 0x12 ) );
				case 0x73 :				// HSL(
					return Int2Cplx(CB_RGB( SRC , 2 ) );

				case 0x5C :				// ListCmp( List 1, List 2)
					return Int2Cplx( CB_ListCmp( SRC ) );
					
				case 0x4A :				// List>Mat( List 1, List 2,..) -> List 5
					CB_List2Mat( SRC );
					return Int2Cplx( 0 );
				case 0x4B :				// Mat>List( Mat A, m) -> List n
					CB_Mat2List( SRC );;
					return Int2Cplx( 0 );
					
				case 0x26 :				// dx/dy
					return Dbl2Cplx( CB_Differ( SRC ) );

				case 0xFFFFFFCF :				// System(
					return Int2Cplx( CB_System( SRC ) );
				case 0xFFFFFFDF :				// Version
					return Int2Cplx( CB_Version() );		//

				case 0xFFFFFF90 :				// F Result
					dspflag=4;		// 2:value		3:Mat    4:List
				case 0xFFFFFF91 :				// F Start
				case 0xFFFFFF92 :				// F End
				case 0xFFFFFF93 :				// F pitch
					return Dbl2Cplx( REGf[c-0xFFFFFF90] );

				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case '&' :	// & VarPtr
			return Int2Cplx( CB_VarPtr( SRC ) );
		case 0xFFFFFFD0 :	// const_PI
			return Dbl2Cplx( const_PI );
		case 0xFFFFFFC1 :	// Ran#
			return Dbl2Cplx( CB_frand( SRC ) );
		case 0xFFFFFF97 :	// abs
			return Cplx_fabs( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFA6 :	// int
			return Cplx_fint( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFDE :	// intg
			return Cplx_intg( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFB6 :	// frac
			return Cplx_frac( Cplx_Evalsub5( SRC ) );
		case  0xFFFFFFAB  :	//  ! Not
			return Cplx_fNot( Cplx_Evalsub5( SRC ) );

		case '%' :	// 1/128 Ticks
			return Int2Cplx( CB_Ticks( SRC ) );	// 
		case '*' :	// peek
			return CB_Peek( SRC, Cplx_Evalsub1( SRC ).real );	// 
//		case '@' :	// Mat @A
//			ExecPtr--;
//			goto Matjmp;
			
		case '{':	// { 1,2,3,4,5... }->List Ans
//			CB_List(SRC);
			dspflag=4;
			return Int2Cplx( 0 );
		case '[':	// [ [0,1,2][2,3,4] ]->Mat Ans
//			CB_Matrix(SRC);
			dspflag=3;
			return Int2Cplx( 0 );

		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					y= Cplx_EvalsubTop( SRC ).real;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x= Cplx_EvalsubTop( SRC ).real;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Int2Cplx( PxlTest(y, x) );			// 
				case 0xFFFFFFB0 :				// SortA( List 1)
					return Int2Cplx( CB_EvalSortAD( SRC, 1 ) );
				case 0xFFFFFFB1 :				// SortD( List 1)
					return Int2Cplx( CB_EvalSortAD( SRC, 0 ) );
				case 0xFFFFFFF4:	// SysCall(
					return  Int2Cplx( CB_SysCall( SRC ) );
				case 0xFFFFFFF5:	// Call(
					return  Int2Cplx( CB_Call( SRC ) );
				case 0xFFFFFFF8:	// RefreshCtrl
					return  Int2Cplx( RefreshCtrl );
				case 0xFFFFFFFA:	// RefreshTime
					return  Int2Cplx( Refreshtime+1 );
				case 0xFFFFFFFB:	// Screen
					return  Int2Cplx( ScreenMode );
				case 0xFFFFFFFE:	// BackLight
					return	Int2Cplx( BackLight(-1) );
				case 0xFFFFFFF2:	// PopUpWin(
					return Int2Cplx( CB_PopUpWin(SRC) );
				case 0xFFFFFFDE:	// BatteryStatus
					return Int2Cplx( CB_BatteryStatus(SRC) );
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFF86 :	// sqr
			return  Cplx_fsqrt( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF95 :	// log10
			return  Cplx_flog10( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFB5 :	// 10^
			return  Cplx_fpow10( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF85 :	// ln
			return  Cplx_fln( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFA5 :	// expn
			return  Cplx_fexp( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF96 :	// cuberoot
			return  Cplx_fcuberoot( Cplx_Evalsub5( SRC ) );

		case 0xFFFFFF81 :	// sin
			return  Cplx_fsin( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF82 :	// cos
			return  Cplx_fcos( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF83 :	// tan
			return  Cplx_ftan( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF91 :	// asin
			return  Cplx_fasin( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF92 :	// acos
			return  Cplx_facos( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFF93 :	// atan
			return  Cplx_fatan( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFA1 :	// sinh
			return  Cplx_fsinh( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFA2 :	// cosh
			return  Cplx_fcosh( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFA3 :	// tanh
			return  Cplx_ftanh( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFB1 :	// asinh
			return  Cplx_fasinh( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFB2 :	// acosh
			return  Cplx_facosh( Cplx_Evalsub5( SRC ) );
		case 0xFFFFFFB3 :	// atanh
			return  Cplx_fatanh( Cplx_Evalsub5( SRC ) );
			
		case 0xFFFFFF8D :	// integral
			return  Dbl2Cplx( CB_Integral( SRC ) );
			
		case 0xFFFFFF80 :	// Pol( x, y ) -> r=List Ans[1] , Theta=List Ans[2]
			tmp=Cplx_EvalsubTop( SRC );
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return Int2Cplx( 0 ); }	// Syntax error
			ExecPtr++;
			tmp2=Cplx_EvalsubTop( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			Cplx_WriteListAns2( Cplx_fpolr(tmp,tmp2), Cplx_fpolt(tmp,tmp2) );
			return Int2Cplx( 0 );
		case 0xFFFFFFA0 :	// Rec( r, Theta ) -> X,Y
			tmp=Cplx_EvalsubTop( SRC );
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return Int2Cplx( 0 ); }	// Syntax error
			ExecPtr++;
			tmp2=Cplx_EvalsubTop( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			Cplx_WriteListAns2( Cplx_frecx(tmp,tmp2), Cplx_frecy(tmp,tmp2) );
			return Int2Cplx( 0 );
			
		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFC6:	// M_PixelTest(
					return Int2Cplx( CB_ML_PixelTest( SRC ) );
				case 0xFFFFFFD8:	// M_Test
					return Int2Cplx( CB_MLTest( SRC ) );
//				case 0x53:	// M_Contrast(
//					return Int2Cplx( CB_ML_GetContrast( SRC ) );
				case 0x31:	// StrLen(
					return Int2Cplx( CB_StrLen( SRC ) );
				case 0x32:	// StrCmp(
					return Int2Cplx( CB_StrCmp( SRC ) );
				case 0x33:	// StrSrc(
					return Int2Cplx( CB_StrSrc( SRC ) );
				case 0x38:	// Exp(
					return CB_Cplx_EvalStr(SRC, 0 );
				case 0x50:	// StrAsc(
					return Int2Cplx( CB_StrAsc( SRC ) );
//				case 0x5F:	// IsError(
//					return Int2Cplx( CB_IsError(SRC) );
				case 0x60:	// GetFont(
					return Int2Cplx( CB_GetFont(SRC) );
				case 0x62:	// GetFontMini(
					return Int2Cplx( CB_GetFontMini(SRC) );
				case 0x21:	// Xdot
					return Dbl2Cplx( Xdot );
				case 0x1B :		// fn str
					return CB_Cplx_FnStr( SRC, 0 );
					
				case 0x4B:	// DotP(
					return Cplx_CB_DotP( SRC );
				case 0x4A:	// CrossP(
					Cplx_CB_CrossP( SRC );
					return Int2Cplx( 0 );
				case 0x6D:	// Angle(
					return Cplx_CB_AngleV( SRC );
				case 0x5E:	// UnitV(
					Cplx_CB_UnitV( SRC );
					return Int2Cplx( 0 );
				case 0x5B:	// Norm(
					return Cplx_CB_NormV( SRC );
					
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFFDD :	// Eng
			return Int2Cplx( ENG );
		default:
			break;
	}
	if ( c == '#') {
		result = Cplx_EvalsubTop( SRC );
//		result = Cplx_Evalsub1( SRC );
		return result;
	} else
	if ( c==' ' ) { while ( c==' ' )c=SRC[ExecPtr++]; goto topj; }	// Skip Space
	
	ExecPtr--;
	reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
	CB_Error(SyntaxERR) ; // Syntax error 
	return Int2Cplx( 0 );
}

//-----------------------------------------------------------------------------
int EvalObjectAlignE4cgg( unsigned int n ){ return n ; }	// align +4byte
int EvalObjectAlignE4chh( unsigned int n ){ return n+n; }	// align +6byte
int EvalObjectAligncE4ii( unsigned int n ){ return n ; }	// align +4byte
//int EvalObjectAligncE4jj( unsigned int n ){ return n ; }	// align +4byte
//-----------------------------------------------------------------------------

complex Cplx_Evalsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	complex result,tmp;
	int c;
	result = Cplx_Evalsub1( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				result = Cplx_fMUL( result, result ) ;
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				if ( result.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result = Cplx_frecip( result );
				break;
			case  0xFFFFFFAB  :	//  !
				result = Cplx_ffact( result );
				break;
				
			case  0x01  :	//  femto
				result = Cplx_ffemto( result );
				break;
			case  0x02  :	//  pico
				result = Cplx_fpico( result );
				break;
			case  0x03  :	//  nano
				result = Cplx_fnano( result );
				break;
			case  0x04  :	//  micro
				result = Cplx_fmicro( result );
				break;
			case  0x05  :	//  milli
				result = Cplx_fmilli( result );
				break;
			case  0x06  :	//  Kiro
				result = Cplx_fKiro( result );
				break;
			case  0x07  :	//  Mega
				result = Cplx_fMega( result );
				break;
			case  0x08  :	//  Giga
				result = Cplx_fGiga( result );
				break;
			case  0x09  :	//  Tera
				result = Cplx_fTera( result );
				break;
			case  0x0A  :	//  Peta
				result = Cplx_fPeta( result );
				break;
			case  0x1B  :	//  Exa
				result = Cplx_fExa( result );
				break;
				
			case  0xFFFFFF8C  :	//  dms
				result.real = DmsToDec( SRC, result.real );
				result.imag = 0;
				break;
				
			case  0xFFFFFF9C  :	//  Deg
				result=Cplx_finvdegree( result );
				break;
			case  0xFFFFFFAC  :	//  Rad
				result=Cplx_finvradian( result );
				break;
			case  0xFFFFFFBC  :	//  Grad
				result=Cplx_finvgrad( result );
				break;
				
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_Evalsub3(char *SRC) {	//  3rd Priority  ( ^ ...)
	complex result,tmp;
	int c;
	char *pt;
	result = Cplx_Evalsub2( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = Cplx_fpow( result, Cplx_Evalsub2( SRC ) );
				CheckMathERR(&result.real); // Math error ?
				break;
			case  0xFFFFFFB8  :	// powroot
				result = Cplx_fpow( Cplx_Evalsub2( SRC ), Cplx_frecip( result ) );
				CheckMathERR(&result.real); // Math error ?
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_Evalsub4(char *SRC) {	//  4th Priority  (Fraction) a/b/c
	complex result,frac1,frac2,frac3;
	unsigned int c;
	result = Cplx_Evalsub3( SRC );
	c = SRC[ExecPtr];
	if ( c == 0xFFFFFFBB ) {
		ExecPtr++;
		frac1 = result ;
		frac2 = Cplx_Evalsub3( SRC );
		c = SRC[ExecPtr];
		if ( c == 0xFFFFFFBB ) {
			ExecPtr++;
			frac3 = Cplx_Evalsub3( SRC );
			if ( frac3.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = Cplx_fADD( frac1, Cplx_fDIV( frac2, frac3 ) );
		} else {
			if ( frac2.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = Cplx_fDIV( frac1, frac2 );
		}
	}
	return result;
}
complex Cplx_Evalsub5(char *SRC) {	//  5th Priority abbreviated multiplication
	complex result,tmp;
	int c,execptr;
	int dimA,dimB,reg,x,y;
	result = Cplx_Evalsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A'<=c )&&( c<='Z' )) ||
			(( 'a'<=c )&&( c<='z' )) ||
			 ( c == 0xFFFFFFCD ) || // <r>
			 ( c == 0xFFFFFFCE ) || // Theta
			 ( c == 0xFFFFFFD0 ) || // const_PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 ) || // Ran#
			 ( c == 0xFFFFFF8D )) { // integral
				result = Cplx_fMUL( result, Cplx_Evalsub4( SRC ) );
		} else if ( c == 0x7F ) { // 7F..
				c = SRC[ExecPtr+1];
				if ( ( 0xFFFFFFB0 <= c ) && ( c <= 0xFFFFFFBD ) && ( c != 0xFFFFFFB3 ) ) goto exitj;	// And Or xor
				result = Cplx_fMUL( result, Cplx_Evalsub4( SRC ) );
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					result = Cplx_fMUL( result, Cplx_Evalsub4( SRC ) );
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x1B:	// fn
				case 0x21:	// Xdot
				case 0x31:	// StrLen(
				case 0x32:	// StrCmp(
				case 0x33:	// StrSrc(
				case 0x38:	// Exp(
				case 0x4B:	// DotP(
					result = Cplx_fMUL( result, Cplx_Evalsub4( SRC ) );
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
			if (c>0) { ExecPtr=execptr; result = Cplx_fMUL( result, Cplx_Evalsub4( SRC ) ); }
			else return result;
		}
	 }
	return result;
}
complex Cplx_Evalsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	complex result,tmp;
	int c;
	result = Cplx_Evalsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
			case '{' :
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
				result = Cplx_fMUL( result, Cplx_Evalsub5( SRC ) );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_Evalsub8(char *SRC) {	//  8th Priority  ( nPr,nCr,/_ )
	complex result,tmp;
	int c;
	result = Cplx_Evalsub7( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF88 :		// nPr
				result = Cplx_f_nPr( result, Cplx_Evalsub7( SRC ) );
				break;
			case 0xFFFFFF98 :		// nCr
				result = Cplx_f_nCr( result, Cplx_Evalsub7( SRC ) );
				break;
			case 0x7F:
				c = SRC[ExecPtr++];
				switch ( c ) {
					case 0x54:	// /_ Angle
						result = Cplx_fAngle( result, Cplx_Evalsub7( SRC ) );
						break;
					default:
						ExecPtr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_Evalsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	complex result,tmp;
	int c;
	result = Cplx_Evalsub8( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ~
				result = Cplx_fMUL( result, Cplx_Evalsub8( SRC ) );
				break;
			case 0xFFFFFFB9 :		// €
				tmp = Cplx_Evalsub8( SRC );
				if ( tmp.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result = Cplx_fDIV( result, tmp );
				break;
			case 0x7F:
				c = SRC[ExecPtr++];
				switch ( c ) {
					case 0xFFFFFFBC:	// Int€
						result = Cplx_fIDIV( result, Cplx_Evalsub8( SRC ) );
						break;
					case 0xFFFFFFBD:	// Rmdr
						result = Cplx_fMOD( result, Cplx_Evalsub8( SRC ) );
						break;
					default:
						ExecPtr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_Evalsub11(char *SRC) {	//  11th Priority  ( +,- )
	complex result;
	int c;
	result = Cplx_Evalsub10( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result = Cplx_fADD( result, Cplx_Evalsub10( SRC ) );
				break;
			case 0xFFFFFF99 :		// -
				result = Cplx_fSUB( result, Cplx_Evalsub10( SRC ) );
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}

complex Cplx_Evalsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	complex result;
	int c;
	result = Cplx_Evalsub11( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = Cplx_fcmpEQ( result,  Cplx_Evalsub11( SRC ) );
				break;
			case '>' :	// >
				result = Cplx_fcmpGT( result,  Cplx_Evalsub11( SRC ) );
				break;
			case '<' :	// <
				result = Cplx_fcmpLT( result,  Cplx_Evalsub11( SRC ) );
				break;
			case 0x11 :	// !=
				result = Cplx_fcmpNE( result,  Cplx_Evalsub11( SRC ) );
				break;
			case 0x12 :	// >=
				result = Cplx_fcmpGE( result,  Cplx_Evalsub11( SRC ) );
				break;
			case 0x10 :	// <=
				result = Cplx_fcmpLE( result,  Cplx_Evalsub11( SRC ) );
				break;
			case 0xFFFFFF9A :	// xor
				result = Cplx_fXOR( result,  Cplx_Evalsub11( SRC ) );
				break;
			case '|' :	// or
			case 0xFFFFFFAA :	// or
				result = Cplx_fOR( result,  Cplx_Evalsub11( SRC ) );
				break;
			case '&' :	// and
			case 0xFFFFFFBA :	// and
				result = Cplx_fAND( result,  Cplx_Evalsub11( SRC ) );
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_Evalsub13(char *SRC) {	//  13th Priority  ( And,and)
	complex result;
	int c;
	result = Cplx_Evalsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = Cplx_fAND_logic( result,  Cplx_Evalsub12( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) ExecPtr++;	// Skip Space
		else return result;
	}
}
complex Cplx_Evalsub14(char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	complex result;
	int c;
	result = Cplx_Evalsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = Cplx_fOR_logic( result,  Cplx_Evalsub13( SRC ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = Cplx_fXOR_logic( result,  Cplx_Evalsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) ExecPtr++;	// Skip Space
		else return result;
	}
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

complex Cplx_Eval2(char *SRC, int *ptr) {		// Eval temp mat
	complex result;
	int execptr=ExecPtr;
	int oplen=strlenOp((char*)SRC);
	ErrorPtr= 0;
	ErrorNo = 0;
	if ( oplen == 0 ) return Int2Cplx(0);
	ExecPtr= *ptr;
	CB_StrBufferCNT=0;			// Quot String buffer clear
	result = Cplx_ListEvalsubTop( SRC );
	if ( ( EvalEndCheck( SRC[ExecPtr] ) == 0 ) && ( ExecPtr < oplen ) ) CB_Error(SyntaxERR) ; // Syntax error 
//	if ( ( ComplexMode==0 ) && ( result.imag != 0 ) ) CB_Error(NonRealERR) ; // Non Real error 
	if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	*ptr=ExecPtr;
	ExecPtr=execptr;
	return result;
}
complex Cplx_Eval(char *SRC) {		// Eval temp
	int ptr=0;
	return Cplx_Eval2( SRC, &ptr);
}

