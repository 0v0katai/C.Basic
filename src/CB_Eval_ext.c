//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//	Gauss-Kronrod numerical integration integral function
//	refer to
//	https://community.casiocalc.org/topic/7637-cbasic-international-release/page-14#entry62978
//-----------------------------------------------------------------------------
#include "CB.h"

const double Kronrod[65]={0,
		1.231760537267155e-1,
		6.158081806783294e-2,
		6.147118987142532e-2,
		6.154448300568508e-2,
		1.222424429903100e-1,
		6.112850971705305e-2,
		1.228646926107104e-1,
		6.053945537604586e-2,
		1.837189394210489e-1,
		1.194557635357848e-1,
		5.972034032417406e-2,
		2.438668837209884e-1,
		5.868968002239421e-2,
		3.030895389311078e-1,
		1.148582591457116e-1,
		5.743711636156783e-2,

		3.611723058093878e-1,
		5.595081122041232e-2,
		4.178853821930377e-1,
		1.085196244742637e-1,
		5.425112988854549e-2,
		4.730027314457150e-1,
		5.236288580640748e-2,
		5.263252843347192e-1,
		1.005359490670506e-1,
		5.027767908071567e-2,
		5.776629302412230e-1,
		4.798253713883671e-2,
		6.268100990103174e-1,
		9.102826198296365e-2,
		4.550291304992179e-2,
		6.735663684734684e-1,

		4.287284502017005e-2,
		7.177664068130844e-1,
		8.014070033500102e-2,
		4.008382550403238e-2,
		7.592592630373576e-1,
		3.711627148341554e-2,
		7.978737979985001e-1,
		6.803833381235692e-2,
		3.400213027432934e-2,
		8.334426287608340e-1,
		3.079230016738749e-2,
		8.658470652932756e-1,
		5.490469597583519e-2,
		2.747531758785174e-2,
		8.949919978782754e-1,
		2.400994560695322e-2,

		9.207471152817016e-1,
		4.093915670130631e-2,
		2.043537114588284e-2,
		9.429745712289743e-1,
		1.684781770912830e-2,
		9.616149864258425e-1,
		2.635498661503214e-2,
		1.323622919557168e-2,
		9.766639214595175e-1,
		9.473973386174152e-3,
		9.880357945340772e-1,
		1.139379850102629e-2,
		5.561932135356714e-3,
		9.95556969790498e-1,
		1.987383892330316e-3,
		9.992621049926098e-1};

double Integral_Kronrod( char *SRC, int execptr, double A, double B, double tol ){
	int Na[6]={1,10,100,300,1000};
	int n,i,j,f;
	double	N,D,E,G,H,J,K;
	double xreg=regX.real;
	
	for( j=0; j<=4; j++){							//	For 1->N% To 3
		n = Na[j];									//		N[N%]->N
		D = 0;										//		0->D
		E = 0;										//		0->E
		H = A;										//		A->H
		G = (B-A)/(2*n);							//		(B-A)/(2*N)->G
		do {										//		Do
			H = G+H;								//			G+H->H
			i = 64;									//			64->I
			f = 1;									//			1->F
			do {									//			Do
				J = G*Kronrod[i];					//				G*Mat A[I]->J
				regX.real = H-J;					//				H-J->X
				ExecPtr = execptr;					//				Prog Str 1
				K = CB_EvalDbl( SRC );				//				X->K
				regX.real = H+J;					//				H+J->X
				ExecPtr = execptr;					//				Prog Str 1
				J = CB_EvalDbl( SRC )+K;			//				X+K->J
				i--;								//				I-1->I
				E += Kronrod[i]*J;					//				Mat A[I]*J+E->E
				f = -f;								//				(-)F->F
				if ( f>=0 ) {						//				If F>=0 Then 
					i--;							//					I-1->I
					D += Kronrod[i]*J;				//					Mat A[I]*J+D->D
				}									//				IfEnd
				i--;								//				I-1->I
			} while(i>2);							//			LpWhile I>2
			regX.real = H;							//			H->X
			ExecPtr = execptr; 						//			Prog Str 1
			J = CB_EvalDbl( SRC );					//			X->J
			E += Kronrod[i]*J;						//			Mat A[I]*J+E->E
			D += Kronrod[1]*J;						//			J*Mat A[1]+D->D
			H += G;									//			G+H->H
			n--;									//			N-1->N
			if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; goto exit; }	// [AC] break?
		} while(n>0);								//		LpWhile N>0
		D *= G;										//		G*D->D
		E *= G;										//		G*E->E
		if ( tol>=1 ) {
			if ( Round(E,Sci,(int)tol)==Round(D,Sci,(int)tol) ) {	//	If RndFix(D,Sci C+1)=RndFix(E,Sci C+1)Then 
				break;									//			Break
			}											//		IfEnd
		} else {
			if ( fabs(D-E)<=tol ) {						//		If Abs(D-E)<tol Then 
				D = Round(E,Fix,-log10(tol));
				break;
			}
		}
	}												//	Next
	if ( j>4 ) { 									//	If N%>3 Then "**ACCURACY NOT MET**"
		CB_Error(NotAccuracyERR);
	}												//	IfEnd
  exit:
	regX.real=xreg;
	return E;											//	E
}


void IntgralOprand( char *SRC, double *start, double *end, double *tol ){	// Integral(sin X,1,10[,5])
	int exptr=ExecPtr;
	int errflag=0;
	double data;
  restart:
	data=CB_EvalDbl( SRC );	// dummy read
	if ( ErrorNo == 0 ) errflag=0;
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	if ( errflag ) if ( ErrorNo ) return ;	// fatal error
	errflag=ErrorNo;	// error?
	if ( (unsigned char)SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	ExecPtr++;
	*start=CB_EvalDbl( SRC );	// start
	if ( (unsigned char)SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	ExecPtr++;
	*end=CB_EvalDbl( SRC );	// end
	if ( (unsigned char)SRC[ExecPtr] == ',' ) {
		ExecPtr++;
		*tol=CB_EvalDbl( SRC );	// tol
	} else *tol=6;

	if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( errflag ) {
		ExecPtr=exptr;
		regX=Dbl2Cplx(((*start)+(*end))/2);
		ErrorPtr= 0;
		ErrorNo = 0;	// error cancel
		goto restart;
	}
}

double CB_Integral( char *SRC ){	// Integral( FX, start, end, tol )
	int minus=0;
	int exptr,exptr2;
	double start,end,result,tol;
	
	exptr  = ExecPtr;
	IntgralOprand( SRC, &start, &end, &tol );
	exptr2 = ExecPtr;
	if ( ErrorNo ) return 0;
	if ( start>end ) { result=start; start=end; end=result;  minus=1; }
	result  = Integral_Kronrod( SRC, exptr, start, end, tol );
	if ( minus ) result=-result;
	ExecPtr = exptr2;
	if ( ErrorNo ) ErrorPtr=ExecPtr;
	return result;
}

double CB_Differ( char *SRC ){	// Differ( FX, tol )
	int exptr=ExecPtr;
	int errflag=0;
	double data;
  restart:
	CB_EvalDbl( SRC );	// dummy read
	return 0;
}

