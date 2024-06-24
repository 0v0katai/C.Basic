/* *****************************************************************************
 * CB_complex.h -- Header for complex arithmetic library
 * Copyright (C) 2015-2024 Sentaro21 <sentaro21@pm.matrix.jp>
 *
 * This file is part of C.Basic.
 * C.Basic is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * C.Basic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C.Basic; if not, see <https://www.gnu.org/licenses/>.
 * ************************************************************************** */
//----------------------------------------------------------------------------- complex
double fReP( complex z ) ;
double fImP( complex z ) ;
double farg( complex z ) ;
complex Cplx_fReP( complex z ) ;
complex Cplx_fImP( complex z ) ;
complex Cplx_farg( complex z ) ;
complex Cplx_fconjg( complex z ) ;

complex Int2Cplx2( int a, int b );	// a,b->a+bi;
complex Dbl2Cplx2( double a, double b );	// a,b->a+bi
complex Int2Cplx( int a );	// a->a+0i;
complex Dbl2Cplx( double a );	// a->a+0i

complex Cplx_flog( complex z ) ;
complex Cplx_fsinh( complex z ) ;
complex Cplx_fcosh( complex z ) ;
complex Cplx_ftanh( complex z ) ;
complex Cplx_fasinh( complex z ) ;
complex Cplx_facosh( complex z ) ;
complex Cplx_fatanh( complex z ) ;
complex Cplx_frac( complex z ) ;
complex Cplx_fabs( complex z ) ;
complex Cplx_intg( complex z ) ;	// intg
complex Cplx_fint( complex z ) ;	// int
complex Cplx_fdegree( complex z ) ;	//	Rad,Grad ->Deg
complex Cplx_finvdegree( complex z ) ;	//	Deg -> Rad,Grad
complex Cplx_fgrad( complex z ) ;		//	Deg,Rad -> Grad
complex Cplx_finvgrad( complex z ) ;		//	Grad -> Deg,Rad
complex Cplx_fradian( complex z ) ;	//	Deg,Grad ->Rad
complex Cplx_finvradian( complex z ) ;	//	Rad -> Deg,Grad
complex Cplx_fsin( complex z ) ;
complex Cplx_fcos( complex z ) ;
complex Cplx_ftan( complex z ) ;
complex Cplx_fasin( complex z ) ;
complex Cplx_facos( complex z ) ;
complex Cplx_fatan( complex z ) ;
complex Cplx_fpolr( complex x, complex y ) ;	// Pol(x,y) -> r
complex Cplx_fpolt( complex x, complex y ) ;	// Pol(x,y) -> Theta
complex Cplx_frecx( complex r, complex t ) ;	// Rec(r,Theta) -> x
complex Cplx_frecy( complex r, complex t ) ;	// Rec(r,Theta) -> y
complex Cplx_fAngle( complex r, complex t ) ;	// (r,Theta) -> a+bi
complex Cplx_fnot( complex z ) ;
complex Cplx_fsqrt( complex z ) ;
complex Cplx_flog10( complex z ) ;
complex Cplx_fpow10( complex z ) ;
complex Cplx_fln( complex z ) ;
complex Cplx_fexp( complex z ) ;
complex Cplx_fcuberoot( complex z ) ;
complex Cplx_fsqu( complex z ) ;
complex Cplx_ffact( complex z ) ;
complex Cplx_f_nPr( complex n, complex r ) ;
complex Cplx_f_nCr( complex n, complex r ) ;
complex Cplx_frecip( complex z ) ;	// ^(-1) RECIP
complex Cplx_ffemto( complex z ) ;	// femto
complex Cplx_fpico( complex z ) ;	// pico
complex Cplx_fnano( complex z ) ;	// nano
complex Cplx_fmicro( complex z ) ;	// micro
complex Cplx_fmilli( complex z ) ;	// milli
complex Cplx_fKiro( complex z ) ;	// Kiro
complex Cplx_fMega( complex z ) ;	// Mega
complex Cplx_fGiga( complex z ) ;	// Giga
complex Cplx_fTera( complex z ) ;	// Tera
complex Cplx_fPeta( complex z ) ;	// Peta
complex Cplx_fExa( complex z ) ;	// Exa

complex Cplx_fsign( complex z ) ;	// -x
complex Cplx_fADD( complex x, complex y ) ;	// x + y
complex Cplx_fSUB( complex x, complex y ) ;	// x - y
complex Cplx_fMUL( complex x, complex y ) ;	// x * y
complex Cplx_fDIV( complex x, complex y ) ;	// x / y
complex Cplx_fAND( complex x, complex y ) ;	// x & y
complex Cplx_fOR( complex x, complex y ) ;	// x | y
complex Cplx_fXOR( complex x, complex y ) ;	// x ^ y
complex Cplx_fNot( complex z ) ;			// ! x
complex Cplx_fAND_logic( complex x, complex y ) ;	// x && y
complex Cplx_fOR_logic( complex x, complex y ) ;	// x || y
complex Cplx_fXOR_logic( complex x, complex y ) ;	// (x!=0) ^ (y!=0)
complex Cplx_fNot_logic( complex x ) ;				// x == 0
complex Cplx_fcmpEQ( complex x, complex y ) ;	// x = y
complex Cplx_fcmpEQ_0( complex z ) ;	//  z == 0 
complex Cplx_fcmpGT( complex x, complex y ) ;	// x > y
complex Cplx_fcmpLT( complex x, complex y ) ;	// x < y
complex Cplx_fcmpNE( complex x, complex y ) ;	// x != y
complex Cplx_fcmpGE( complex x, complex y ) ;	// x >= y
complex Cplx_fcmpLE( complex x, complex y ) ;	// x <= y
complex Cplx_fpow( complex x, complex y ) ;	// pow(x,y)
complex Cplx_fpowroot( complex x, complex y ) ;	// powroot(x,y)
complex Cplx_fMOD( complex x, complex y ) ;	// fMOD(x,y)
complex Cplx_fIDIV( complex x, complex y ) ;	// (int)x / (int)y
complex Cplx_flogab( complex x, complex y ) ;	// flogab(x,y)
complex Cplx_frand() ;
complex Cplx_fGCD( complex x, complex y ) ;	// GCD(x,y)
complex Cplx_fLCM( complex x, complex y ) ;	// LCM(x,y)
complex Cplx_fRanNorm( complex sd, complex mean) ;	// RanNorm#
complex Cplx_fRanBin( complex n, complex p) ;	// RanBin#

complex Cplx_RoundFix( complex num, complex digit);
complex Cplx_RoundSci( complex num, complex digit);

double fabsz( complex z ) ;

complex Cplx_Eval(char *SRC);
complex Cplx_Eval2(char *SRC, int *ptr);
complex Cplx_Evalsub1(char *SRC);
complex Cplx_Evalsub2(char *SRC);
complex Cplx_Evalsub3(char *SRC);
complex Cplx_Evalsub4(char *SRC);
complex Cplx_Evalsub5(char *SRC);
complex Cplx_Evalsub6(char *SRC);
complex Cplx_Evalsub7(char *SRC);
complex Cplx_Evalsub8(char *SRC);
complex Cplx_Evalsub9(char *SRC);
complex Cplx_Evalsub10(char *SRC);
complex Cplx_Evalsub11(char *SRC);
complex Cplx_Evalsub12(char *SRC);
complex Cplx_Evalsub13(char *SRC);
complex Cplx_Evalsub14(char *SRC);
complex Cplx_EvalsubTop(char *SRC);
complex CB_Cplx_EvalDbl( char *SRC ) ;
int Cplx_Get2Eval( char *SRC, complex *tmp, complex *tmp2);

double CB_EvalDblReal( char *SRC ) ;	// eval real only
double EvalsubTopReal( char *SRC ) ;	// eval real only

int CB_EvalCheckZero( char *SRC ) ;
int CB_Cplx_EvalDblCheckZero( char *SRC ) ;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Cplx_WriteListAns2( complex x, complex y ) ;

complex Cplx_ListEvalsub1(char *SRC) ;
complex Cplx_ListEvalsub2(char *SRC) ;
complex Cplx_ListEvalsub3(char *SRC) ;
complex Cplx_ListEvalsub4(char *SRC) ;
complex Cplx_ListEvalsub5(char *SRC) ;
complex Cplx_ListEvalsub6(char *SRC) ;
complex Cplx_ListEvalsub7(char *SRC) ;
complex Cplx_ListEvalsub8(char *SRC) ;
complex Cplx_ListEvalsub9(char *SRC) ;
complex Cplx_ListEvalsub10(char *SRC) ;
complex Cplx_ListEvalsub11(char *SRC) ;
complex Cplx_ListEvalsub12(char *SRC) ;
complex Cplx_ListEvalsub13(char *SRC) ;
complex Cplx_ListEvalsub14(char *SRC) ;
complex Cplx_NoListEvalsubTop(char *SRC) ;	//  
complex Cplx_ListEvalsubTop(char *SRC) ;
int Cplx_ListEvalsub1Ans(char *SRC) ;	//  
int Cplx_ListEvalsubTopAns(char *SRC) ;	//  

complex CB_Sigma( char *SRC ) ; //	Sigma(X^2,X,1.10)

//-----------------------------------------------------------------------------
complex CB_Peek( char *SRC, int adrs ) ;	// Peek(123456).f
void CB_PokeSub( char *SRC, complex data, int adrs ) ;	// Poke(123456).f

//-----------------------------------------------------------------------------
complex Cplx_Round( complex num, int round_mode, int digit);

void sprintGi( char* buffer, double num, int width, int align_mode) ;
void Cplx_sprintG( char* buffer, complex num, int width, int align_mode) ;
void Cplx_sprintGR1( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit) ; // + round  ENG  + i
void Cplx_sprintGR2( char* buffer, char* buffer2, complex num, int width, int align_mode, int round_mode, int round_digit) ; // + round  ENG  + i
void Cplx_sprintGR2SRC( char* SRC, char* buffer, char* buffer2, complex num, int width ) ; // + round  ENG  + i
void Cplx_sprintGR1cut( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit ) ; // + round  ENG  + i
void Cplx_sprintGcut( char* buffer, complex num, int width, int align_mode) ;
void Cplx_sprintGR1cutlim( char* buffer, complex num, int width, int align_mode, int round_mode, int round_digit ) ; // + round  ENG  + i

complex InputNumC_fullsub(int x, int y, int width, complex defaultNum ) ;
complex InputNumC_fullhex(int x, int y, int width, complex defaultNum, int hex) ;
complex InputNumC_full(int x, int y, int width, complex defaultNum) ;		// full number display
complex InputNumC_Char(int x, int y, int width, complex defaultNum, int code) ;
complex InputNumC_replay(int x, int y, int width, complex defaultNum) ;

complex InputNumC_fullsub_mini(int x, int y, int width, complex defaultNum, int miniflag ) ;
complex InputNumC_fullhex_mini(int x, int y, int width, complex defaultNum, int hex, int miniflag) ;
complex InputNumC_full_mini(int x, int y, int width, complex defaultNum, int miniflag) ;		// full number display
complex InputNumC_Char_mini(int x, int y, int width, complex defaultNum, int code, int miniflag) ;
complex InputNumC_replay_mini(int x, int y, int width, complex defaultNum, int miniflag) ;

complex InputNumC_CB(int x, int y, int width, int MaxStrlen, char* SPC, int REV, complex defaultNum) ;		//  Basic Input
complex InputNumC_CB1(int x, int y, int width, int MaxStrlen, char* SPC, int REV, complex defaultNum) ;		//  Basic Input 1
complex InputNumC_CB2(int x, int y, int width, int MaxStrlen, char* SPC, int REV, complex defaultNum, int miniflag, int dispzero) ;		//  Basic Input 2
//-----------------------------------------------------------------------------

complex Cplx_ReadMatrix( int reg, int dimA, int dimB);				// 0-
void Cplx_WriteMatrix( int reg, int dimA, int dimB, complex value);	// 0-
void InitMatSub( int reg, complex value );

complex CB_Sum( char *SRC ) ;	// Sum( List 1 )
complex CB_Prod( char *SRC ) ;	// Prod( List 1 )
complex CB_MinMax( char *SRC, int flag) ;	// Min( List 1 )	flag  0:min  1:max
complex CB_Mean( char *SRC ) ;	// Mean( List 1 )
complex Cplx_CB_MatDet( char *SRC ) ;	// Det Mat A

	
complex Cplx_CB_DotP( char *SRC );		// DotP(Vct A,Vct B)
void  Cplx_CB_CrossP( char *SRC );	// CrossP(Vct A,Vct B)
complex Cplx_CB_AngleV( char *SRC );	// Angle(Vct A,Vct B)
void  Cplx_CB_UnitV( char *SRC );		// UnitV(Vct A)
complex Cplx_CB_NormV( char *SRC );		// Norm(Vct A)
	
