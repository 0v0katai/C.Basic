// syscalls are defined in syscall.hpp and syscall.cpp
// this file is used for derived functions

#ifndef __MEMVIEWCLASS_CPP__
#define __MEMVIEWCLASS_CPP__
#ifdef __cplusplus
extern "C" {
#endif

#include "keyboard.hpp"
#include "disp_tools.hpp"
#include "display_tools.h"
#include "STATUS_ICONS.hpp"
#include "MemViewClass.hpp"
#include "filebios.h"
#include "Bfile_syscalls.h"
#include "keyboard_syscalls.h"
#include "Serial_syscalls.h"

#define BLOCKSIZE 0x1000
#define DIVISOR 0x1400

#define MAXEDPOS 15
#define XBOX 3
#define YBOX 3

//
TMsgRes xSearchMemory( unsigned int&start_address, TViewMode vm, unsigned char*LastSearchString ){
unsigned char binwork[MEMORY_MAXSEARCHSTRINGSIZE+1], hb[20], swork[MEMORY_MAXSEARCHSTRINGSIZE+1];
int key, i, iContinue=1, idigit, edpos = 0, work_address = start_address;
int maxcomp;
TMsgRes result = mrNONE;
int xpos=XBOX, ypos=YBOX;

TedSTR eds;

unsigned char work[MEMORY_MAXSEARCHSTRINGSIZE+1];

	memcpy( work, LastSearchString, sizeof( work ) );
	if ( vm == mvmHEX ){
		eds.pretext = "enter hexstr:";
		eds.buffer = work;
		eds.maxlen = MEMORY_MAXSEARCHSTRINGSIZE;
		result = EditDialog( edHEXSTR, &eds );
	}else{
		eds.pretext = "enter ascstr:";
		eds.buffer = work;
		eds.maxlen = MEMORY_MAXSEARCHSTRINGSIZE;
		result = EditDialog( edSTR, &eds );
	}
	if ( result == mrOK ){
		memcpy( LastSearchString, work, sizeof( work ) );
	}else return result;
	
	MsgBoxPush( 5 );
	if ( vm == mvmHEX ){
		edpos = strlen( (char*)work );
		maxcomp = edpos / 2;
		for ( i = 0; i < maxcomp; i++){ 
			HexToByte( work+2*i, binwork+i );
		}
	}else{
		edpos = strlen( (char*)work );
		maxcomp = edpos;
		memcpy( binwork, work, maxcomp );
	}
//	MessageDlg( &maxcomp, mtDebugInt );

	work_address++;
	locate_OS( XBOX, YBOX );
	PrintLine( "searching...", 19 );
	locate_OS( XBOX, YBOX+1 );
	PrintLine( "", 19 );
	locate_OS( XBOX, YBOX+2 );
	PrintLine( "hold EXIT to quit.", 19 );
	Bdisp_PutDisp_DD();
	while( !memcmp( (void*)work_address, binwork, maxcomp ) ){
		work_address++;
		if (!work_address) break;
		if ( !(work_address & 0x000FFFFF) ){
			LongToAscHex( work_address, hb, 8 );
			locate_OS( XBOX, YBOX+1 );
			Print( hb );
			Bdisp_PutDisp_DD();
			
			if ( PRGM_GetKey() == KEY_PRGM_EXIT ){
				work_address = 0;
				break;
			}
			
		}
	}
	if ( work_address ){
//		start_address = work_address & ~0x03;
		start_address = work_address;
		LongToAscHex( work_address, hb, 8 );
		locate_OS( XBOX, YBOX+1 );
		Print( hb );
		result = mrOK;
	}
	MsgBoxPop();

	return result;
}


//
void xMemoryToSerial( const unsigned int start_address ){
int j, i, size=0, iresult, key;
unsigned int chksum;
unsigned char hb[20];
void*work_address=(void*)start_address;
unsigned char serial_mode[ 6 ];
unsigned char*p;
TedHEX edh;

	memset( hb, 0, 20 );
	memset( hb, '0', 8 );
	
	edh.pretext = "enter size:";
	edh.value = 0x2000;
	if ( EditDialog( edHEX, &edh ) == mrOK ){
		size = edh.value;
//		MessageDlg( &(edh.value), mtDebugInt );
	};
	if ( size <= 0 ) return;
	// not more than 32 MB
	if ( size > 0x2000000 ) return;
	
	MsgBoxPush( 4 );
	
	locate_OS( 3, 3 );
	PrintLine( "Opening...wait", 19 );
	locate_OS( 3, 4 );
	itoa( 2 + size / DIVISOR, hb );
	Print( hb );
	PrintLine( " sec", 19 );
	Bdisp_PutDisp_DD();

	// open serial	
	serial_mode[0] = 0;
	serial_mode[1] = 9;
	serial_mode[2] = 2;
	serial_mode[3] = 0;
	serial_mode[4] = 1;
	serial_mode[5] = 0;
	iresult = Serial_Open( serial_mode );

	if ( iresult ){
		locate_OS( 3, 3 );
		PrintLine( "Serial failed!", 19 );
		GetKey( &i );
	}else{
		locate_OS( 3, 3 );
		PrintLine( "Sending...", 19 );
		locate_OS( 3, 4 );
		LongToAscHex( size, hb, 8 );
		Print( hb );
		locate_OS( 3, 5 );
		LongToAscHex( (unsigned int)work_address, hb, 8 );
		Print( hb );
//		key = 0;
//		while ( key != KEY_CTRL_F1 ) GetKey( &key );
		p = (unsigned char*)&size;
		for (j=0;j<2;j++){
			for (i=0;i<4;i++){
				iresult = 1;
				while( iresult == 1 ) iresult = Serial_DirectTransmitOneByte( p[i] );
			}
			if ( iresult ) return;
		}
		
		if ( 1 ){
			chksum = 0;
			for ( i = 0;i < size; i++ )	{
				chksum = chksum + *(unsigned char*)work_address;

				iresult = 1;
				while( iresult == 1 ){
					iresult = Serial_DirectTransmitOneByte( *(unsigned char*)work_address );
				}
				if ( iresult == 3 ) break;
				work_address = (void*)((unsigned int)work_address + 1);
				if ( ( i % 0x1000 ) == 0 ){
					if ( PRGM_GetKey() ) break;
					LongToAscHex( size - i, hb, 8 );
					locate_OS( 3, 4 );
					PrintLine( hb, 19 );
					Bdisp_PutDisp_DD();
				}	
			}
			p = (unsigned char*)&chksum;
			for (j=0;j<2;j++){
				for (i=0;i<4;i++){
					iresult = 1;
					while( iresult == 1 ) iresult = Serial_DirectTransmitOneByte( p[i] );
				}
				if ( iresult ) return;
			}
		}	
		while ( Serial_Close( 0 ) ){
			locate_OS( 3, 5 );
			PrintLine( "wait for trans...", 19 );
			GetKey( &key );
		}	
	}
	MsgBoxPop();
	Cursor_SetFlashOn( 0 );

}



const unsigned char xFNBackup[] = "\\\\fls0\\MEMORY.BKP";
//
void xBackupMemory( const unsigned int start_address ){
int size=0, blocksize;
int i, handle;
unsigned short FName[64];
unsigned char hb[20];
void*work_address=(void*)start_address;
TedHEX edh;
	
	memset( hb, 0, 20 );
	memset( hb, '0', 8 );
	
	edh.pretext = "enter size:";
	edh.value = 0x2000;
	if ( EditDialog( edHEX, &edh ) == mrOK ){
		size = edh.value;
//		MessageDlg( &(edh.value), mtDebugInt );
	};
	if ( size == 0 ) return;
	
	size = ( size + 1 ) & ~1;	// 2-align
	if ( size < 0 ) return;
	// not more than 2 MB
	if ( size > 0x200000 ) return;
	
	MsgBoxPush( 3 );
	
	locate_OS( 3, 3 );
	PrintLine( "Creating...wait", 19 );
	locate_OS( 3, 4 );
	itoa( 2 + size / DIVISOR, hb );
	Print( hb );
	PrintLine( " sec", 19 );
	Bdisp_PutDisp_DD();
	
	Bfile_StrToName_ncpy( FName, xFNBackup, 64 );
	Bfile_DeleteEntry( FName );
	if (Bfile_CreateEntry_OS( FName, 1, &size )){
		locate_OS( 3, 3 );
		PrintLine( "Create failed!", 19 );
		GetKey( &i );
	}else{
		locate_OS( 3, 3 );
		PrintLine( "Writing...", 19 );
		handle = Bfile_OpenFile_OS( FName, 2 );
		if (handle >= 0){
			while (size){
				itoa( size, hb );
				locate_OS( 3, 4 );
				PrintLine( hb, 19 );
				Bdisp_PutDisp_DD();

				if (size >= BLOCKSIZE) blocksize = BLOCKSIZE;
				else blocksize = size;
				Bfile_WriteFile_OS( handle, (const void *)work_address, blocksize );
				work_address = (void*)((unsigned int)work_address + BLOCKSIZE);
				size = size - blocksize;
			}
			Bfile_CloseFile_OS( handle );
		}else{
			locate_OS( 3, 3 );
			PrintLine( "Open failed!", 19 );
			GetKey( &i );
		}
	}
	MsgBoxPop();
	Cursor_SetFlashOn( 0 );

}


////////// TMemView //////////
//
TMemView::~TMemView(){
}

//
int TMemView::FOnCreate(){
	SetFKey( 0, "ADDR", FKT_STRING );
	SetFKey( 1, "SAVE", FKT_STRING );
	SetFKey( 2, "3PIN", FKT_STRING );
	SetFKey( 3, "FIND", FKT_STRING );
	SetFKey( 5, "SWAP", FKT_STRING );
	PViewMode = mvmHEX;
	FSource = (void*)MINADDR;

	return 0;
};

//
int TMemView::vBeforeBrowse(){
int result=TListView::vBeforeBrowse();
  return result;
}

//
int TMemView::vSourceBOL() const{
  return FSource == (void*)MINADDR;
}

//
int TMemView::vSourceEOL() const{
  return FSource == (void*)(MAXADDR-BYTESPERLINE*LINESPERPAGE);
}

//
TListViewItem*TMemView::MakeBuffer(){
int i;
unsigned char j;
char b[10];
	memset( b, 0, sizeof( b ) );
	FLineBuffer[0] = 0;
	/*
	WordToHex( (unsigned int)FSource >> 16, (unsigned char*)b );
	strcat( FLineBuffer, b );
	WordToHex( (unsigned int)FSource & 0xFFFF, (unsigned char*)b );
	*/
	LongToAscHex( (unsigned int)FSource, (unsigned char*)b, 8 );
	strcat( FLineBuffer, b );
	strcat( FLineBuffer, ":" );
	for (i=0;i<BYTESPERLINE;i++){
		if ( PViewMode == mvmHEX ){
			ByteToHex( *(unsigned char*)((int)FSource+i), (unsigned char*)b );
		}else{
			j = *(unsigned char*)((int)FSource+i);
			if ( j < ' ' ){
				b[1]='.';
			}else if ( j >= 127 ){
				b[1]='.';
			}else{
				b[1]=j;
			}
			b[0]=' ';
		}
		b[2]=0;
		strcat( FLineBuffer, b );
		if ( ( i & 3 ) == 3 )strcat( FLineBuffer, " " );
	}
	return (TListViewItem*)FLineBuffer;
};

//
int TMemView::vPrevLine(){
	if ( (unsigned int)FSource >= ( MINADDR + BYTESPERLINE ) )
		FSource = (void*)((unsigned int)FSource - BYTESPERLINE );
	return (int)MakeBuffer();
};

//
int TMemView::vNextLine(){
	if ( (unsigned int)FSource <= ( MAXADDR - BYTESPERLINE ) )
		FSource = (void*)((unsigned int)FSource + BYTESPERLINE );
	return (int)MakeBuffer();
};

//
int TMemView::vSourceGetCurrentItem( TListViewItem w ){
	memset( w, 0, sizeof( TListViewItem ) );
	strcpy( w, (char*)MakeBuffer() );
	return strlen( w );
};

//
int TMemView::vSourceJump( TListViewJump dir ){
  switch ( dir ){
    case lvjTOP : FSource = (void*)MINADDR; break;
    case lvjBOTTOM : FSource = (void*)(MAXADDR-BYTESPERLINE*LINESPERPAGE); break;
  };  
  return 0;
};

//
int TMemView::vShowAll(){
int result = TListView::vShowAll();
TStatusGlyphDefinition sgd;
	sgd.dx = STICON_WIDTH;
	sgd.dy = STICON_HEIGHT;
	sgd.xalignment = 2;
	sgd.xoffset = 1;

	if ( PViewMode == mvmHEX ) sgd.shortcolor_bitmap = GetStatusIconPtr( sticHEX );
	else sgd.shortcolor_bitmap = GetStatusIconPtr( sticASCII );
	DefineStatusGlyph( 0, &sgd );
	return result;
};

//
int TMemView::SetAddress( int addr ){
	FSource = (void*)addr;
	return addr;
};

//
int TMemView::GetAddress(){
	return (int)FSource;
};

//
int TMemView::vProcessMessage(){
int result = 1;
unsigned int start_address;
TedHEX edh;

	switch ( FMessage.message ) {
		case lvmKEY +  KEY_CTRL_F1: 
			edh.pretext = "enter address:";
			edh.value = (int)FSource;
			if ( EditDialog( edHEX, &edh ) == mrOK ){
				SetAddress( edh.value );
				SetFYDisp( -1 );
//				MessageDlg( &(edh.value), mtDebugInt );
				Refresh();
			};
			break;
		case lvmKEY +  KEY_CTRL_F2: 
			xBackupMemory( (unsigned int)FSource );
			break;
		case lvmKEY +  KEY_CTRL_F3: 
			xMemoryToSerial( (unsigned int)FSource );
			break;	
		case lvmKEY +  KEY_CTRL_F4: 
			start_address = (unsigned int)FSource;
			if ( xSearchMemory( start_address, PViewMode, PLastSearchString ) == mrOK ){
				FSource = (void*)start_address;
				Refresh();
			};
			break;
		case lvmKEY +  KEY_CTRL_EXE: 
			break;
		case lvmKEY +  KEY_CTRL_F6: 
			if ( PViewMode == mvmHEX )PViewMode = mvmASC;
			else PViewMode = mvmHEX;
			Refresh();
			break;
		default : result = 1;
	};
	return result;
};


////////// TMemView //////////

#ifdef __cplusplus
}
#endif
#endif