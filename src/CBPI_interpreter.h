//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CBint_Store( char *SRC );
void CBint_Dsz( char *SRC ) ; //	Dsz
void CBint_Isz( char *SRC ) ; //	Isz

//void CBint_PxlOprand( char *SRC, int *py, int *px) ;
//void CBint_PxlOn( char *SRC ) ; //	PxlOn
//void CBint_PxlOff( char *SRC ) ; //	PxlOff
//void CBint_PxlChg( char *SRC ) ; //	PxlChg
void CBint_PxlSub( char *SRC, int mode );  //	mode  1:PxlOn   0:PxlOff   2:PxlChg

int CBint_BinaryEval( char *SRC ) ;
int CBint_UnaryEval( char *SRC ) ;

int CBint_interpreter( char *SRC) ;
