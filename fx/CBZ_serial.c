#include "CB.h"

char Transfermode=1;		// 0:binmode  1:normal

int send_data( unsigned char *buffer, int n ){
	int r;
	int i=0;
	if ( n==0 ) {
		if ( Serial_GetFreeTransmitSpace() > 128 ) 	Serial_ClearTransmitBuffer();	// clear buffer
		r=Serial_BufferedTransmitOneByte( buffer[0] );
//		r=Serial_DirectTransmitOneByte( buffer[0] );
		return 0;
	}
	while ( n>i ) {
	  loop:
		while ( Serial_GetFreeTransmitSpace() < 128 ) {
			if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return 0; }	// [AC] break?
		}
		r=Serial_BufferedTransmitOneByte( buffer[0] );
		if ( r==2 ) goto loop;	// no space is available in the serial interrupt transmit buffer (256 bytes max)
		if ( r ) break ;
		i++;
		buffer++;
	}
	return r;
}


int receive_data( unsigned char *buffer, int n ){
	int r;
	int i=0;
	short actbytes;
	if ( n==0 ) {
		r=Serial_ReadOneByte( buffer );
//		if ( Serial_GetReceivedBytesAvailable() > 0 ) Serial_ClearTransmitBuffer();	// clear buffer
		return 0;
	}
	while ( n>i ) {
	  loop:
		while ( Serial_GetReceivedBytesAvailable() < 1 ) {
			if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return 0; }	// [AC] break?
		}
		r=Serial_ReadOneByte( buffer );
		if ( r==1 ) goto loop;	// no byte is available
		if ( r ) break ;
		i++;
		buffer++;
	}
	return r;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Send(/ Recv(  format
//	"C"+"B"+ type + type2 + sizeL + sizeH + size3 + size4 + data........
//  type: 0:bin  1:byte 2:short  4:int  8:double  99:str
#define SERIAL_BIN    0
#define SERIAL_BIT    1
#define SERIAL_BYTE   8
#define SERIAL_WORD   16
#define SERIAL_LONG   32
#define SERIAL_DOUBLE 64
#define SERIAL_STRING 99
//------------------------------------------------------------------------------
int send_formatdata( unsigned char *buffer, int type, int n ){
	int r;
	unsigned char header[8];
	header[0]='C';
	header[1]='B';
	header[2]=type;
	header[3]=type;
	header[4]=(n&0x000000FF);
	header[5]=(n&0x0000FF00)>>8;
	header[6]=(n&0x00FF0000)>>16;
	header[7]=(n&0xFF000000)>>24;
	r=send_data( header, 8 );
	if ( r ) return r;
	r=send_data( buffer, n );
	return r;
}

int receive_formatdata( unsigned char *buffer, int *type, int *n ){
	int r,len=*n,type2=*type;
	unsigned char header[]={0,0,0,0,0,0,0,0};
	r=receive_data( header, 8 );
	if ( r ) return r;
	if ( ( header[0] == 'C' ) && ( header[1] == 'B' ) ) {
		*type=header[2];
		if ( *type!=type2 ) return -3;	// Type mismatch
		*n= header[4] + (header[5]<<8) + (header[6]<<16) + (header[7]<<24);
		if ( *n>len ) *n=len;
		if ( *n<=0 ) return -2;	// illegal data
		r=receive_data( buffer, *n );
	} else return -1;	// illegal data
	return r;
}


//------------------------------------------------------------------------------


int VarPtrLength( char *SRC, int *length, int *type, int flag) {
	int c,result,maxoplen;
	int reg,dimA, dimB,ElementSize,m,n;
	c=SRC[ExecPtr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {	// variable
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) { ExecPtr++; result=(int)&LocalInt[reg][0]; (*length)=4; *type=SERIAL_LONG; }
		else
		if ( c=='[' ) { goto Matrix; }		// A[1]
		else
		if ( ( '0'<=c )&&( c<='9' ) ) {		// A1
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
		} else
		if ( c=='#' ) { ExecPtr++; result=(int)&LocalDbl[reg][0]; (*length)=8; *type=SERIAL_DOUBLE; }
		else
		if (CB_INT==1)	{ result=(int)&LocalInt[reg][0]; (*length)=4; *type=SERIAL_LONG; } else { result=(int)&LocalDbl[reg][0]; (*length)=8; *type=SERIAL_DOUBLE; }
	} else
	if ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) {	// Mat
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) { reg=c-'A'; ExecPtr++; } 
		else { reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
		if ( SRC[ExecPtr] == '[' ) {
		Matrix:	
			ExecPtr++;
			MatOprand2( SRC, reg, &dimA, &dimB );	// Mat A[a,b]
		Matrix2:	
			if ( ErrorNo ) return 0 ; // error
			result=(int)MatrixPtr( reg, dimA, dimB );
		Matrix3:
			ElementSize=MatAry[reg].ElementSize; if ( ElementSize==2 ) ElementSize=1;
			m=MatAry[reg].SizeA;
			n=MatAry[reg].SizeB;
			if ( ElementSize>1 ) {
				(*length)=m*n*(ElementSize>>3);
			} else {	// 1 bit matrix
				(*length)=( ((m-1)>>3)+1 )*n;
			}
			*type=ElementSize;
		} else {
			result=(int)MatAry[reg].Adrs;	// Mat A
			goto Matrix3;
		}
	} else
	if ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) {	// List
		ExecPtr+=2;
		c=SRC[ExecPtr];
		reg=ListRegVar( SRC );
		if ( reg<0 ) CB_Error(SyntaxERR) ;  // Syntax error 
		if ( SRC[ExecPtr] == '[' ) {
			ExecPtr++;
			MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
			if ( ErrorNo ) return 0 ; // error
			result=(int)MatrixPtr( reg, dimA, dimB );
			goto Matrix3;
		} else {
			result=(int)MatAry[reg].Adrs;	// List 1
			goto Matrix3;
		}
	} else {
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			if ( ( flag ) && (c==0x22) ) CB_Error(SyntaxERR) ;  // Syntax error 
			result=(int)CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return 
			*type=SERIAL_STRING;
			if ( flag )	(*length)=maxoplen; 	// Recv
			else		(*length)=strlenOp( (char *)result )+1;	// Send
		} else {
			reg=RegVarAliasEx(SRC); if ( reg>=0 ) goto regj;
			if ( flag )	CB_Error(SyntaxERR) ;  // Syntax error 
			if (CB_INT==1) { 
				if ( c=='#' ) goto cdbl;
				else {
				if ( c=='%' ) ExecPtr++;
				  cint:
					CBint_CurrentValue=CB_EvalInt(SRC); *type=SERIAL_LONG;
					result=(int)&CBint_CurrentValue; (*length)=4;
				}
			} else	{
				if ( c=='%' ) goto cint;
				else {
				if ( c=='#' ) ExecPtr++;
				  cdbl:
					CB_CurrentValue=CB_Cplx_EvalDbl(SRC); *type=SERIAL_DOUBLE;
					result=(int)&CB_CurrentValue; (*length)=8; 
				}
			}
		}
	}
	return result;
}

int CB_baudrate( char *SRC, unsigned char *baud ){
	int b,c=SRC[ExecPtr];
	if ( c!=',' ) return 0;
	ExecPtr++;
	if ( SRC[ExecPtr]==',' ) return 0;
	b=CB_EvalInt(SRC);
	if ( ( b<0 ) || ( 9<b ) ) { CB_Error(ArgumentERR) ; } // Argument error 
	*baud=b;
	return 1;
}
int CB_length( char *SRC, int *length ){
	int len,c=SRC[ExecPtr];
	if ( c!=',' ) return 0;
	ExecPtr++;
	if ( SRC[ExecPtr]==',' ) return 0;
	len=CB_EvalInt(SRC);
	if ( ( len<0 ) ) { CB_Error(ArgumentERR) ; } // Argument error 
	*length=len;
	return 1;
}

void CB_Send( char *SRC ){				// Send( data )
	int ptr;
	int length,type;
	unsigned char mode[]={0,7,0,0,0,0};
	int r;

	ptr=VarPtrLength(SRC, &length, &type, 0 );
	if ( ErrorNo ) return;
//	CB_length( SRC, &length );
	CB_baudrate( SRC, &mode[1] );
	Serial_Close( 1 );	// close
	Serial_ClearTransmitBuffer();	// clear buffer
	Serial_Open( mode );	// open
	r=send_formatdata( (unsigned char*)ptr, type, length );
	while ( Serial_Close(0)==5 );	// transmissions are under way?
	Serial_Close( 1 );	// close
	if ( r ) CB_Error(TransmitERR);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
}
void CB_Receive( char *SRC ){			// Receive( data )
	int ptr;
	int length,type,type2;
	unsigned char mode[]={0,7,0,0,0,0};
	int r;
	short actbytes;

	ptr=VarPtrLength(SRC, &length, &type, 1 );
	if ( ErrorNo ) return;
//	CB_length( SRC, &length );
	CB_baudrate( SRC, &mode[1] );
	Serial_Close( 1 );	// close
	Serial_ClearTransmitBuffer();
	Serial_Open( mode );
	type2=type;
	r=receive_formatdata( (unsigned char*)ptr, &type2, &length );
	while ( Serial_Close(0)==5 );	// transmissions are under way?
	Serial_Close( 1 );	// close
	if ( type2 != type ) CB_Error(TypeMismatchERR);
	if ( r ) CB_Error(ReceiveERR);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
}

void CB_OpenComport38k( char *SRC ){	// OpenComport38k
	int c,r;
	int tmode=1;
	unsigned char mode[]={0,7,0,0,0,0};
	Serial_ClearTransmitBuffer();	// clear buffer
	CB_baudrate( SRC, &mode[1] );
	CB_length( SRC, &tmode );
	Transfermode=tmode;
	r=Serial_Open( mode );	// open
//	if ( r==3 ) CB_Error(AlreadyOpenERR);
}
void CB_CloseComport38k( char *SRC ){	// CloseComport38k
	int r;
	while ( Serial_Close(0)==5 );	// transmissions are under way?
	Serial_ClearTransmitBuffer();	// clear buffer
	r=Serial_Close( 1 );	// close
	if ( r ) CB_Error(ComERR);
}
void CB_Send38k( char *SRC ){			// Send38k
	int ptr;
	int length,type;
	int r;

	ptr=VarPtrLength(SRC, &length, &type, 0 );
	if ( ErrorNo ) return;
	CB_length( SRC, &length );
	if ( Transfermode == 0 ) {
		r=send_data( (unsigned char*)ptr, length );
	} else {
		r=send_formatdata( (unsigned char*)ptr, type, length );
	}
	if ( r==3 ) CB_Error(ComNotOpenERR);
	if ( r ) CB_Error(TransmitERR);
	
}
void CB_Receive38k( char *SRC ){		// Receive38k
	int ptr;
	int length,type,type2;
	int r;
	
	ptr=VarPtrLength(SRC, &length, &type, 1 );
	if ( ErrorNo ) return;
	CB_length( SRC, &length );
	if ( Transfermode == 0 ) {
		r=receive_data( (unsigned char*)ptr, length );
	} else {
		type2=type;
		r=receive_formatdata( (unsigned char*)ptr, &type2, &length );
		if ( type2 != type ) CB_Error(TypeMismatchERR);
	}
	if ( r==3 ) CB_Error(ComNotOpenERR);
	if ( r ) CB_Error(ReceiveERR);
}



//---------------------------------------------------------------------------------------------
// direct 3pin access		from INSIGHT.CPP

void CMT_Delay_micros( int n ) {
	int PortCR = P7305_SERIAL_DIRECT_PORTCR;
	volatile unsigned short SerialPortMode0;
	unsigned int SerialPortCR;
	int i,j;
	volatile int t=0;
	SerialPortCR = PortCR;
//	for ( i=0; i<n; i++ ) for ( j=0; j<1; j++ ) SerialPortMode0 = *(unsigned short*)SerialPortCR;
	for ( i=0; i<n; i++ ) for ( j=0; j<1; j++ ) t+=RTC_GetTicks();
}


void Direct3Pin_out( int micro, int time  ){
unsigned int SerialPortDR, SerialPortCR;
unsigned short SerialPortMask, SerialPortMode, SerialPortMode0, SerialPortMode1;
unsigned char SerialPortOut, SerialPortIn;
unsigned int iresult, iDelay, i;
unsigned char hb[15];
int PortCR = P7305_SERIAL_DIRECT_PORTCR;
	
// open the outer gate
	*(unsigned short*)P11CR = ( *(unsigned short*)P11CR & ~P11CR_ENABLE_SERIAL_MASK) | P11CR_ENABLE_SERIAL;
	*(unsigned char*)P11DR = *(unsigned char*)P11DR | P11DR_ENABLE_SERIAL;
	
// direct serial port
	SerialPortCR = PortCR;
	if ( PortCR == P7305_SERIAL_DIRECT_PORTCR ){
		SerialPortDR = P7305_SERIAL_DIRECT_PORTDR;
		
		// serial transmit pin
		// the following assignments have been determined experimetally and by deduction
		// 
		SerialPortOut  = 1 << P7305_SERIAL_TXD_BIT;
		SerialPortIn = 1 << P7305_SERIAL_RXD_BIT;
		SerialPortMask = ( 0x0003 << (P7305_SERIAL_TXD_BIT*2) ) | ( 0x0003 << (P7305_SERIAL_RXD_BIT*2) );
		SerialPortMode = ( 0x0001 << (P7305_SERIAL_TXD_BIT*2) ) | ( 0x0002 << (P7305_SERIAL_RXD_BIT*2) );
		iDelay = 10;
	}else if ( PortCR == P7305_ALT_SERIAL_DIRECT_PORTCR ){
		SerialPortDR = P7305_ALT_SERIAL_DIRECT_PORTDR;
		SerialPortOut = 1 << P7305_ALT_SERIAL_TXD_BIT;
		SerialPortIn  = 1 << P7305_ALT_SERIAL_RXD_BIT;
		SerialPortMask = ( 0x0003 << (P7305_ALT_SERIAL_TXD_BIT*2) ) | ( 0x0003 << (P7305_ALT_SERIAL_RXD_BIT*2) );
		SerialPortMode = ( 0x0001 << (P7305_ALT_SERIAL_TXD_BIT*2) ) | ( 0x0002 << (P7305_ALT_SERIAL_RXD_BIT*2) );
		
		SerialPortOut = 0xFF;
		SerialPortIn  = 0xFF;
		SerialPortMask = 0xFFFF;
		SerialPortMode = 0x5555;	// all to out
		SerialPortMode = 0xAAAA;	// all to in
		iDelay = 1000;
	}else{
		return;
	}
	
	// save the actual content (for debug purposes only)
	SerialPortMode0 = *(unsigned short*)SerialPortCR;
	*(unsigned short*)SerialPortCR = (SerialPortMode0 & (~SerialPortMask)) | SerialPortMode;

	// the following loop gives a sharp edged symmetric square wave of about 17 kHz.
//	iresult = 5000000 / iDelay;
//	iresult = 500;
	micro >>=1;
	while (time-- > 0 ){
		CMT_Delay_micros( micro );
//		for ( i=0; i<waitcount; i++ ) WaitTimer32768() ;
		*(unsigned char*)SerialPortDR = *(unsigned char*)SerialPortDR & (~SerialPortOut);
		CMT_Delay_micros( micro );
//		for ( i=0; i<waitcount; i++ ) WaitTimer32768() ;
		*(unsigned char*)SerialPortDR = *(unsigned char*)SerialPortDR | SerialPortOut;
	}
	SerialPortMode1 = *(unsigned short*)SerialPortCR; // for debug purposes only
	*(unsigned short*)SerialPortCR = SerialPortMode0;
	
// close the outer gate
	*(unsigned char*)P11DR = (*(unsigned char*)P11DR & ~P11DR_ENABLE_SERIAL) | 0x08;
	*(unsigned short*)P11CR = *(unsigned short*)P11CR | P11CR_ENABLE_SERIAL_MASK;
}
//------------------------------------------------------------------------------
void CB_Beep( char *SRC ){
	int r,a=1000, n=500;
	int microsecond, ntime;
	int c=SRC[ExecPtr];
	if ( ( c==':' )||( c==0x0D )||( c==0x0C )||( c==0 ) ) goto next;
	a=CB_EvalInt( SRC );
	CB_GetOprand_int1( SRC, &n);
	if ( ( a<=0 ) || ( n<=0 ) || ( n>10000 ) ) {  CB_Error(ArgumentERR); return ; } // Argument error
  next:
	microsecond=1000000/a/8;
	ntime=n*a/1000;
	Direct3Pin_out( microsecond, ntime );
}
