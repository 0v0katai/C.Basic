#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>

#include "fx_syscall.h"
#include "TIMER_FX2.H"

#include "CB_inp.h"
#include "CB_interpreter.h"
#include "CBI_interpreter.h"

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

double q_un_sub(void) {
     int i;
     double a[9];
     for (i=0;i<9;i++) a[i]=0;
     regN=1;  
 l5: regR=8;
     regS=0;
     regX=0;
 l0: if (regX==regR) goto l4;
     regX++;
     a[(int)regX]=regR;
 l1: ++regS;
     regY=regX;
 l2: if (!--regY) goto l0;
     if (!(regT=a[(int)regX]-a[(int)regY])) goto l3;
     if (regX-regY!=fabs(regT)) goto l2;
 l3: if (--a[(int)regX]) goto l1;
     if (--regX) goto l3;
 l4: if (--regN) goto l5;

  return (regS);
}

double asctt4(){
	regK=1.0;
	lbl0:
	regK=regK+1.0;
	regA=regK/2.0*regK+4.0-regK;
	if ( regK<10000 ) goto lbl0;
	return regA;;
}


void CB_test() {
	int i, s, t, result;
	char	buffer[32];
	unsigned int key=0;

	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"8queen ");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=q_un_sub();

	t=get_timer();	// --------------------------------------

	locate(1,2);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,3);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);
	Bdisp_PutDisp_DD();

	
	locate(1,5); Print((unsigned char*)"Asciit4 ");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=asctt4();

	t=get_timer();	// --------------------------------------

	locate(1,6);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,7);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);

	Bdisp_PutDisp_DD();
	GetKey(&key);
}


//----------------------------------------------------------------------------------------------

int q_un_subint(void) {
     int i;
     int a[9];
     for (i=0;i<9;i++) a[i]=0;
     regintN=1;  
 l5: regintR=8;
     regintS=0;
     regintX=0;
 l0: if (regintX==regintR) goto l4;
     regintX++;
     a[(int)regintX]=regintR;
 l1: ++regintS;
     regintY=regintX;
 l2: if (!--regintY) goto l0;
     if (!(regintT=a[(int)regintX]-a[(int)regintY])) goto l3;
     if (regintX-regintY!=abs(regintT)) goto l2;
 l3: if (--a[(int)regintX]) goto l1;
     if (--regintX) goto l3;
 l4: if (--regintN) goto l5;

  return (regintS);
}

int asctt4int(){
	regintK=1;
	lbl0:
	regintK=regintK+1;
	regintA=regintK/3*regintK+4-regintK;
	if ( regintK<10000 ) goto lbl0;
	return regintA;;
}


void CBint_test() {
	int i, s, t, result;
	char	buffer[32];
	unsigned int key=0;

	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"8queen int");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=q_un_subint();

	t=get_timer();	// --------------------------------------

	locate(1,2);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,3);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);
	Bdisp_PutDisp_DD();

	
	locate(1,5); Print((unsigned char*)"Asciit4 int");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=asctt4int();

	t=get_timer();	// --------------------------------------

	locate(1,6);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,7);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);

	Bdisp_PutDisp_DD();
	GetKey(&key);
}


//----------------------------------------------------------------------------------------------
